#ifndef _CHAOS3D_RE_MATERIAL_H
#define _CHAOS3D_RE_MATERIAL_H

#include <string>
#include <memory>
#include <vector>
#include "gpu_program.h"
#include "render_state.h"
#include "render_uniform.h"

class material_mgr;

/// the collection of attributes used in rendering and their values
class material {
public:
    typedef std::unique_ptr<material> ptr;

public:
    template<class N, class P, class S, class U>
    material(N&& n, P&& p, S&& s, U&& u)
    : _name(std::forward<N>(n)),
    _program(std::forward<P>(p)),
    _state(std::forward<S>(s)),
    _uniform(std::forward<U>(u))
    { }

    material(material const&rhs)
    :_name(rhs._name),
    _program(rhs._program->retain<gpu_program>()),
    _state(rhs._state),
    _uniform(rhs._uniform)
    { }

    // create a new material by replacing uniforms and the state
    material set_uniforms(std::initializer_list<render_uniform::init_t> const&,
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
    bool compatible(material const& rhs) const {
        return _program == rhs._program && *_state == *rhs._state && *_uniform == *rhs._uniform;
    }

    // exact the same
    bool operator==(material const& rhs) const {
        return _name == rhs._name && compatible(rhs);
    }

    // material order by names
    bool operator<(material const& rhs) const {
        return _name < rhs._name;
    }

private:
    std::string _name;
    gpu_program::const_ptr _program;
    render_state::ptr _state;
    render_uniform::ptr _uniform;

    friend class material_mgr;
};

/// material managers, owning the material, and hold the only reference
/// TODO: resource_mgr? different resource mgr policy?
class material_mgr {
public:
    typedef std::vector<material::ptr> materials_t;

public:
    material* add_material(std::string const&,
                           gpu_program::ptr&& program,
                           render_state::ptr && state,
                           render_uniform::ptr && uniform);

    material* find_first_material(std::string const&) const;

    materials_t const& materials() const { return _materials; }

private:
    materials_t _materials;
};

#endif
