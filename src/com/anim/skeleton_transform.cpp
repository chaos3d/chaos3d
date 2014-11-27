#include "com/anim/skeleton_transform.h"
#include "common/log.h"
#include "sg/transform.h"
#include "io/data_stream.h"
#include "loader/json/json_loader.h"
#include <rapidjson/document.h>

using namespace com;

skeleton_animation_clip::skeleton_animation_clip(data_stream* ds) {
    load_from(ds);
}

typedef rapidjson::GenericValue<rapidjson::UTF8<char>> json_value_t;

template<>
skeleton_animation_clip::skeleton_animation_clip(json_value_t const& anim) {
    if (!anim.IsObject()) {
        LOG_WARN("animation is not a JSON object, loading ignored");
        return;
    }
    
    // TODO: other slots/events?
    auto& bones = anim["bones"];
    if (bones.IsObject()) {
        for (auto bone = bones.MemberBegin(); bone != bones.MemberEnd(); ++bone) {
            // name -> index (last element)
            _names.emplace(bone->name.GetString(), _channels.size());
            
            // new channel
            _channels.emplace_back(new joint_channel());
            auto *channel = _channels.back().get();
            
            for (auto anim = bone->value.MemberBegin();
                 anim != bone->value.MemberEnd(); ++anim) {
                double x = DBL_MAX, y = DBL_MAX, angle = DBL_MAX;
                int type = 0; // 0, translate, 1, scale, 2, rotate
                std::vector<time_t> times;
                std::vector<vector3f> v3f_kf;
                std::vector<quaternionf> qf_kf;
                
                if (strcmp(anim->name.GetString(), "translate") == 0) {
                    type = 0;
                } else if (strcmp(anim->name.GetString(), "scale") == 0) {
                    type = 1;
                } else if (strcmp(anim->name.GetString(), "rotate") == 0) {
                    type = 2;
                } else {
                    LOG_WARN("not supported animation type: " << anim->name.GetString());
                    continue;
                }
                
                for (auto each = anim->value.Begin();
                     each != anim->value.End(); ++each) {
                    for (auto attr = each->MemberBegin();
                         attr != each->MemberEnd(); ++attr) {
                        if (strcmp(attr->name.GetString(), "time") == 0) {
                            times.emplace_back(attr->value.GetDouble());
                        } else if (strcmp(attr->name.GetString(), "x") == 0) {
                            x = attr->value.GetDouble();
                        } else if (strcmp(attr->name.GetString(), "y") == 0) {
                            y = attr->value.GetDouble();
                        } else if (strcmp(attr->name.GetString(), "angle") == 0) {
                            angle = attr->value.GetDouble();
                        }
                    }
                    
                    if (type == 0) {
                        v3f_kf.emplace_back(x == DBL_MAX ? 0.f : x,
                                            y == DBL_MAX ? 0.f : y, 0.f);
                    } else if (type == 1) {
                        v3f_kf.emplace_back(x == DBL_MAX ? 1.f : x,
                                            y == DBL_MAX ? 1.f : y, 1.f);
                    } else if (type == 2) {
                        qf_kf.emplace_back(Eigen::AngleAxisf(angle*M_PI/180.f,
                                                             vector3f::UnitZ()));
                    }
                }
                
                if (type == 0) {
                    channel->translate = translate_channel_t::create(WRAP_CLAMP,
                                                                     times,
                                                                     v3f_kf);
                } else if (type == 1) {
                    channel->scale = scale_channel_t::create(WRAP_CLAMP,
                                                             times,
                                                             v3f_kf);
                } else if (type == 2) {
                    channel->rotate = rotate_channel_t::create(WRAP_CLAMP,
                                                               times,
                                                               qf_kf);
                }
            }
        }
    }
    
}

void skeleton_animation_clip::load_from(data_stream *ds) {
    if (ds == nullptr || !ds->valid()) {
        LOG_WARN("the stream is not ready, ignore loading");
        return;
    }

    json_document doc(ds);
    auto& json = doc.internal<rapidjson::Document>();

    // FIXME: refactor loading process
    // only testing the concept now

    auto& animations = json["animations"];
    if (animations.IsObject()) {
        for (auto anim = animations.MemberBegin();
             anim != animations.MemberEnd(); ++anim) {
            
            auto& bones = anim->value["bones"];
            if (bones.IsObject()) {
                for (auto bone = bones.MemberBegin(); bone != bones.MemberEnd(); ++bone) {
                    // name -> index (last element)
                    _names.emplace(bone->name.GetString(), _channels.size());
                    
                    // new channel
                    _channels.emplace_back(new joint_channel());
                    auto *channel = _channels.back().get();
                    
                    for (auto anim = bone->value.MemberBegin();
                         anim != bone->value.MemberEnd(); ++anim) {
                        double x = DBL_MAX, y = DBL_MAX, angle = DBL_MAX;
                        int type = 0; // 0, translate, 1, scale, 2, rotate
                        std::vector<time_t> times;
                        std::vector<vector3f> v3f_kf;
                        std::vector<quaternionf> qf_kf;
                        
                        if (strcmp(anim->name.GetString(), "translate") == 0) {
                            type = 0;
                        } else if (strcmp(anim->name.GetString(), "scale") == 0) {
                            type = 1;
                        } else if (strcmp(anim->name.GetString(), "rotate") == 0) {
                            type = 2;
                        } else {
                            LOG_WARN("not supported animation type: " << anim->name.GetString());
                            continue;
                        }
                        
                        for (auto each = anim->value.Begin();
                             each != anim->value.End(); ++each) {
                            for (auto attr = each->MemberBegin();
                                 attr != each->MemberEnd(); ++attr) {
                                if (strcmp(attr->name.GetString(), "time") == 0) {
                                    times.emplace_back(attr->value.GetDouble());
                                } else if (strcmp(attr->name.GetString(), "x") == 0) {
                                    x = attr->value.GetDouble();
                                } else if (strcmp(attr->name.GetString(), "y") == 0) {
                                    y = attr->value.GetDouble();
                                } else if (strcmp(attr->name.GetString(), "angle") == 0) {
                                    angle = attr->value.GetDouble();
                                }
                            }
                            
                            if (type == 0) {
                                v3f_kf.emplace_back(x == DBL_MAX ? 0.f : x,
                                                    y == DBL_MAX ? 0.f : y, 0.f);
                            } else if (type == 1) {
                                v3f_kf.emplace_back(x == DBL_MAX ? 1.f : x,
                                                    y == DBL_MAX ? 1.f : y, 1.f);
                            } else if (type == 2) {
                                qf_kf.emplace_back(Eigen::AngleAxisf(angle*M_PI/180.f,
                                                                     vector3f::UnitZ()));
                            }
                        }
                        
                        if (type == 0) {
                            channel->translate = translate_channel_t::create(WRAP_CLAMP,
                                                                             times,
                                                                             v3f_kf);
                        } else if (type == 1) {
                            channel->scale = scale_channel_t::create(WRAP_CLAMP,
                                                                     times,
                                                                     v3f_kf);
                        } else if (type == 2) {
                            channel->rotate = rotate_channel_t::create(WRAP_CLAMP,
                                                                       times,
                                                                       qf_kf);
                        }
                    }
                }
            }
            
            // testing now, only load the first animation
            break;
        }
    }
}

uint32_t skeleton_animation_clip::get_channels(names_t const& names,
                                               channels_t& channels) const {
    uint16_t count = 0;
    for (auto& name : names) {
        auto idx = _names.find(name.first);
        if (idx == _names.end())
            continue;
        
        if (name.second >= channels.size()) {
            LOG_WARN("the joint index is greater than the # of channels:" << name.first);
            continue;
        }
        
        channels[name.second] = _channels[idx->second].get();
        ++ count;
    }
    return count;
}

