#include "go/component_manager.h"
#include "go/game_object.h"
#include <vector>
#include <memory>

#pragma mark - the global manager

class managers_internal : public component_manager::managers_t {
public:
    typedef std::unique_ptr<component_manager> mgr_t;
    typedef std::vector<mgr_t> mgrs_t;
    
public:
    // add to the list and retain the ownership
    void add(component_manager* mgr) {
        // already existed or the same type already existed?
        assert(std::none_of(_mgrs.begin(), _mgrs.end(), [=](mgr_t const& elmt) {
            return elmt.get() == mgr || typeid(*elmt.get()) == typeid(*mgr);
        }));
        _mgrs.emplace_back(mgr);
    }
    
    // remove mgr from the list and release the ownership.
    void remove(component_manager* mgr) {
        _mgrs.erase(std::find_if(_mgrs.begin(), _mgrs.end(), [=](mgr_t& elmt) {
            if(elmt.get() == mgr) {
                elmt.release();
                return true;
            }else
                return false;
        }));
    }
             
private:
    void update(game_object* go) override {
        if(!go)
            return;
        
        // TODO: cached the results
        // TODO2: wrap with a collection result with helper functions
        std::vector<game_object*> gos;
        gos.reserve(game_object::number_of_objects());
        
        go->pre_order(_active_mark++, [&](game_object const& go) {
            // const cast here is fine since we would use it
            // after processing
            gos.emplace_back(const_cast<game_object*>(&go));
        });
        
        if(gos.empty())
            return;
        
        // the root won't have a parent
        for(auto it = ++gos.begin(); it != gos.end(); ++it) {
            (*it)->populate_flag();
        }
        
        for(auto& it : _mgrs) {
            it->update(gos);
        }

        for(auto& it : gos) {
            it->reset_flag();
        }
    };
    
    mgrs_t _mgrs;
    uint32_t _active_mark = 0;
};

component_manager::managers_t& component_manager::managers() {
    static managers_internal internal;
    return internal;
}

#define global static_cast<managers_internal&>(managers())

#pragma mark - the component manager

uint32_t component_manager::_fixed_component = 0;

component_manager::component_manager(bool managed) {
    if(managed)
        global.add(this);
}

component_manager::~component_manager() {
    global.remove(this);
}