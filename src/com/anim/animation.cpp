#include "common/log.h"
#include "sg/transform.h"
#include "go/game_object.h"
#include "com/anim/animation.h"
#include "loader/json/json_loader.h"
#include "com/sprite2d/quad_sprite.h"
#include "action_support/action_animation.h"
#include <forward_list>
#include <rapidjson/document.h>

using namespace com;
using namespace rapidjson; // TODO: move this out of this scope

#define QUATERNION_Z(r) quaternionf(Eigen::AngleAxisf(r*M_PI/180.f, vector3f::UnitZ()))

animation::animation(game_object* go, data_stream* ds,
                     std::vector<texture_atlas*> const& atlas, int32_t idx)
: component(go), _start_index(idx) {
    if (ds != nullptr) {
        load_from(ds, atlas);
    }
}

::action::ptr animation::make_action(const std::string &clip_name) const {
    auto it = _clips.find(clip_name);
    if (it == _clips.end()) {
        LOG_WARN("animation clip couldn't be found: " << clip_name);
        return nullptr;
    }
    
    auto* anim = new act::action_animation(it->second, this);
    return ::action::ptr(anim);
}

void animation::clear() {
    // TODO:
}

bool animation::load_from(data_stream *ds, std::vector<texture_atlas*> const& atlases) {
    if (ds == nullptr || !ds->valid()) {
        LOG_WARN("the stream is not ready, ignore loading");
        return false;
    }
    
    clear();
    
    game_object* root = parent();
    
    json_document doc(ds);
    auto& json = doc.internal<rapidjson::Document>();
    auto& joints = json["bones"];
    size_t idx = 0;
    if (joints.IsArray()) {
        for (auto it = joints.Begin(); it != joints.End(); ++it, ++idx) {
            auto* name = (*it)["name"].GetString();
            size_t parent_idx = SIZE_T_MAX;
            double x = 0.f, y = 0.f, scaleX = 1.f, scaleY = 1.f;
            double rotate = 0.f;
            _names.emplace(std::piecewise_construct,
                           std::forward_as_tuple(name),
                           std::forward_as_tuple(idx));
            auto* go = new game_object(root, name);

            for (auto itr = it->MemberBegin(); itr != it->MemberEnd(); ++itr) {
                if (strcmp(itr->name.GetString(), "x") == 0) {
                    x = itr->value.GetDouble();
                } else if (strcmp(itr->name.GetString(), "y") == 0) {
                    y = itr->value.GetDouble();
                } else if (strcmp(itr->name.GetString(), "scaleX") == 0) {
                    scaleX = itr->value.GetDouble();
                } else if (strcmp(itr->name.GetString(), "scaleY") == 0) {
                    scaleY = itr->value.GetDouble();
                } else if (strcmp(itr->name.GetString(), "rotation") == 0) {
                    rotate = itr->value.GetDouble();
                } else if (strcmp(itr->name.GetString(), "parent") == 0) {
                    auto pid = _names.find(std::string(itr->value.GetString()));
                    if (pid != _names.end()) {
                        parent_idx = pid->second;
                    } else {
                        LOG_WARN("parent joint is not defined yet: " << itr->value.GetString());
                    }
                }
            }
            _transforms.emplace_back(&go->add_component<com::transform>(vector3f(x, y, 0.f),
                                                                        QUATERNION_Z(rotate),
                                                                        vector3f(scaleX, scaleY, 1.f)));
            _setup_poses.emplace_back(joint_pose{
                vector3f(x, y, 0.f),
                vector3f(scaleX, scaleY, 1.f),
                QUATERNION_Z(rotate)
            });
            if (parent_idx != SIZE_T_MAX) {
                _transforms[parent_idx]->parent()->add_child(_transforms.back()->parent());
            }
        }
    }
    
    auto& skins = json["skins"];
    if (skins.IsObject()) {
        for (auto it = skins.MemberBegin(); it != skins.MemberEnd(); ++it) {
            auto skin_ret = _skins.emplace(std::piecewise_construct,
                                           std::forward_as_tuple(it->name.GetString()),
                                           std::forward_as_tuple());
            if (!skin_ret.second) {
                LOG_WARN("the skin is dup, skipped: " << it->name.GetString());
                continue;
            }
            
            auto& skin = skin_ret.first->second;
            for (auto itr = it->value.MemberBegin(); itr != it->value.MemberEnd(); ++itr) {
                // slot name is ignored. TODO: preprocessor to reorg the data
                
                for (auto sprite_it = itr->value.MemberBegin();
                     sprite_it != itr->value.MemberEnd(); ++sprite_it) {
                    auto* name = sprite_it->name.GetString();
                    double rotation = 0.f;
                    box2f bound;
                    vector3f translate;
                    
                    for (auto attr = sprite_it->value.MemberBegin();
                         attr != sprite_it->value.MemberEnd(); ++attr) {
                        if (strcmp(attr->name.GetString(), "name") == 0) {
                            name = attr->value.GetString();
                        } else if (strcmp(attr->name.GetString(), "x") == 0) {
                            translate.x() = attr->value.GetDouble();
                        } else if (strcmp(attr->name.GetString(), "y") == 0) {
                            translate.y() = attr->value.GetDouble();
                        } else if (strcmp(attr->name.GetString(), "width") == 0) {
                            bound.min().x() = -attr->value.GetDouble() / 2.0;
                            bound.max().x() = attr->value.GetDouble() / 2.0;
                        } else if (strcmp(attr->name.GetString(), "height") == 0) {
                            bound.min().y() = -attr->value.GetDouble() / 2.0;
                            bound.max().y() = attr->value.GetDouble() / 2.0;
                        } else if (strcmp(attr->name.GetString(), "rotation") == 0) {
                            rotation = attr->value.GetDouble();
                        }
                    }

                    skin.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(name),
                                 std::forward_as_tuple(skin_piece{nullptr, bound, translate, rotation}));
                }
                
            }
        }
    }
    
    for (auto& atlas : atlases) {
        add_atlas(atlas);
    }
    
    auto& slots = json["slots"];
    if (slots.IsArray()) {
        auto& default_skin = _skins["default"];
        int32_t index = start_index();
        for (auto it = slots.Begin(); it != slots.End(); ++it) {
            auto* name = (*it)["name"].GetString();
            std::string joint = (*it)["bone"].GetString();
            auto jit = _names.find(joint);
            if (jit == _names.end()) {
                LOG_WARN("the joint is not defined, ignored: " << joint);
                continue;
            }
            _slots.emplace(std::piecewise_construct,
                           std::forward_as_tuple(name),
                           std::forward_as_tuple(jit->second));
            
            game_object* go = new game_object(_transforms[jit->second]->parent());
            go->add_component<com::transform>();
            
            auto& attachement = (*it)["attachment"];
            
            if (attachement.IsString()) {
                auto piece = default_skin.find(attachement.GetString());
                if (piece != default_skin.end() && piece->second.atlas) {
                    auto* tf = go->get_component<com::transform>();
                    auto& sp = go->add_component<sprite2d::quad_sprite>(*piece->second.atlas,
                                                                        piece->first);
                    sp.set_index(++index);

                    // FIXME: default bound if omitted
                    sp.set_bound_from_box(piece->second.bound);
                    tf->set_rotate(QUATERNION_Z(piece->second.rotation));
                    tf->set_translate(piece->second.translate);
                    
                }
            }
        }
    }

#ifdef DEBUG
    // seem to be a bug, need more investigations
    _names.find("doesn't exist");
#endif
    
    //FIXME
    ds->reset();
    _clips.emplace(std::piecewise_construct,
                   std::forward_as_tuple("test"),
                   std::forward_as_tuple(new skeleton_animation_clip(ds)));
    return true;
}

void animation::add_atlas(texture_atlas* atlas) {
    for (auto& skin : _skins) {
        for (auto& piece : skin.second) {
            if (atlas->has_frame(piece.first)) {
                piece.second.atlas = atlas;
            }
        }
    }
}
