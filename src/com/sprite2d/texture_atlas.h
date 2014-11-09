#ifndef _CHAOS3D_TEXTURE_ATLAS_H
#define _CHAOS3D_TEXTURE_ATLAS_H

#include <string>
#include <unordered_map>
#include <Eigen/Geometry>
#include "common/utility.h"
#include "common/log.h"
#include "common/referenced_count.h"
#include "re/texture.h"

// TODO: move up for general purposes?
// TODO: use shared_ptr
class texture_atlas : public referenced_count {
public:
    typedef std::unique_ptr<texture_atlas, release_deleter> ptr;
    typedef std::unique_ptr<texture_atlas const, release_deleter> const_ptr;
    
    // TODO: frame spacing
    typedef Eigen::AlignedBox2f box2f; // uv coordinate
    typedef std::unordered_map<std::string, box2f> rects_t;
    
    // tags for different format
    struct TexturePacker {};
    
public:
    // get the underlying texture
    texture::ptr const& texture_ptr() const { return _texture; }
    
    // get the named frame
    // if the name doesn't exist, return the empty box (0,0)
    box2f const& get_frame(std::string const& name) const {
        static box2f _empty(2);
        auto it = _rects.find(name);
        return it != _rects.end() ? it->second : _empty;
    }
    
    // the texture size
    texture::vector2i const& size() const {
        return _texture->size();
    }
    
    texture_atlas& add_frame(std::string const& name, box2f const& box) {
        auto it = _rects.find(name);
        if (it != _rects.end()) {
            // TODO: log error
        } else {
            _rects.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                           std::forward_as_tuple(box));
        }
        return *this;
    }
    
private:
    texture::ptr _texture;
    rects_t _rects;
    
    CONSTRUCT_FROM_LOADER(texture_atlas);
};

#endif