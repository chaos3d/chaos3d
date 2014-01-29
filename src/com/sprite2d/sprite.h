#ifndef _SPRITE2D_SPRITE_H
#define _SPRITE2D_SPRITE_H

#include <vector>
#include "go/component_manager.h"
#include "com/render/renderable.h"

namespace com {
    class transform;
}

// TODO: in forward declare headers
class render_device;
class vertex_layout;
class vertex_index_buffer;
class render_uniform;
class render_state;
class gpu_program;
class texture;

namespace sprite2d {
    class sprite_mgr;

    struct vertices_buffer {
        // probably we could create two buffers, one for static like uv
        // and one for dynamic like position
        vertex_layout* layout;
        vertex_index_buffer* indices; // the bound indice buffer
    };
    
    // TODO: mutable uniforms per sprite, how?
    typedef std::tuple<gpu_program *, const render_state *, const render_uniform *> sprite_material;
    
    // 2d sprite
    // it can be more than 1 sprite in a single component, or rather
    // the whole 2d skeleton, so it could generate more than more
    // batches especially it uses more textures
    class sprite : public component {
        
        // TODO: this should be a base class, the subclass may
        // have different types of sprites and be templatized
    public:
        typedef sprite_mgr manager_t;
        typedef std::vector<uint16_t> indices_t;
        struct data_t{ //TODO: combine static and dynamic
            vertices_buffer* buffer;
            sprite_material* material;
            data_t() : buffer(nullptr), material(nullptr)
            {}
        };
        
    public:
        sprite(game_object*, texture*);
        ~sprite();
        
        std::tuple<const void*, size_t> indics_buffer() const {
            return std::make_tuple(_indices.data(), _indices.size());
        }
        
        bool batchable(sprite const& rhs) const {
            return std::memcmp(&rhs._data, & _data, sizeof(_data)) == 0;
        }
        
        data_t const& data() const { return _data; }
        
        void set_texture(texture*);

        //TODO: customized materials
    protected:
        virtual void fill_buffer(com::transform*);

    private:
        indices_t _indices;
        
        data_t _data;
        // uniform: texture, params, etc
        // raw vertices buffer
        
        friend class sprite_mgr;
    };
    
    // sprite manager
    //  update the vertices data
    //  manage the buffer caches
    //  manage the materials
    class sprite_mgr : public component_manager_base<sprite_mgr> {
    public:
        typedef std::integral_constant<uint32_t, 1> flag_bit_t;
        
        // this can be esculated to the generic mgr
        struct sprite_vertex {
            std::string name;
            int type;
            int count;
            
            bool operator ==(sprite_vertex const& rhs) const {
                return type == rhs.type && count == rhs.type && name == rhs.name;
            }
        };
        typedef std::unique_ptr<sprite_material> spt_mat_ptr;
        typedef std::vector<sprite_vertex> vertices_t;
        typedef std::vector<vertices_t> types_t;
        typedef std::vector<std::tuple<gpu_program*, const render_state*>> materials_t; // static material data
        typedef std::vector<spt_mat_ptr> sprite_materials_t;
        
        enum { // a few default layouts and material
            position_uv = 0,
        };
        
        enum { // buffer configs
            Vertex_Capacity = 8096,  // number of vertices
            Indices_Capacity = 6 * 1024, // number of indices
        };
        
    public:
        sprite_mgr(render_device*);
        virtual ~sprite_mgr();
        
        // vertices buffer
        vertices_buffer* request_buffer(uint32_t count, int = 0);
        void release_buffer(vertices_buffer*);
        
        // add a vertice layout/type, returned value to be used
        // to request the buffer
        // if the layout exists, (exactly the same while the order won't
        // matter because they will be sorted by the name), the previous
        // id will be returned w/o creating new one
        uint8_t add_type(vertices_t const&);

        // sprite material
        // TODO: add custom shaders/materials
        sprite_material* get_material(std::unique_ptr<render_uniform>&&, int = 0);
        
    protected:
        virtual void update(std::vector<game_object*> const&);
        
    private:
        vertices_buffer* create_buffer(vertices_t const&);
        
    private:
        render_device* _device;
        materials_t _materials;
        types_t _types;
        sprite_materials_t _sprite_materials;
    };
}
#endif