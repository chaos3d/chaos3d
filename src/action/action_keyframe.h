#ifndef _CHAOS3D_ACTION_KEYFRAME_H
#define _CHAOS3D_ACTION_KEYFRAME_H

#include <vector>
#include <memory>
#include <algorithm>
#include <cfloat>
#include "Eigen/Geometry"

#include "action/action.h"
#include "common/timer.h"
#include "common/utility.h"

// TODO0: better namespace
// TODO: WRAP_FULL_LOOP
enum { WRAP_CLAMP, WRAP_LOOP };
enum { STEP, LINEAR, MAX_TYPE };

struct keyframe_info {
    timer::time_t offset;  // time frame
    uint8_t type;   // interpolation type
};

// void is for no-interpolation
// template<class Key>
// struct interpolator_concrete {};

// linear/slerp interpolator that can be used by animation_keyframe
// to plug into the generic interpolation
template<class Key>
struct interpolator_linear {
    inline Key operator() (Key const& from, Key const& to,
                           float p, int) const {
        return from * (1.f - p) + to * p;
    }
};

template<class Key>
struct interpolator_slerp {
    inline Key operator() (Key const& from, Key const& to,
                           float p, int) const {
        return from.slerp(p, to);
    }
};

// a collection of key frames and generic interpolation
// action_keyframe uses this to save key frames
// applier can use this to interpolate values
// TODO: move this to a different file
template<class Key>
class animation_keyframe : public std::enable_shared_from_this<animation_keyframe<Key>> {
public:
    typedef timer::time_t time_t;
    typedef Key key_t;              // key type
    typedef keyframe_info frame_info;
    
    typedef std::vector<frame_info> frame_infos_t;
    typedef std::vector<key_t> key_frames_t;
    typedef std::shared_ptr<animation_keyframe> ptr;
    typedef std::shared_ptr<animation_keyframe const> const_ptr;
    
    // helper struct for initialize the data, TODO: remove this
    struct key_frame_t {
        typedef Key key_t;
        time_t timestamp;
        key_t key;
    
        template<class... Args>
        key_frame_t(time_t ts, Args&&... args)
        : key(std::forward<Args>(args)...), timestamp(ts)
        {}
    };
    
public:
    key_frames_t const& keyframes() const {
        return _keyframes;
    }

    // lower bound of the given time
    typename frame_infos_t::const_iterator keyframe(time_t offset) const {
        return std::lower_bound(_frame_infos.begin(), _frame_infos.end(), offset,
                                [] (frame_info const& key, time_t const& offset) {
                                    return key.offset < offset;
                                });
    }
    
    // regular interpolation
    template<class I, typename std::enable_if<!std::is_same<I, void>::value>::type* = nullptr>
    key_t interpolate(float offset, I const& i = I()) const {
        return interpolate<I>(keyframe(offset), offset, i);
    }
    
    template<class I, typename std::enable_if<!std::is_same<I, void>::value>::type* = nullptr>
    key_t interpolate(frame_infos_t::const_iterator const& fit, float offset, I const& i = I()) const {
        assert(offset >= 0.f && offset <= 1.f);
        assert(_keyframes.size() > 0);
        auto& timestamp = fit;
        auto it = _keyframes.begin() + std::distance(_frame_infos.begin(), timestamp);
        if (it == _keyframes.end()) {
            return _wrap == WRAP_CLAMP ? _keyframes.back() : _keyframes.front();
        } else if (timestamp->offset - offset <= FLT_EPSILON ||
                   it == _keyframes.begin()) {
            return *it;
        } else {
            auto pre = std::next(it, -1);
            auto pre_ts = std::next(timestamp, -1);
            return i(*pre, *it, (offset - pre_ts->offset) / (timestamp->offset - pre_ts->offset),
                     (int)std::distance(_keyframes.begin(), pre));
        }
    }
    
    // concrete/no interpolation
    template<class I, typename std::enable_if<std::is_same<I, void>::value>::type* = nullptr>
    key_t const& interpolate(float offset) const {
        return interpolate<void>(keyframe(offset), offset);
    }
    
    template<class I, typename std::enable_if<std::is_same<I, void>::value>::type* = nullptr>
    key_t const& interpolate(frame_infos_t::const_iterator const& fit, float offset) const {
        assert(offset >= 0.f && offset <= 1.f);
        assert(_keyframes.size() > 0);
        auto& timestamp = fit;
        auto it = _keyframes.begin() + std::distance(_frame_infos.begin(), timestamp);
        if (it == _keyframes.end()) {
            return _wrap == WRAP_CLAMP ? _keyframes.back() : _keyframes.front();
        } else if (it == _keyframes.begin()) {
            return *it;
        } else {
            return *std::next(it, -1);
        }
    }

    template<class I, class... Is>
    key_t interpolate_in_frame_helper(frame_infos_t::const_iterator const& fit,
                                      float offset, int idx) const {
        if (sizeof...(Is) == idx) {
            return interpolate<I>(fit, offset);
        } else {
            return interpolate_in_frame_helper<Is...>(fit, offset, idx);
        }
    }

    template<class... Is, typename std::enable_if<sizeof...(Is) == 0>::type* = nullptr>
    key_t interpolate_in_frame_helper(frame_infos_t::const_iterator const& fit,
                                      float offset, int idx) const {
        return key_t();
    }
    
    template<class... Is>
    key_t interpolate_in_frame(float offset) const {
        assert(offset >= 0.f && offset <= 1.f);
        assert(_keyframes.size() > 0);
        auto fit = keyframe(offset);
        assert(fit->type < sizeof...(Is));
        return interpolate_in_frame_helper<Is...>(fit, offset, fit->type);
    }

protected:
    template<class Loader>
    animation_keyframe(Loader const&);

    // helper constructor for existing Lua binding. TODO: remove this
    animation_keyframe(int wrap, std::vector<key_frame_t> const& keyframes)
    : _wrap(wrap) {
        _frame_infos.reserve(keyframes.size());
        _keyframes.reserve(keyframes.size());
        
        for (auto& it : keyframes) {
            _frame_infos.push_back({it.timestamp});
            _keyframes.push_back(it.key);
        }
        normalize();
    }
    
    animation_keyframe(int wrap, frame_infos_t const& infos, key_frames_t const& frames)
    : _wrap(wrap), _keyframes(frames), _frame_infos(infos) {
        normalize();
    }
    
    animation_keyframe() = default;
    
    // sort the time bounds and scale it to [0,1]
    void normalize() {
        if (_keyframes.size() == 0)
            return;
        
        struct time_index {
            size_t idx;
            time_t time;
        };
        time_index indices[_frame_infos.size()];
        auto it = _frame_infos.begin();
        std::generate_n(indices, _frame_infos.size(), [&] {
            size_t idx = std::distance(_frame_infos.begin(), it);
            return time_index{ idx, (it ++)->offset };
        });
        std::stable_sort(indices, indices + _frame_infos.size(),
                         [] (time_index const& lhs, time_index const& rhs) {
                             return lhs.time < rhs.time;
                         });

        for (size_t i = 0; i < _frame_infos.size(); ++i) {
            while (indices[i].idx != i) {
                std::swap(_frame_infos[i], _frame_infos[indices[i].idx]);
                std::swap(_keyframes[i], _keyframes[indices[i].idx]);
                std::swap(indices[i], indices[indices[i].idx]);
            }
        }
        
        auto last = _frame_infos.back().offset;
        if (last < FLT_EPSILON)
            return;
        
        for (auto& it : _frame_infos) {
            it.offset /= last;
        }
    }
    
private:
    /// key frames and times bound (separated)
    frame_infos_t _frame_infos;
    key_frames_t _keyframes; // key frames are constant
    int _wrap = WRAP_CLAMP;
    
    CONSTRUCTOR_FOR_SHARED(animation_keyframe);
};

// often used types of keys in frames
typedef animation_keyframe<float> scalarf_anim_kf_t;
typedef animation_keyframe<Eigen::Vector2f> vec2f_anim_kf_t;
typedef animation_keyframe<Eigen::Vector3f> vec3f_anim_kf_t;
typedef animation_keyframe<Eigen::Quaternionf> quaternionf_anim_kf_t;

// helper type to extract keyframes type
typedef typename scalarf_anim_kf_t::key_frames_t scalarf_keyframes_t;
typedef typename vec2f_anim_kf_t::key_frames_t vec2f_keyframes_t;
typedef typename vec3f_anim_kf_t::key_frames_t vec3f_keyframes_t;
typedef typename quaternionf_anim_kf_t::key_frames_t quaternionf_keyframes_t;

// helper function to create keyframes derived from the given key
template<class Key>
using anim_kf_times_t = typename animation_keyframe<Key>::times_t;

template<class Key>
using anim_kf_keyframes_t = typename animation_keyframe<Key>::key_frames_t;

template<class Key>
typename animation_keyframe<Key>::ptr make_animation_keyframe(int wrap,
                                                              anim_kf_times_t<Key> const& times,
                                                              anim_kf_keyframes_t<Key> const& keyframes) {
    return animation_keyframe<Key>::create(wrap, times, keyframes);
}

// the action that applies the key frame
// the applier usually calls keyframe.interpolate
template<class Key>
class action_keyframe : public action {
public:
    typedef typename animation_keyframe<Key>::const_ptr keyframe_ptr;
    typedef typename timer::time_t time_t;
    typedef std::function<void (animation_keyframe<Key> const&, time_t)> applier_t;
    
public:
    template<class... Args>
    action_keyframe(time_t duration, keyframe_ptr const& keyframe,
                    timer const& t, Args&&... args)
    : _duration(duration), _keyframe(keyframe), _loop(duration),
    _timer(t), _applier(std::forward<Args>(args)...)
    {}
    
    void set_loop(time_t loop) { _loop = loop; }
    
protected:
    virtual void on_start() override {
        action::on_start();
        _start = _timer.current_time();
    }
    
    virtual void on_stop(bool skip) override {
        assert(0); // TODO
    }
    
    virtual bool done() const override {
        return _duration > 0.f // infinite loop if duration is negative or 0
            && (_timer.current_time() - _start >= _duration && action::done());
    }
    
    virtual void update() override {
        action::update();
        
        auto elapsed = _timer.current_time() - _start;
        auto normalized = fmod(elapsed, _loop) / _loop;
        _applier(*_keyframe,
                 (_duration > 0.f && elapsed >= _duration) ? 1.f
                 : normalized < 0.f ? 0.f
                 : normalized);
    }
    
private:
    applier_t _applier;
    keyframe_ptr _keyframe;
    time_t _start;      // start time
    time_t _duration;   // the entire duration
    time_t _loop;       // the duration for each piece
    timer const& _timer;
};

// helper function to create actions
template<class Key>
using action_kf_keyframe_ptr = typename action_keyframe<Key>::keyframe_ptr;

template<class Key, class Applier>
action::ptr make_action_keyframe(Applier const& applier, time_t duration,
                                 action_kf_keyframe_ptr<Key> const& keyframe,
                                 timer const& t = global_timer_base::instance()) {
    return action::ptr(new action_keyframe<Key>(duration, keyframe, t, applier));
}

#endif