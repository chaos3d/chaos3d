#ifndef _RENDER_TARGET_H
#define _RENDER_TARGET_H

#include <Eigen/Dense>
#include <vector>
#include "render_batch.h"

class render_target {
public:
    typedef Eigen::Vector2f target_size_t;
    typedef std::vector<render_batch> batches_t;
    
    render_target(target_size_t const& size_) :
    _size(size_)
    {};
    virtual ~render_target() {};
    
    target_size_t const& get_size() const { return _size; };
    
    void do_render();
    void sort(); // TODO: comparor
    void add_batch();
protected:
    virtual bool bind() = 0;
    virtual bool flush() = 0;
    
private:
    target_size_t _size;
    batches_t _batches;
};

class texture;
class render_texture : public render_target{
public:
    render_texture(target_size_t const& size_) :
    render_target(size_)
    {};
    
    texture* get_texture() const { return _texture; };
    
private:
    texture* _texture;
};

class render_window : public render_target {
public:
    typedef Eigen::Vector2f window_pos_t;
    
    render_window(target_size_t const& size_,
                  window_pos_t const& pos_ = window_pos_t(0.f,0.f)) :
    render_target(size_), _position(pos_)
    {};
    virtual ~render_window() {};
    
    virtual void *native_handle() = 0;
    window_pos_t const& get_position() const { return _position; };
    //void set_position();
    //void set_size();
    
    // ui events
    struct event_listener {
        virtual void touchBegan() {};
        virtual void touchMoved() {};
        virtual void touchEnded() {};
        virtual void touchCancelled() {};
        virtual void mouseMoved() {};
        virtual void keyUp() {};
    };
    
protected:
    window_pos_t _position;
};

#endif