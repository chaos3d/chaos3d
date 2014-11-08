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

// single key frame
// the static/constant keyframes data for an animation
template<class Key>
struct key_frame {
    typedef std::vector<key_frame> key_frames_t;
    typedef Key key_t;

    Key key;
    float timestamp;
    
    template<class... Args>
    key_frame(float ts, Args&&... args)
    : key(std::forward<Args>(args)...), timestamp(ts)
    {}
    
    bool operator<(key_frame const& rhs) const {
        return timestamp < rhs.timestamp;
    };
};

// a collection of key frames and generic interpolation
// action_keyframe uses this to save key frames
// applier can use this to interpolate values
template<class Key>
class animation_keyframe : public std::enable_shared_from_this<animation_keyframe<Key>> {
public:
    typedef key_frame<Key> key_frame_t;
    typedef std::shared_ptr<animation_keyframe> ptr;
    typedef std::shared_ptr<animation_keyframe const> const_ptr;
    typedef std::vector<key_frame_t> key_frames_t;
    
public:
    key_frames_t const& keyframes() const {
        return _keyframes;
    }

    // lower bound of the given time
    typename key_frames_t::const_iterator keyframe(float offset) const {
        return std::lower_bound(_keyframes.begin(), _keyframes.end(), offset,
                                [] (key_frame_t const& key, float offset) {
                                    return key.timestamp < offset;
                                });
    }
    
    // regular interpolation
    template<class I, typename std::enable_if<!std::is_same<I, void>::value>::type* = nullptr>
    Key interpolate(float offset, I const& i = I()) const {
        assert(offset >= 0.f && offset <= 1.f);
        assert(_keyframes.size() > 0);
        auto it = keyframe(offset);
        if (it == _keyframes.end()) {
            return _wrap == WRAP_CLAMP ? _keyframes.back().key : _keyframes.front().key;
        } else if (it->timestamp - offset <= FLT_EPSILON ||
                   it == _keyframes.begin()) {
            return it->key;
        } else {
            auto pre = std::next(it, -1);
            return i(pre->key, it->key, (offset - pre->timestamp) / (it->timestamp - pre->timestamp),
                     (int)std::distance(pre, _keyframes.begin()));
        }
        return Key();
    }
    
    // concrete/no interpolation
    template<class I, typename std::enable_if<std::is_same<I, void>::value>::type* = nullptr>
    Key const& interpolate(float offset) const {
        assert(offset >= 0.f && offset <= 1.f);
        assert(_keyframes.size() > 0);
        auto it = keyframe(offset);
        if (it == _keyframes.end()) {
            return _wrap == WRAP_CLAMP ? _keyframes.back().key : _keyframes.front().key;
        } else if (it->timestamp - offset <= FLT_EPSILON ||
                   it == _keyframes.begin()) {
            return it->key;
        } else {
            return std::next(it, -1)->key;
        }
    }

protected:
    template<class Loader>
    animation_keyframe(Loader const&);

    animation_keyframe(int wrap, key_frames_t const& frames)
    : _wrap(wrap), _keyframes(frames) {
        normalize();
    }
    
    animation_keyframe() = default;
    
    void normalize() {
        if (_keyframes.size() == 0)
            return;
        
        std::stable_sort(_keyframes.begin(), _keyframes.end());
        auto last = _keyframes.back().timestamp;
        
        if (last < FLT_EPSILON)
            return;
        
        for (auto& it : _keyframes) {
            it.timestamp /= last;
        }
    }
    
private:
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

// helper type to extract key frame type
typedef key_frame<float> scalarf_keyframe_t;
typedef key_frame<Eigen::Vector2f> vec2_keyframe_t;
typedef key_frame<Eigen::Vector3f> vec3_keyframe_t;
typedef key_frame<Eigen::Quaternionf> quaternionf_keyframe_t;

// helper function to create keyframes derived from the given key
template<class Key>
typename animation_keyframe<Key>::ptr make_animation_keyframe(int wrap,
                                                              std::vector<key_frame<Key>> const& keyframes) {
    typedef animation_keyframe<Key> anim_kf_t;
    return anim_kf_t::create(wrap, keyframes);
}

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

// the action that applies the key frame
// the applier usually calls keyframe.interpolate
template<class Key, class Applier>
class action_keyframe : public action {
public:
    typedef typename animation_keyframe<Key>::const_ptr keyframe_ptr;
    typedef typename timer::time_t time_t;
    
public:
    template<class... Args>
    action_keyframe(time_t duration, keyframe_ptr const& keyframe,
                    timer const& t, Args&&... args)
    : _duration(duration), _keyframe(keyframe),
    _timer(t), _applier(std::forward<Args>(args)...)
    {}
    
protected:
    virtual void on_start() override {
        action::on_start();
        _start = _timer.current_time();
    }
    
    virtual void on_stop(bool skip) override {
        assert(0); // TODO
    }
    
    virtual bool done() const override {
        return _timer.current_time() - _start >= _duration && action::done();
    }
    
    virtual void update() override {
        action::update();
        
        auto normalized = (_timer.current_time() - _start) / _duration;
        _applier(*_keyframe, normalized > 1.f ? 1.f : normalized < 0.f ? 0.f : normalized);
    }
    
private:
    Applier _applier;
    keyframe_ptr _keyframe;
    time_t _duration, _start;
    timer const& _timer;
};

// helper function to create actions
template<class Key, class Applier>
action_keyframe<Key, Applier>* make_action_keyframe(Applier const& applier, time_t duration,
                                                    typename action_keyframe<Key, Applier>::keyframe_ptr const& keyframe,
                                                    timer const& t = global_timer_base::instance()) {
    return new action_keyframe<Key, Applier>(duration, keyframe, t, applier);
}

#endif