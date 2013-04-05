/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NODEAPPLIER_H
#define	_CHAOS_NODEAPPLIER_H

#include "chaos_config.h"
#include "Scene2DNode.h"
#include "NodeColor.h"
#include "Transform.h"

_CHAOS_BEGIN

///--------------------------------------------------------
///--------------------------------------------------------
class NodeColor::ColorApplier{
private:
	NodeColor	*mColor;
public:
	inline NodeColor*	getColor() const { return mColor; };
	inline NodeColor*&	getColor() {return mColor; };

	ColorApplier(NodeColor* t) : mColor(t) {
		if( t != 0 )
			t->getNode()->retain();
	};
	
	//ColorApplier(NodeColor* t) : mColor(t) {
	//	mColor->getNode()->retain();
	//};
	ColorApplier(ColorApplier const& rhs) : mColor(rhs.mColor) {
		if( mColor != 0 )
			mColor->getNode()->retain();
	};

	ColorApplier& operator=(ColorApplier const& rhs){
		if( mColor == rhs.mColor )
			return *this;
		
		if( mColor != 0 )
			mColor->getNode()->release();

		mColor = rhs.mColor;
		if( mColor != 0 )
			mColor->getNode()->retain();
		return *this;
	};

	~ColorApplier() {
		if( mColor != 0 ) {
#if defined(DEBUG)
			LOG("ColorAnimation deallocated (%s) %p", mColor->getNode()->getTag().c_str(), this);
#endif
			mColor->getNode()->release();
		}
	};

	inline bool operator() (ColorRGBA const& color){
		mColor->setColor( color );
		return mColor->getNode()->getParent() != 0;
	}
};


///--------------------------------------------------------
///--------------------------------------------------------
class Transform::TranslateApplier{
private:
	Transform	*mTransform;
public:

	inline Transform*	getTransform() const { return mTransform; };
	inline Transform*&	getTransform() {return mTransform; };

	inline bool operator() (Vector3f const& pos){
		mTransform->setTranslate( pos );
		return mTransform->getNode()->getParent() != 0;
	}

	//TranslateApplier(Transform* t) : mTransform(t) {};
	TranslateApplier(Transform* t) : mTransform(t) {
		if( t != 0 )
			t->getNode()->retain();
	};

	TranslateApplier(TranslateApplier const& rhs) : mTransform(rhs.mTransform) {
		if( mTransform != 0 )
			mTransform->getNode()->retain();
	};

	TranslateApplier& operator=(TranslateApplier const& rhs){
		if( mTransform == rhs.mTransform )
			return *this;
		
		if( mTransform != 0 )
			mTransform->getNode()->release();

		mTransform = rhs.mTransform;
		if( mTransform != 0 )
			mTransform->getNode()->retain();
		return *this;
	};

	~TranslateApplier() {
		if( mTransform != 0 ){
#if defined(DEBUG)
			LOG("TranslateAnimation deallocated (%s) %p", mTransform->getNode()->getTag().c_str(), this);
#endif
			mTransform->getNode()->release();
		}
	};

};


class Transform::RotateApplier{
private:
	Transform	*mTransform;
public:
	inline Transform*	getTransform() const { return mTransform; };
	inline Transform*&	getTransform() {return mTransform; };

	//RotateApplier(Transform* t) : mTransform(t) {};
	RotateApplier(Transform* t) : mTransform(t) {
		if( t != 0 )
			t->getNode()->retain();
	};

	RotateApplier(RotateApplier const& rhs) : mTransform(rhs.mTransform) {
		if( mTransform != 0 )
			mTransform->getNode()->retain();
	};

	RotateApplier& operator=(RotateApplier const& rhs){
		if( mTransform == rhs.mTransform )
			return *this;
		
		if( mTransform != 0 )
			mTransform->getNode()->release();

		mTransform = rhs.mTransform;
		if( mTransform != 0 )
			mTransform->getNode()->retain();
		return *this;
	};

	~RotateApplier() {
		if( mTransform != 0 ){
#if defined(DEBUG)
			LOG("RotateAnimation deallocated (%s) %p", mTransform->getNode()->getTag().c_str(), this);
#endif
			mTransform->getNode()->release();
		}
	};
	inline bool operator() (Quaternionf const& rot){
		mTransform->setRotate( rot );
		return mTransform->getNode()->getParent() != 0;
	}
};

class Transform::ScaleApplier{
private:
	Transform	*mTransform;
public:
	inline Transform*	getTransform() const { return mTransform; };
	inline Transform*&	getTransform() {return mTransform; };

	//ScaleApplier(Transform* t) : mTransform(t) {};
	ScaleApplier(Transform* t) : mTransform(t) {
		if( t != 0 )
			t->getNode()->retain();
	};

	ScaleApplier(ScaleApplier const& rhs) : mTransform(rhs.mTransform) {
		if( mTransform != 0 )
			mTransform->getNode()->retain();
	};

	ScaleApplier& operator=(ScaleApplier const& rhs){
		if( mTransform == rhs.mTransform )
			return *this;
		
		if( mTransform != 0 )
			mTransform->getNode()->release();

		mTransform = rhs.mTransform;
		if( mTransform != 0 )
			mTransform->getNode()->retain();
		return *this;
	};

	~ScaleApplier() {
		if( mTransform != 0 ){
#if defined(DEBUG)
			LOG("ScaleAnimation deallocated (%s) %p", mTransform->getNode()->getTag().c_str(), this);
#endif
			mTransform->getNode()->release();
		}
	};
	
	inline bool operator() (Vector3f const& scale){
		mTransform->setScale( scale );
		return mTransform->getNode()->getParent() != 0;
	}
};


_CHAOS_END

#endif

