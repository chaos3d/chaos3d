/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_REFERENCEDCOUNT_H
#define	_CHAOS_REFERENCEDCOUNT_H

#include <cassert>
#include <memory>
#include <type_traits>

#define SAFE_DELETE0(obj)   if( (obj) != 0 ) { delete (obj); (obj) = 0; }
#define SAFE_DELETE(obj)    if( (obj) != 0 ) { delete (obj); }

#define SAFE_RELEASE0(obj)  if( (obj) != 0 ) { (obj)->release(); (obj) = 0; }
#define SAFE_RELEASE(obj)   if( (obj) != 0 ) { (obj)->release(); }
#define SAFE_RETAIN(obj)    if( (obj) != 0 ) { (obj)->retain(); }
#define SAFE_ASSIGN(obj, o) do{ if( obj == o) break; if( (obj) != 0 ) { delete (obj); } (obj) = (o); } }while(0)
#define SAFE_REFAGN(obj, o) do{ if( obj == o) break; if( (obj) != 0 ) { (obj)->release(); } if(((obj) = (o)) != 0) { (obj)->retain(); } }while(0)
#define SAFE_AUTOREFAGN(obj, o) do{ if( obj == o) break; if( (obj) != 0 ) { (obj)->autorelease(); } if(((obj) = (o)) != 0) { (obj)->retain(); } }while(0)

#define RETAIN(obj)         ((obj)->retain(), obj)
#define AUTORELEASE(obj)    ((obj)->autorelease(), obj)

/*
A reference counting object that will delete itself when counting down zero.

the schema is similar to obj-c. when objects are explicitly created using new
operator, you should then "release" it afterwards.
because there is an autorelease pool, objects, if created by a function, are
not needed to release if not retained. The creaction functor will "autorelease" it

NB, you should not explicitly delete the referenced object even if you create
them using new operator.

*/

//template<class R = std::nullptr_t>
class referenced_count{
public:
    struct release_deleter {
        void operator()(referenced_count const* obj) const {
            obj->release();
        }
    };
    
//    typedef std::unique_ptr<R, referenced_count::release_deleter> ptr;
//    typedef std::unique_ptr<R const, referenced_count::release_deleter> const_ptr;
    
public:
	referenced_count() : _ref_count( 1 ) {};
	virtual ~referenced_count() {};

    // no need to move
    referenced_count(referenced_count&& rhs) = delete;
    referenced_count& operator=(referenced_count&& rhs) = delete;
    
    template<class T,
    typename std::enable_if<std::is_base_of<referenced_count, T>::value>::type* = nullptr>
    std::unique_ptr<T const, referenced_count::release_deleter> retain() const {
	   	++ _ref_count;
        return std::unique_ptr<T const, referenced_count::release_deleter>(static_cast<T*>(this));
	};

    template<class T,
    typename std::enable_if<std::is_base_of<referenced_count, T>::value>::type* = nullptr>
    std::unique_ptr<T, referenced_count::release_deleter> retain() {
	   	++ _ref_count;
        return std::unique_ptr<T, referenced_count::release_deleter>(static_cast<T*>(this));
	};
    
#if 0
    template<typename std::enable_if<std::is_base_of<referenced_count, R>::value>::type* = nullptr>
    ptr retain() {
	   	++ _ref_count;
        return ptr(static_cast<R*>(this));
	};
    
    template<typename std::enable_if<std::is_base_of<referenced_count, R>::value>::type* = nullptr>
    const_ptr retain() const {
	   	++ _ref_count;
        return const_ptr(static_cast<R*>(this));
	};
#endif
    
    void retain() const {
	   	++ _ref_count;
    }

	void release() const {
		assert( _ref_count > 0 );
		
		if( -- _ref_count == 0 )
			delete this;
	}

	int ref_count() const { return _ref_count; };
    bool unique() const { return _ref_count == 0; }
    
private:
	mutable int _ref_count;
};

template<class T, class... Args>
std::unique_ptr<T, referenced_count::release_deleter> make_unique_ref(Args&&... args) {
    static_assert(std::is_base_of<referenced_count, T>::value, "T is not derived from referenced_count");
    return std::unique_ptr<T, referenced_count::release_deleter>(new T(std::forward<Args>(args)...));
}

template<class T, class... Args>
std::shared_ptr<T> make_shared_ref(Args&&... args) {
    static_assert(std::is_base_of<referenced_count, T>::value, "T is not derived from referenced_count");
    return std::shared_ptr<T>(new T(std::forward<Args>(args)...), referenced_count::release_deleter());
}

#endif

