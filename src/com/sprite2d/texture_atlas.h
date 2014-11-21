#ifndef _CHAOS3D_TEXTURE_ATLAS_H
#define _CHAOS3D_TEXTURE_ATLAS_H

#include "common/log.h"
#include "common/utility.h"
#include "common/base_types.h"
#include "common/referenced_count.h"
#include "re/texture.h"
#include <string>
#include <unordered_map>
#include <Eigen/Geometry>

// TODO: move up for general purposes?
// TODO: use shared_ptr
class texture_atlas : public referenced_count {
public:
    typedef std::unique_ptr<texture_atlas, release_deleter> ptr;
    typedef std::unique_ptr<texture_atlas const, release_deleter> const_ptr;
    
    // TODO: frame spacing
    typedef std::array<vector2f, 4> sprite_v_t;
    typedef std::unordered_map<std::string, sprite_v_t> rects_t;
    
    // tags for different format
    struct TexturePacker {};
    
public:
    // get the underlying texture
    texture::ptr const& texture_ptr() const { return _texture; }
    
    // get the named frame
    // if the name doesn't exist, return the empty box (0,0)
    sprite_v_t const& get_frame(std::string const& name) const {
        static sprite_v_t _empty{{}};
        auto it = _rects.find(name);
        return it != _rects.end() ? it->second : _empty;
    }
    
    /// whether this atlas has the named sprite
    bool has_frame(std::string const& name) const {
        return _rects.find(name) != _rects.end();
    }
    
    // the texture size
    texture::vector2i const& size() const {
        return _texture->size();
    }
    
    /// add a frame for the name
    texture_atlas& add_frame(std::string const& name, sprite_v_t const& box) {
        auto it = _rects.find(name);
        if (it != _rects.end()) {
            // TODO: log error
        } else {
            _rects.emplace(std::piecewise_construct,
                           std::forward_as_tuple(name),
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