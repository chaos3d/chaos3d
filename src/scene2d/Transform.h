/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_TRANSFORM_H
#define	_CHAOS_TRANSFORM_H

#include "common/common.h"
#include "wm4-algebra/algebra.h"

_CHAOS_BEGIN

class Scene2DNode;

/*
* Definition ext:
*	transform = {translate={x,y,z}, scale={x,y,z}, rotate={x,y,z}} | false
*
* @remark
*	if not specified, identical transform applied
*	if false, then no transforms
*
*/

class Transform{
public:
	class TranslateApplier;
	class RotateApplier;
	class ScaleApplier;

protected:
	// World
    Wm4::Matrix4f		mGlobal;	// todo: reduce to matrix4x3

	// Local
	Wm4::Quaternionf	mRotate;
	Wm4::Vector3f		mTranslate;
	Wm4::Vector3f		mScale;

	Scene2DNode* const mNode;       // this is a weak reference

public:
	Transform(Scene2DNode *n)
		: mRotate(1.f,0.f,0.f,0.f),mScale(1.f,1.f,1.f),mTranslate(0.f,0.f,0.f),
		mNode(n)
	{};

    Transform(Transform const&);
    
	//Transform(Scene2DNode *, TypeLua const&);

	void updateTransform();
	void forceUpdate();	// force to calculate transform now

	/**
	* get the global transform matrix
	*/
	Wm4::Matrix4f const& getMatrix() const { return mGlobal; };

	void	setRotate( Wm4::Quaternionf const& rot );
	void	setTranslate( Wm4::Vector3f const& trans );
	void	setScale( Wm4::Vector3f const& scale );

	void	setRotate( float x, float y, float z, float angle);
	void	setRotate( float x, float y, float z);
	void	setTranslate( float x, float y, float z);
	void	setScale( float x, float y, float z);

	Wm4::Quaternionf const&	getRotate() const{ return mRotate; };
	Wm4::Vector3f const&		getTranslate() const{ return mTranslate; };
	Wm4::Vector3f const&		getScale() const{ return mScale; }

	// keep the same world position
	// but under a different parent node
	void relocate(Transform* );
	
#if 0
	// Animation
	AnimationState*		translate(Wm4::Vector3f const& pos){
		return translate( pos[0], pos[1], pos[2]);
	};
	AnimationState*		translate(float x, float y, float z);
	AnimationState*		rotate(float x, float y, float z);

	// helper function for script
	template<class Applier, class TE, class Numeric>
	AnimationState*	animation( NumericAnimation<Numeric>* na ){
		typedef NumericAnimationState<Applier, Numeric, TE> NAS;
		NAS* as = new NAS(na, Applier(this));
		as->autorelease();
		return as;
	}
#endif

	Scene2DNode* getNode() const { return mNode; };
};

_CHAOS_END

#endif

