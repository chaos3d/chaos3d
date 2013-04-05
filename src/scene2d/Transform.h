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

#include "chaos_config.h"
#include "core/math.h"
#include "script/LuaType.h"

_CHAOS_BEGIN

class Scene2DNode;
class AnimationState;
template<class, class, class> class NumericAnimationState;
template<class> class NumericAnimation;

/*
* Definition ext:
*	transform = {translate={x,y,z}, scale={x,y,z}, rotate={x,y,z}} | false
*
* @remark
*	if not specified, identical transform applied
*	if false, then no transforms
*
*/

DECLARE_CLASS(Transform, NilParent);

class Transform{
public:
	class TranslateApplier;
	class RotateApplier;
	class ScaleApplier;

protected:
	// World
	Matrix4f		mGlobal;	// todo: reduce to matrix4x3

	// Local
	Quaternionf		mRotate;
	Vector3f		mTranslate;
	Vector3f		mScale;

	Scene2DNode* const mNode;

public:
	Transform(Scene2DNode *n)
		: mRotate(1.f,0.f,0.f,0.f),mScale(1.f,1.f,1.f),mTranslate(0.f,0.f,0.f),
		mNode(n)
	{};

	Transform(Scene2DNode *, TypeLua const&);

	void updateTransform();
	void forceUpdate();	// force to calculate transform now

	/**
	* get the global transform matrix
	*/
	Matrix4f const& getMatrix() const { return mGlobal; };

	void	setRotate( Quaternionf const& rot );
	void	setTranslate( Vector3f const& trans );
	void	setScale( Vector3f const& scale );

	void	setRotate( float x, float y, float z, float angle);
	void	setRotate( float x, float y, float z);
	void	setTranslate( float x, float y, float z);
	void	setScale( float x, float y, float z);

	Quaternionf const&	getRotate() const{ return mRotate; };
	Vector3f const&		getTranslate() const{ return mTranslate; };
	Vector3f const&		getScale() const{ return mScale; }

	// keep the same world position
	// but under a different parent node
	void relocate(Transform* );
	
	// Animation
	AnimationState*		translate(Vector3f const& pos){
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

	Scene2DNode* getNode() const { return mNode; };
};

_CHAOS_END

#endif

