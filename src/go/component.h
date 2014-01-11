#ifndef _COMPONENT_H
#define _COMPONENT_H

#if 0
#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/size.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/int.hpp>
#endif

#include <vector>
#include <cassert>
#include "common/utility.h"

class game_object;

class component {
public:
    component(game_object* go) : _parent(go)
    {
    }

    game_object* parent() const { return _parent;}
    
private:
    bool _dirty;
    game_object* _parent;
};


#endif