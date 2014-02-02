#ifndef _SPRITE2D_SPRITE_H
#define _SPRITE2D_SPRITE_H

#include <vector>
#include "go/component_manager.h"
#include "go/component.h"
#include "re/vertex_layout.h"
#include "re/render_uniform.h"
#include "re/gpu_program.h"
#include "re/render_state.h"

namespace com {
    class transform;
}

// TODO: in forward declare headers
class render_device;
class vertex_index_buffer;
class render_target;
class texture;

namespace sprite2d {
    class sprite_mgr;
    class camera2d;

    struct vertices_buffer {
        // probably we could create two buffers, one for static like uv
        // and one for dynamic like position
        vertex_layout::ptr layout;
        vertex_index_buffer::ptr indices; // the bound indice buffer
    };
    
    class sprite_material {
    public:
        template<class P, class S, class U>
        sprite_material(P&& p, S&& s, U&& u)
        : _program(std::forward<P>(p)),
        _state(std::forward<S>(s)),
        _uniform(std::forward<U>(u))
        { }
        
        sprite_material(sprite_material const&rhs)
        : _program(rhs._program->retain<gpu_program>()),
        _state(rhs._state),
        _uniform(rhs._uniform)
        { }
        
        // create a new material by replacing uniforms and the state
        sprite_material set_uniforms(std::initializer_list<render_uniform::init_t> const&,
                                     render_state::const_ptr const&) const;
    private:
        gpu_program::const_ptr _program;
        render_state::const_ptr _state;
        render_uniform::const_ptr _uniform;
    };
    
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
        sprite(game_object*, size_t count /*number of vertices*/, int type/*layout*/);
        virtual ~sprite();
        
        // buffer data
        std::tuple<const void*, size_t> index_data() const {
            return std::make_tuple(_indices.data(), _indices.size());
        }
        
        // batching operation
        bool batchable(sprite const& rhs) const {
            return std::memcmp(&rhs._data, & _data, sizeof(_data)) == 0;
        }
        
        vertex_layout::ptr layout() const {
            return _data.buffer->layout->retain<vertex_layout>();
        }
        
        vertex_index_buffer::ptr index_buffer() const {
            return _data.buffer->indices->retain<vertex_index_buffer>();
        }

        void set_texture(texture*);

        //TODO: customized materials
        
    protected:
        
        // only mark for the removal, the sprite_mgr owns it
        virtual void destroy();
        
        // fill the vertices into the buffer
        // one shouldn't use more than it requested
        virtual void fill_buffer(void* buffer, size_t stride, com::transform const&) const = 0;

        // vertices are moved around the buffer, indices needs updating to
        // point to the right place without re-computing the buffer
        virtual void fill_indices() {};
        
        // generate batch/batches
        //  batched is the number of indices being shared among
        //  batchable sprites in the same vertices layout
        virtual void generate_batch(render_target*, size_t batched) const;

        indices_t _indices;
        data_t _data;

    private:
        bool _mark_for_remove;
        // uniform: texture, params, etc
        // raw vertices buffer
        
        friend class sprite_mgr;    // fill_buffer
        friend class camera2d;      // generate_batch
    };
    
    // sprite manager
    //  update the vertices data
    //  manage the buffer caches
    //  manage the materials
    class sprite_mgr : public component_manager_base<sprite_mgr> {
    public:
        // 1 bit, dirty bound/frame
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
        typedef std::vector<sprite_vertex> vertices_t;
        typedef std::vector<vertices_t> types_t;
       
        // batching layout
        // the shared vertex buffer contains all the sprites that're using
        // the same layout. the batching command would just update the indices
        // (whether it's visible or not) to the index buffer.
        struct layout_buffer {
            // sprite, start, count (number of vertices),
            typedef std::tuple<sprite*, size_t, size_t> sprite;
            typedef std::vector<sprite> sprites_t;
            
            vertex_layout::ptr layout; // FIXME: support multi-buffers?
            sprites_t sprites;
            uint32_t type_idx; // index for vertice types
        };
        typedef std::vector<layout_buffer> buffers_t; // sorted by types

        typedef std::unique_ptr<sprite_material> spt_mat_ptr;
        typedef std::vector<std::tuple<gpu_program::const_ptr, render_state::const_ptr> > materials_t; // static material data
        typedef std::vector<spt_mat_ptr> sprite_materials_t;
        
        enum { // a few default layouts and material
            position_uv = 0,
        };
        struct position_uv_t {};
        struct position_uv_color_t {};
        
        enum { // buffer configs
            Vertex_Capacity = 8096,  // number of vertices
            Indices_Capacity = 6 * 1024, // number of indices
        };
        
    public:
        sprite_mgr(render_device*);
        virtual ~sprite_mgr();

        // add the sprite to the vertex buffer, the manager will "ask" the sprite
        // to fill out the buffer and then the index buffer
        // note: the mgr will take over the ownership
        void assign_buffer(sprite*, uint32_t count, int typeIdx);

        // vertices buffer
        vertices_buffer* request_buffer(uint32_t count, int = 0);
        //uint32_t request_buffer(sprite*, uint32_t count, int = 0);
        void release_buffer(vertices_buffer*);
        
        // add a vertice layout/type, returned value to be used
        // to request the buffer
        // if the layout exists, (exactly the same while the order won't
        // matter because they will be sorted by the name), the previous
        // id will be returned w/o creating new one
        uint8_t add_type(vertices_t const&);

        // sprite material
        // TODO: add custom shaders/materials
        sprite_material* get_material(render_uniform::const_ptr const&, int = 0);
        
    protected:
        virtual void update(std::vector<game_object*> const&);
        
    private:
        vertices_buffer* create_buffer(vertices_t const&);
        
    private:
        render_device* _device;
        materials_t _materials;
        types_t _types;
        sprite_materials_t _sprite_materials;
        
        buffers_t _buffers;
    };

}
#endif