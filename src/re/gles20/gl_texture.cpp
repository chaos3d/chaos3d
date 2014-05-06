#include "re/gles20/gl_texture.h"
#include "io/memory_stream.h"

#include <EGL/egl.h>
#include <GLES2/gl2ext.h>

using namespace gles20;

static GLenum _type_map [] = {
    0,  // T1D
    GL_TEXTURE_2D,  // T2D
    0,  // T3D
};

static GLenum _color_map_storage [] = {
    GL_RGBA8_OES,    //RGBA8888,
    GL_RGB565,       //RGB565,
    GL_ALPHA8_EXT,   //ALPHA,
    GL_LUMINANCE,    //LUMINANCE
    GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, //PRVTC4_RGB
    GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, //PRVTC2_RGB
    GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,//PVRTC4_RGBA
    GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,//PVRTC2_RGBA
};

static GLenum _color_map [] = {
    GL_RGBA,        //RGBA8888,
    GL_RGB,         //RGB565,
    GL_ALPHA,       //ALPHA,
    GL_LUMINANCE,   //LUMINANCE
    GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, //PRVTC4_RGB
    GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, //PRVTC2_RGB
    GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,//PVRTC4_RGBA
    GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,//PVRTC2_RGBA
};

static GLenum _filter_map [] = {
    GL_NEAREST, GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
};

static GLenum _wrap_map [] = {
    GL_CLAMP_TO_EDGE,
    GL_REPEAT,
    GL_MIRRORED_REPEAT,
};

gl_texture::gl_texture(texture::vector2i const& size, texture::attribute_t const& attr)
: texture(size, attr){
    glGenTextures(1, &_tex_id);
    glBindTexture(_type_map[attr.type], _tex_id);
    
    if (glTexStorage2DEXT != nullptr) {
        glTexStorage2DEXT(_type_map[attr.type], attr.mipmap,
                          _color_map_storage[attr.color], size[0], size[1]);
    } else {
        // FIXME: allocate mipmaps/compressed data
        glTexImage2D(_type_map[attr.type], 0, _color_map[attr.color],
                     size[0], size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filter_map[attr.min_filter]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filter_map[attr.max_filter]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrap_map[attr.wrap_s]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrap_map[attr.wrap_t]);
    
    GLNOERROR;
    glBindTexture(_type_map[attr.type], 0);
}

gl_texture::~gl_texture() {
    glDeleteTextures(1, &_tex_id);
}

bool gl_texture::generate_mipmap() {
    GLenum target = _type_map[attribute().type];
    glBindTexture(target, _tex_id);    
    glGenerateMipmap(target);
    return glGetError() == GL_NO_ERROR;
}

bool gl_texture::load(memory_stream* stream, int color, int level) {
    GLenum target = _type_map[attribute().type];
    glBindTexture(target, _tex_id);

    // TODO: fix width based on mipmap level
    switch (color) {
        case RGBA8888:
            glTexSubImage2D(target, level, 0, 0, size()[0], size()[1],
                            GL_RGBA, GL_UNSIGNED_BYTE, stream->address());
            break;
        case RGB565:
            glTexSubImage2D(target, level, 0, 0, size()[0], size()[1],
                            GL_RGB, GL_UNSIGNED_SHORT_5_6_5, stream->address());
            break;
        case ALPHA:
            glTexSubImage2D(target, level, 0, 0, size()[0], size()[1],
                            GL_ALPHA, GL_UNSIGNED_BYTE, stream->address());
        case LUMINANCE:
            glTexSubImage2D(target, level, 0, 0, size()[0], size()[1],
                            GL_LUMINANCE, GL_UNSIGNED_BYTE, stream->address());
            break;
        case PVRTC4_RGBA:
            glCompressedTexSubImage2D(target, level, 0, 0, size()[0], size()[1],
                                      GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
                                      (GLsizei)stream->size(), stream->address());
        case PVRTC2_RGBA:
            glCompressedTexSubImage2D(target, level, 0, 0, size()[0], size()[1],
                                      GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
                                      (GLsizei)stream->size(), stream->address());
            break;
        case PVRTC4_RGB:
            glCompressedTexSubImage2D(target, level, 0, 0, size()[0], size()[1],
                                      GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,
                                      (GLsizei)stream->size(), stream->address());
            break;
        case PVRTC2_RGB:
            glCompressedTexSubImage2D(target, level, 0, 0, size()[0], size()[1],
                                      GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,
                                      (GLsizei)stream->size(), stream->address());
            break;
    }

    GLNOERROR;
    return true;
}
