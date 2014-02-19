#ifndef _SPRITE2D_SPRITE_H
#define _SPRITE2D_SPRITE_H

#include <vector>
#include "go/component_manager.h"
#include "go/component.h"
#include "go/game_object.h"
#include "re/vertex_layout.h"
#include "re/render_uniform.h"
#include "re/gpu_program.h"
#include "re/render_state.h"
#include "re/render_batch.h"

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
    class sprite;
    
    // batching layout
    // the shared vertex buffer contains all the sprites that're using
    // the same layout. the batching command would just update the indices
    // (whether it's visible or not) to the index buffer.
    struct layout_buffer {
        // sprite, start, count (number of vertices), old_start(moved)
        typedef std::tuple<std::unique_ptr<sprite>, uint16_t, uint16_t, uint16_t> sprite_t; // the buffer owns the sprite proxy
        typedef std::vector<sprite_t> sprites_t;
        enum { POSITION, UV, COLOR, MAX }; // common vertex channels
        
        vertex_layout::ptr layout;
        sprites_t sprites;
        uint32_t type_idx; // index for vertice types
        // removing this, there is more complicated scenario to update
        // vertices, may not worth this, besides, this only reduces one loop
        //bool need_update; // update vertex indices due to adding sprites
        std::array<int, MAX> channel_indices; // {{-1,-1,-1}};
    };
    

    // "constant" sprite material, owned by sprite_mgr
    // the batched sprites will shared the same material, so any change
    // to the material will affect all the other sprites; a new material
    // can be created based on the same settings with upated uniforms.
    class sprite_material {
        // TODO:
        // 1. move this up to the material/material mgr
        // 2. easy access to find a material
    public:
        template<class N, class P, class S, class U>
        sprite_material(N&& n, P&& p, S&& s, U&& u)
        :_name(std::forward<N>(n)),
        _program(std::forward<P>(p)),
        _state(std::forward<S>(s)),
        _uniform(std::forward<U>(u))
        { }
        
        sprite_material(sprite_material const&rhs)
        :_name(rhs._name),
        _program(rhs._program->retain<gpu_program>()),
        _state(rhs._state),
        _uniform(rhs._uniform)
        { }
        
        // create a new material by replacing uniforms and the state
        sprite_material set_uniforms(std::initializer_list<render_uniform::init_t> const&,
                                     render_state::ptr const&) const;

        std::string const& name() const { return _name; }
        gpu_program::const_ptr const& program() const { return _program; }
        render_uniform::const_ptr uniform() const { return _uniform; }
        render_state::const_ptr state() const { return _state; }
        
        // uniforms/states can be modified with caution
        // that those changes will be applied to all batched
        // sprites, hence shared.
        render_uniform* shared_uniform() { return _uniform.get(); }
        render_state* shared_state() { return _state.get(); }
        
        // compatible/batchable
        bool compatible(sprite_material const& rhs) const {
            return _program == rhs._program && *_state == *rhs._state && *_uniform == *rhs._uniform;
        }
        
        // exact the same
        bool operator==(sprite_material const& rhs) const {
            return _name == rhs._name && compatible(rhs);
        }
        
        // material order by names
        bool operator<(sprite_material const& rhs) const {
            return _name < rhs._name;
        }
    private:
        std::string _name;
        gpu_program::const_ptr _program;
        render_state::ptr _state;
        render_uniform::ptr _uniform;
        
        friend class sprite_mgr;
    };
    
    // 2d sprite
    // it can be more than 1 sprite in a single component, or rather
    // the whole 2d skeleton, so it could generate more than more
    // batches especially it uses more textures
    // TODO: aabb and culling
    class sprite : public component {
    public:
        typedef sprite_mgr manager_t;
        typedef std::vector<uint16_t> indices_t;
        
        // collected batching info
        // those two are managed by sprite_mgr
        struct data_t{
            layout_buffer* buffer;
            sprite_material* material;
            data_t() : buffer(nullptr), material(nullptr)
            {}
            
            bool operator==(data_t const& rhs) const {
                return std::memcmp(this, &rhs, sizeof(*this)) == 0;
            }
        };
        
    public:
        sprite(game_object*, size_t count /*number of vertices*/, int type/*layout*/);
        virtual ~sprite();
        
        // buffer data
        std::tuple<const void*, size_t> index_data() const {
            return std::make_tuple(_indices.data(), _indices.size() * sizeof(uint16_t));
        }
        
        // batching operation
        bool batchable(sprite const& rhs) const {
            return _data == rhs._data;
        }
        
        vertex_layout::ptr layout() const {
            return _data.buffer->layout->retain<vertex_layout>();
        }
        
        vertex_index_buffer::ptr index_buffer() const {
            return _data.buffer->layout->index_buffer();
        }

        // reset a material from a given name by replacing with the given uniforms
        bool set_material(std::string const&,
                          std::initializer_list<render_uniform::init_t> const& = {},
                          render_state::ptr const& = render_state::ptr());
        
        void set_material(sprite_material* mat) {
            if(_data.material != mat) {
                mark_dirty();
                _data.material = mat;
            }
        }
        
        // shared material
        sprite_material* shared_material() const {
            return _data.material;
        }
        
        // reset a new material by modifying the current one using the given uniforms
        void mod_material(std::initializer_list<render_uniform::init_t> const&,
                          render_state::ptr const& = render_state::ptr());
        
        void mark_dirty() const;
        
        // generate batch/batches
        //  batched is the number of indices being shared among
        //  batchable sprites in the same vertices layout
        virtual void generate_batch(render_target*, size_t start, size_t count) const;
        
    protected:
        sprite(sprite const& rhs);
        
        // only mark for the removal, the sprite_mgr owns it
        virtual void destroy();
        
        // fill the vertices into the buffer
        // one shouldn't use more than it requested
        virtual void fill_buffer(vertex_layout::locked_buffer const& buffer,
                                 com::transform const&) const = 0;

        // vertices are moved around the buffer, indices needs updating to
        // point to the right place without re-computing the buffer
        virtual void fill_indices(uint16_t start_index) = 0;
        
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
        typedef std::vector<vertices_t> types_t; // vertex layout types
       
        typedef std::vector<std::unique_ptr<layout_buffer>> buffers_t; // layout buffers, sorted by types

        typedef std::unique_ptr<sprite_material> spt_mat_ptr; // mgr owns the materials
        typedef std::vector<spt_mat_ptr> materials_t; // shared materials, sorted by names
        
        enum { // a few default layouts and material
            position_uv = 0,    // pos*4 (forth element being alpha, uv*2
        };
        
        struct position_uv_t {};
        struct position_uv_color_t {};
        
        enum { // buffer configs
            Vertex_Capacity = 4096,  // number of vertices
            Indices_Capacity = 6 * 1024, // number of indices
        };
        
    public:
        sprite_mgr(render_device*,
                   size_t vsize = Vertex_Capacity, size_t isize = Indices_Capacity,
                   std::array<std::string, layout_buffer::MAX> const& =
                   {"position", "uv", "color"} // default channel names
                   );
        virtual ~sprite_mgr();

        // add the sprite to the vertex buffer, the manager will "ask" the sprite
        // to fill out the buffer and then the index buffer
        // note: the mgr will take over the ownership
        layout_buffer* assign_buffer(sprite*, uint32_t count, uint32_t typeIdx);

        // add a vertice layout/type, returned value to be used
        // to request the buffer
        // if the layout exists, (exactly the same while the order won't
        // matter because they will be sorted by the name), the previous
        // id will be returned w/o creating new one
        uint16_t add_type(vertices_t const&);
        
        // vertex layout for a given type
        // i.e. {"position", "uv"}
        std::vector<std::string> vertex_layout(uint16_t type_idx) const;
        
        // retrieve the built-in/default channels indices 
        std::array<int, layout_buffer::MAX> map_channel(vertices_t const&) const;

        // sprite material
        // like layout types, it won't create a new one if it finds an exact same
        // one, so when do batching, it's faster
        // note, the paramters are moved away to the result
        sprite_material* add_material(std::string const&,
                                      gpu_program::const_ptr && program,
                                      render_state::ptr && state,
                                      render_uniform::ptr && uniform);
        sprite_material* add_material(std::unique_ptr<sprite_material>&&);
        sprite_material* find_first_material(std::string const&) const;
        
        materials_t const& materials() const { return _materials; }
        
        // TODO: sort sprites in the game object order so it could be
        // cache effecient
        void sort_sprites(goes_t const& gos);
        
    protected:
        virtual void update(std::vector<game_object*> const&);
        
    private:
        template<class F>
        void update_buffer(layout_buffer&, F const&);
        std::unique_ptr<layout_buffer> create_buffer(vertices_t const&);
        
    private:
        render_device* _device;
        std::array<std::string, layout_buffer::MAX> _channel_names;
        types_t _types;
        buffers_t _buffers;
        materials_t _materials;
        size_t _vertex_buffer_size;
        size_t _index_buffer_size;
    };

    inline void sprite::mark_dirty() const { parent()->set_flag(sprite_mgr::flag_offset()); }

}
#endif