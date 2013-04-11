/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NODECOMPONENT_H
#define	_CHAOS_NODECOMPONENT_H

#include "common/common.h"

_CHAOS_BEGIN

namespace scene2d {
    // component traits
    template <class C>
    struct has_clear_flag {
        struct result_ {
            enum { value = false; }
        };
    };
    
    template <class C>
    struct needs_update {
        struct result_ {
            enum { value = false; }
        };        
    };
}

_CHAOS_END

#endif

