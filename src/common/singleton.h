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

#include <type_traits>
#include <cassert>
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


struct Static_Instance {}; // the class holds a static variable
struct Client_Instance {}; // the client manually creates the instance

template<typename T, typename Type = Client_Instance>
class singleton
{
public:
    singleton()
    {
        assert( __singleton == NULL && "Singleton has been Created.");
        
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
        assert(__singleton != NULL && "Singleton has not been Created!");
        __singleton = NULL;
    }
    
    // FIXME: a elegant way?
    template<typename _Type = Type>
    inline static typename std::enable_if<std::is_same<_Type,Client_Instance>::value, T&>::type instance(Client_Instance = Client_Instance())
    {
        return *__singleton;
    }
    
    template<typename _Type = Type>
    inline static typename std::enable_if<std::is_same<_Type,Static_Instance>::value, T&>::type instance(Static_Instance = Static_Instance())
    {
        static T _instance;
        return *__singleton;
    }
    
private:
    static T* __singleton;
};

template<typename T, typename Type>
    T* singleton<T, Type>::__singleton = NULL;

_CHAOS_END

#endif
