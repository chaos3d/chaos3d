#ifndef _CHAOS3D_ACTION_ACTION_SCRIPT_H
#define _CHAOS3D_ACTION_ACTION_SCRIPT_H

#include <memory>
#include "action/action.h"
#include "script/state.h"
#include "common/referenced_count.h"

class yieldable : virtual public referenced_count{
public:
    typedef std::unique_ptr<yieldable> ptr;
    
public:
    virtual ~yieldable() {};
    virtual bool is_done() const = 0;
    virtual void update(double delta) = 0;
};

class action_script : public action {
public:
    action_script(script::coroutine&& co)
    : _coroutine(std::move(co)) {
    }

    static action* from(script::coroutine&& co) {
        return new action_script(std::move(co));
    }
    
    virtual bool done() const override;
    
protected:
    virtual void on_start() override;
    virtual void update();
    
    // TODO
    //virtual void on_skip(bool skip) override;
    
private:
    script::coroutine _coroutine;
    yieldable::ptr _yielded;
};

#endif