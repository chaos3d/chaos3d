#ifndef _CHAOS3D_TEXTURE_ATLAS_H
#define _CHAOS3D_TEXTURE_ATLAS_H

#include <string>
#include <unordered_map>
#include <Eigen/Geometry>
#include "common/utility.h"
#include "re/texture.h"

// TODO: move up for general purposes?
class texture_atlas {
public:
    // TODO: frame spacing
    typedef Eigen::AlignedBox2f box2f;
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
    
private:
    texture::ptr _texture;
    rects_t _rects;
    
    CONSTRUCT_FROM_LOADER(texture_atlas);
};

#endif