#ifndef _RENDER_TARGET_H
#define _RENDER_TARGET_H

#include <Eigen/Dense>
#include <vector>
#include <memory>
#include "re/render_batch.h"

class render_context;

class render_target : public referenced_count {
public:
    typedef Eigen::Vector2f target_size_t;
    typedef Eigen::Vector4f color_t;
    typedef std::vector<render_batch> batches_t;
    typedef std::unique_ptr<render_target, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<render_target const, referenced_count::release_deleter> const_ptr;
    
    enum { RGB565, RGBA8888, SRGBA8888 };
    enum { NODEPTH, DEPTH16, DEPTH24 };
    enum { NOSTENCIL, STENCIL8 };
    enum { NOMULTISAMPLE, MULTISAMPLE4X };
    
public:
    render_target(target_size_t const& size);
    virtual ~render_target() {};
    
    target_size_t const& size() const { return _size; };
    
    template<class... Args>
    void add_batch(Args&&... args) {
        _batches.emplace_back(std::forward<Args>(args)...);
    }

    void do_render(render_context*);
    void sort(); // TODO: comparor
    
protected:
    virtual bool bind(render_context*) = 0;
    virtual bool flush(render_context*) = 0;
    
private:
    target_size_t _size;
    batches_t _batches;
    uint8_t _color_format;
    uint8_t _depth_format;
    uint8_t _stencil_format;
    uint8_t _multi_sample;
    
    ATTRIBUTE(color_t, clear_color);
    ATTRIBUTE(bool, batch_retained);
};

class texture;
class render_texture : public render_target{
public:
    render_texture(target_size_t const& size_) :
    render_target(size_)
    {};
    
    texture* get_texture() const { return _texture; };
    
private:
    texture* _texture; // FIXME: texture ptr
};

#endif