/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCRIPTSTATE_H
#define	_CHAOS_SCRIPTSTATE_H

#include "common.h"
#include "lua.h"

_CHAOS_BEGIN

/**
 * a wrapper for lua state
 * and simplify to manipulate the state
 */
class CHAOS_API ScriptState{
public:
	/**
	 * Decouple IO for reading/writing from/to the source
	 */
	class SourceReader{
	public:
		virtual char const* source() { return NULL; };
        virtual size_t read(char* buf, size_t buf_size) = 0;
	};
    
    class SourceWriter{
    public:
        virtual size_t write(const char* buf, size_t buf_size) = 0;
    };
    
    class StringReader : public SourceReader{
    public:
        StringReader(char const* buf);
        virtual size_t read(char* buf, size_t buf_size);
        
    private:
        char const* _strbuf;
        size_t _bufsize;
        long _curpos;
    };
    
	// to be deleted
	// Unwind the pointer and the reference
	// so we only need to implement the push pointer value for general class type
	// because c++ doesn't support partial specification of function template
	// we need to use class functor
	template<class T> struct GetUnwind;
	template<class T> struct PushUnwind;

public:
	ScriptState();
	ScriptState(lua_State* L); // this one might not be public
	ScriptState(ScriptState const&ss);
	ScriptState& operator =(ScriptState const& rhs);
	~ScriptState();

	bool operator==(ScriptState const& rhs) const;
	bool operator!=(ScriptState const& rhs) const;

	inline lua_State* getState() const{
		return _L;
	}

	operator lua_State* () const{
		return _L;
	}

	/**
	 * copy the n objects from another lua state 
	 * and push it to the top of the stack
	 * it could be from a different state (which
	 * is its purpose) or from the same state
	 * (coroutine to coroutine where it will 'copy'
	 * all objects too) 
	 */
	bool copy(ScriptState const& from, int n);

	/**
	 * push a T into the stack
	 *
	 * it is supposed to be less invasive so that it
	 * won't make any assumption how to present an 
	 * object inside lua (how to use UserData or not
	 * at all). To 'extend' the class by specialize
	 * those functions.
	 *
	 * the template function depends on how the type
	 * will be imported into the script, not all the
	 * template functions need to be implemented,
	 * although it can be all. the semantic will come
	 * from the end-users.
	 */
    
    // use for binding
	template<class T> static void push_(lua_State*, T const& val);
	template<class T> static void push_(lua_State*, T val);
	template<class T> static void push_(lua_State*, T* val);
	template<class T> static void push_(lua_State*);
    
    // transforming binding
	template<class T> static void push_(lua_State* L, T const* val)
    { push_<T>(L, const_cast<T*>(val)); }
    
	template<class T> static T get_(lua_State*, int index);

	template<class T> void push_(T const& val)	// push a value
    { push_<T>(_L, val); }
	template<class T> void push_(T val) 		// push a value
    { push_<T>(_L, val); }
	template<class T> void push_(T* val)		// push a pointer
    { push_<T>(_L, val); }
	template<class T> void push_()				// push a new T into the stack
    { push_<T>(_L); }

    // transforming binding
	template<class T> void push_(T const* val)
    { push_<T>(_L, const_cast<T*>(val)); }

	template<class T> T	get_(int index) const{
        return get_<T>(_L, index);
    }

    // useful wrapper functions
    void pushMetatable(void*);  // simpler meta id
    void pushUD(void* obj, void* meta);
    void* getUD(int n) const;
    
    void pop(int n);
    int top() const;
    
    // address to global table as the signature to
    // distinguish the lua state
	const void* signature() const;
    
private:
	int increseRef(int delta);

	lua_State* _L;
};

// load a script and push the function to the stack
template<> void ScriptState::push_<ScriptState::SourceReader>(lua_State *, ScriptState::SourceReader *);

// todo: move this to .inl
template<> void ScriptState::push_<bool>(lua_State *, bool);
template<> void ScriptState::push_<int>(int);
template<> void ScriptState::push_<long>(long);
template<> void ScriptState::push_<float>(float);
template<> void ScriptState::push_<double>(double);
template<> void ScriptState::push_<char const*>(char const*);

template<> bool ScriptState::get_<bool>(lua_State *, int);

//#include "ScriptState.inl"

/*
template<class T>
inline void	ScriptState::push_(T val) const{
	typedef typename TypeDecider<typename TypeUnwind<T>::Type>::TYPE RetType;
	typedef typename TypeTraits<typename TypeUnwind<T>::Type>::Type WrapType;
	PushUnwind<T>::push( *this, val, TYPE_FROM_NAME( WrapType ));
};

template<class T>
inline T	ScriptState::get_(int index) const{
	return GetUnwind<T>::get( *this, index );
};
*/

_CHAOS_END

#endif
