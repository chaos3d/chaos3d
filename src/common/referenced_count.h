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

#define SAFE_RELEASE0(obj)  if( (obj) != nullptr ) { (obj)->release(); (obj) = 0; }
#define SAFE_RELEASE(obj)   if( (obj) != nullptr ) { (obj)->release(); }
#define SAFE_RETAIN(obj)    if( (obj) != nullptr ) { (obj)->retain(); }

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
    
    class weak_ref_ctrl_base {
    public:
        struct weak_releaser {
            void operator()(weak_ref_ctrl_base const* weak_ptr) const {
                weak_ptr->decrease();
            }
        };
        
        weak_ref_ctrl_base(referenced_count* ref) : _weak_count(1), _strong_ref(ref)
        {}
        
        bool expired() const {
            return _strong_ref == nullptr;
        }
        
        referenced_count* raw_pointer() const {
            return _strong_ref;
        }
        
    protected:
        int weak_count() const { return _weak_count - 1; }
        
        void on_zero_release() {
            _strong_ref = nullptr;
            decrease();
        }
        
        void increase() const {
            ++ _weak_count;
        }
        
        void decrease() const {
            -- _weak_count;
            if(_weak_count == 0)
                delete this;
        };

        // TODO: thread safe
        mutable int _weak_count;
        referenced_count* _strong_ref;
        friend class referenced_count;
    };
    
    template<class R>
    class weak_ref_ctrl : public weak_ref_ctrl_base{
    public:
        typedef typename std::remove_const<R>::type const const_r_type;
        typedef typename std::remove_const<R>::type r_type;
        typedef std::unique_ptr<const_r_type, referenced_count::release_deleter> const_ptr;
        typedef std::unique_ptr<r_type, referenced_count::release_deleter> ptr;
        
        typedef std::unique_ptr<weak_ref_ctrl, weak_ref_ctrl_base::weak_releaser> weak_ptr;
        typedef std::unique_ptr<weak_ref_ctrl const, weak_ref_ctrl_base::weak_releaser> const_weak_ptr;
        
    public:
        weak_ref_ctrl(referenced_count* ref) : weak_ref_ctrl_base(ref)
        {}
        
        const_ptr lock() const {
            return _strong_ref ? _strong_ref->retain<R const>() : const_ptr();
        };
        
        ptr lock() {
            return _strong_ref ? _strong_ref->retain<R>() : ptr();
        };
        
        weak_ptr get() {
            increase();
            return weak_ptr(this);
        };
        
        const_weak_ptr get() const {
            increase();
            return const_weak_ptr(this);
        };
        
    };
    
public:
	referenced_count() : _ref_count(1), _weak_ctrl(nullptr){};
	virtual ~referenced_count() {
        if(_weak_ctrl) {
            _weak_ctrl->on_zero_release();
        }
    };

    // no need to move
    referenced_count(referenced_count&& rhs) = delete;
    referenced_count& operator=(referenced_count&& rhs) = delete;
    
    template<class T,
    typename std::enable_if<std::is_base_of<referenced_count, T>::value>::type* = nullptr>
    std::unique_ptr<T const, referenced_count::release_deleter> retain() const {
	   	++ _ref_count;
        return std::unique_ptr<T const, referenced_count::release_deleter>(static_cast<T const*>(this));
	};

    template<class T,
    typename std::enable_if<std::is_base_of<referenced_count, T>::value>::type* = nullptr>
    std::unique_ptr<T, referenced_count::release_deleter> retain() {
	   	++ _ref_count;
        return std::unique_ptr<T, referenced_count::release_deleter>(static_cast<T*>(this));
	};
    
    template<class T,
    typename std::enable_if<std::is_base_of<referenced_count, T>::value>::type* = nullptr>
    typename weak_ref_ctrl<T>::const_weak_ptr get() const{
	   	if(_weak_ctrl == nullptr)
            _weak_ctrl = new weak_ref_ctrl<T const>(const_cast<T*>(this));
        _weak_ctrl->increase();
        return typename weak_ref_ctrl<T>::const_weak_ptr(static_cast<weak_ref_ctrl<T>*>(const_cast<T*>(_weak_ctrl)));
	};
    
    template<class T,
    typename std::enable_if<std::is_base_of<referenced_count, T>::value>::type* = nullptr>
    typename weak_ref_ctrl<T>::weak_ptr get() {
	   	if(_weak_ctrl == nullptr)
            _weak_ctrl = new weak_ref_ctrl<T>(this);
        _weak_ctrl->increase();
        return typename weak_ref_ctrl<T>::weak_ptr(static_cast<weak_ref_ctrl<T>*>(_weak_ctrl));
	};
    
    void retain() const {
	   	++ _ref_count;
    }

	void release() const {
		assert( _ref_count > 0 );
		
		if( -- _ref_count == 0 )
			delete this;
	}

	int ref_count() const { return _ref_count; };
    bool unique() const {
        return _ref_count == 1 && (!_weak_ctrl || _weak_ctrl->weak_count() == 0);
    }
    
private:
	mutable int _ref_count;
    mutable weak_ref_ctrl_base* _weak_ctrl;
};

#if 0
// not useful...
template<class R = std::nullptr_t>
class referenced_count_base : public referenced_count{
public:
    typedef std::unique_ptr<R, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<R const, referenced_count::release_deleter> const_ptr;
    
public:
    ptr retain() {
        referenced_count::retain();
        return ptr(static_cast<R*>(this));
	};
    
    const_ptr retain() const {
        referenced_count::retain();
        return const_ptr(static_cast<R*>(this));
	};
};
#endif

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

