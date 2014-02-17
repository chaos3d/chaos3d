#ifndef _CHAOS3D_ACTION_KEYFRAME_H
#define _CHAOS3D_ACTION_KEYFRAME_H

#include <vector>
#include <algorithm>
#include <cfloat>
#include "action/action.h"

// The static/constant keyframes data for an animation
//
template <class Key>
class animation_keyframe {
public:
    struct key_frame {
        Key key;
        float timestamp;

		bool operator<(key_frame const& rhs) const {
			return timestamp < rhs.timestamp;
		};
    };
    
    enum { WRAP_CLAMP, WRAP_LOOP };
    
    typedef std::vector<key_frame> key_frames_t;
    
public:
    template <class... Args>
    action_keyframe(int wrap, Args&&... args)
    : _wrap(wrap), _keyframes(std::forward<Args>(args)...) {
        normalize();
    }
    
    key_frames_t const& keyframes() const {
        return _keyframes;
    }

    // lower bound of the given time
    typename key_frames_t::const_iterator keyframe(float offset) const {
        return std::lower_bound(_keyframes.begin(), _keyframes.end(), offset,
                                [] (key_frame const& key, float offset) {
                                    return key.timestamp < offset;
                                });
    }
    
    // regular interpolation
    template <class I, typename std::enable_if<!std::is_same<I, void>::value>::type* = nullptr>
    key_frame interpolate(float offset, I i = I()) const {
        assert(offset >= 0.f && offset <= 1.f);
        assert(_keyframes.size() > 0);
        auto it = keyframe(offset);
        if (it == _keyframes.end()) {
            return _wrap == WRAP_CLAMP ? _keyframes.back() : _keyframes.front();
        } else if (it->timestamp - offset <= FLT_EPSILON ||
                   it == _keyframes.begin()) {
            return *it;
        } else {
            auto pre = std::next(it, -1);
            return i(*pre, *it, (offset - pre->timestamp) / (it->timestamp - pre->timestamp),
                     std::distance(pre, _keyframes.begin()));
        }
        return key_frame();
    }
    
    // concrete/no interpolation
    template <class I, typename std::enable_if<std::is_same<I, void>::value>::type* = nullptr>
    key_frame const& interpolate(float offset) const {
        assert(offset >= 0.f && offset <= 1.f);
        assert(_keyframes.size() > 0);
        auto it = keyframe(offset);
        if (it == _keyframes.end()) {
            return _wrap == WRAP_CLAMP ? _keyframes.back() : _keyframes.front();
        } else {
            return *it;
        }
    }
    
protected:
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
    int _wrap;
};

template<class Key>
struct interpolator_linear {
    typedef animation_keyframe<Key>::key_frame key_frame;
    inline key_frame operator() (key_frame const& from, key_frame const& to,
                                 float p, int) const {
        return from + to * p;
    }
};

class action_keyframe : public action {
protected:
    virtual void on_start() override;
    virtual void on_end() override;
    virtual void on_stop(bool skip) override;
    virtual bool done() const override;
    virtual void update() override;
};
#endif