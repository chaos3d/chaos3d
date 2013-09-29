/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_SINGLETON_H
#define _CHAOS_SINGLETON_H

#include "common.h"

_CHAOS_BEGIN

/** This is an implementation for the singleton pattern.
@remark
	be aware of linking.
	each singleton can exist in a dynamic space, where they share
	the same singleton. It could be a case. But, semantically here,
	we care about the existance in our own space.
	Therefore, each singleton should be initialized in some function,
	like startup, or wherever. Then in program space, it calls this
	initialization to create singletons.
*/

template<typename T>
class singleton
{
protected:
    static T*	__singleton;
    
public:
    
    singleton()
    {
        ASSERT( __singleton == NULL && "Singleton has been Created.");
        
        // Singleton will automatically save its location into __msSingleton
        // when it's first initialized.
#if 0
        long offset = (long)(T*)(1L)-(long)(singleton<T>*)(T*)(1L);
        __singleton = (T*)((long)this + offset);
#else
        __singleton = static_cast< T* >( this );
#endif
        
    }
    
    virtual ~singleton()
    {
        ASSERT(__singleton != NULL && "Singleton has not been Created!");
        __singleton = NULL;
    }
    
    inline static T* instance()
    {
        //ASSERT(__msSingleton != NULL && "Singleton has not been Created!");
        return __singleton;
    }
};

template<typename T>
    T* singleton<T>::__singleton = NULL;

_CHAOS_END

#endif
