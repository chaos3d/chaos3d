#include "com/anim/animation_clip.h"
#include "common/log.h"
#include "sg/transform.h"
#include "io/data_stream.h"
#include "loader/json/json_loader.h"
#include <rapidjson/document.h>

using namespace com;

animation_clip::animation_clip(data_stream* ds) {
    load_from(ds);
}

typedef rapidjson::GenericValue<rapidjson::UTF8<char>> json_value_t;

template<>
animation_clip::animation_clip(json_value_t const& anim) {
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
            _channels.emplace_back(new clip_channel());
            auto *channel = _channels.back().get();
            
            for (auto anim = bone->value.MemberBegin();
                 anim != bone->value.MemberEnd(); ++anim) {
                double x = DBL_MAX, y = DBL_MAX, angle = DBL_MAX;
                double ts = 0.0;
                uint8_t interpolation = LINEAR;
                int type = 0; // 0, translate, 1, scale, 2, rotate
                std::vector<keyframe_info> infos;
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
                            ts = attr->value.GetDouble();
                        } else if (strcmp(attr->name.GetString(), "x") == 0) {
                            x = attr->value.GetDouble();
                        } else if (strcmp(attr->name.GetString(), "y") == 0) {
                            y = attr->value.GetDouble();
                        } else if (strcmp(attr->name.GetString(), "angle") == 0) {
                            angle = attr->value.GetDouble();
                        }
                    }
                    infos.push_back(keyframe_info{.offset =  ts, .type = interpolation});
                    
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
                                                                     infos,
                                                                     v3f_kf);
                } else if (type == 1) {
                    channel->scale = scale_channel_t::create(WRAP_CLAMP,
                                                             infos,
                                                             v3f_kf);
                } else if (type == 2) {
                    channel->rotate = rotate_channel_t::create(WRAP_CLAMP,
                                                               infos,
                                                               qf_kf);
                }
            }
        }
    }
}

void animation_clip::load_from(data_stream *ds) {
    if (ds == nullptr || !ds->valid()) {
        LOG_WARN("the stream is not ready, ignore loading");
        return;
    }
}

uint32_t animation_clip::get_channels(names_t const& names,
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

