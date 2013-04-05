/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_ELEMENTLIST_H
#define	_CHAOS_ELEMENTLIST_H

#include "common.h"

_CHAOS_BEGIN

class CHAOS_API Element{
private:
	Element*		mNext;
	Element*		mPrev;

public:
	Element():mNext(0), mPrev(0)
	{};

	Element* next() const{
		return mNext;
	};
	
	Element* pre() const{
		return mPrev;
	};
	
	Element*& next() {
		return mNext;
	};
	
	Element*& pre() {
		return mPrev;
	};
};

template<typename Element>
class ElementRetainer{
public:
	static inline void release(Element* ele){
		ele->release();
	}
	
	static inline void retain(Element* ele){
		ele->retain();
	}
};

template<typename Element>
class ElementDeleter{
public:
	static inline void release(Element* ele){
		delete ele;
	}
	
	static inline void retain(Element* ele){
	}
};

template<typename Element, class LifeGuard = ElementRetainer<Element> >
class CHAOS_API ElementList {
public:
	class iterator {
	private:
		Element	*current;

	public:
		explicit iterator(Element* ele = 0):current(ele)
		{};

		iterator(iterator const& rhs) : current(rhs.current)
		{};

		operator bool() {
			return current != 0;
		};

		iterator& operator=(iterator const& rhs){
			current = rhs.current;
			return *this;
		};

		iterator& operator--(){
			current = (Element*)current->pre();
			return *this;
		}

		iterator& operator++(){
			current = (Element*)current->next();
			return *this;
		}
		
		iterator operator--(int){
			iterator it(current);
			current = (Element*)current->pre();
			return it;
		}
		
		iterator operator++(int){
			iterator it(current);
			current = (Element*)current->next();
			return it;
		}
		
		Element& operator*(){
			return *current;
		}

		Element* operator->(){
			return &operator*();
		}
		
		Element* thiz(){
			return &operator*();
		};
	};
	
	class Lessor{
	public:
		inline bool operator()(Element const& lhs, Element const& rhs) const{
			return lhs < rhs;
		}
	};

protected:
	Element *mFirstElement;

	// not supported copy/assign since elements are not copyable
	ElementList(ElementList const&);
	ElementList& operator=(ElementList const&);
public:
	ElementList(): mFirstElement(0)
	{};

	~ElementList() {
		while(!empty())
			remove(first());
	};

	inline Element* first() const { return mFirstElement; };
	inline Element*& first() { return mFirstElement; };
	
	inline bool exists(Element* ele) const{
		// a dangling element and not a first element
		return first() == ele || ele->next() != 0 || ele->pre() != 0;
	}

	inline bool empty() const{
		return mFirstElement == 0;
	}
	
	template<class less>
	void insert(Element *ele, less const& lessor = Lessor()){
		if( ele == 0 )
			return;
		
		iterator it(first()), last(first);
		for(; it && lessor(*it, *ele); last = it++)
			;
		
		append(ele, it ? &(*it) : &(*last));
	};
	
	void append(Element *ele, Element* after = 0){
		if( ele == 0 )
			return;
		
		if( after == 0 ){
			ele->next() = first();
			ele->pre() = 0;
			if ( first() != 0 ) 
				first()->pre() = ele;
			first() = ele;
		} else {
			ele->next() = after->next();
			ele->pre() = after;
			
			if ( after->next() != 0 ) 
				after->next()->pre() = ele;
			after->next() = ele;
		}
		
		LifeGuard::retain(ele);
	}
	
	void remove(Element *ele){
		if ( ele == 0 )
			return;
		
		if ( ele->next() != 0 ) 
			ele->next()->pre() = ele->pre();
		
		if ( ele->pre() != 0 )
			ele->pre()->next() = ele->next();
		
		if ( first() == ele )
			first() = (Element*)ele->next();
		
		ele->next() = ele->pre() = 0;
		LifeGuard::release(ele);
	}	

	inline iterator begin() const{
		return iterator(mFirstElement);
	}
	
	inline iterator last() const{
		iterator it(mFirstElement);
		if( mFirstElement != 0 ) while( it->next() != 0 )
			++it;
		return it;
	}
};

_CHAOS_END

#endif

