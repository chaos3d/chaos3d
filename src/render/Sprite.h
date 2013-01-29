/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SPRITE_H
#define	_CHAOS_SPRITE_H

#include "chaos_config.h"
#include "core/core.h"
#include "core/math.h"
#include "anim/anim.h"
#include "script/scripting.h"
#include "NodeComponent.h"
#include "Scene2DNode.h"

struct lua_State;

_CHAOS_BEGIN

class Scene2DNode;
class Canvas;
class SpriteAnimation;

struct SpriteData{
	void*	vertexData;
	void*	indexData;
	short	numVertex;
	short	numIndex;
};

struct SpriteLayout{
	float	vtCoord[3];
	float	texCoord[2];
	float	color[4];
};


/*
* Definition ext:
*	sprite={
*		canvas, rect, [frame={}],
*		[type = "multi"|"ps"|"map"|"part"|"part rev"|"part circle"
*	}
*
* @remark
*	default frame will be the same bound of rect centered at origin
*	width and height of rect could be negative, in which case the sprite
*	is displayed reversedly. It would be helpful when using part sprite
*	or circle sprite
* i.e.
*	sprite = {canvas, {0,0,320,480}}
*/

DECLARE_CLASS(Sprite, NilParent);
class Sprite : public NodeComponent{
	DYNAMIC_CLASS(Sprite);
public:
	class SpriteApplier;
	
	struct SpriteValue{
		float x, y, width, height;
		Canvas	*canvas;

		SpriteValue()
			: x(0.f), y(0.f), width(0.f), height(0.f), canvas(0)
		{};

		SpriteValue(float x_, float y_, float width_, float height_,
			Canvas *canvas_)
			: x(x_), y(y_), width(width_), height(height_), canvas(canvas_)
		{};
	};

protected:
	enum{ MAX_CACHE = 32 };
	// global shared sprite vertex data
	static struct VertexData{
		SpriteLayout vertex[MAX_CACHE*4];
		short index[MAX_CACHE*6];
	} sData;

	Rectanglef		mBound;			// rect in the canvas/texture
	Rectanglef		mFrame;			// frame of the sprite, 
	Rectangle3f		mRegion;		// the rectangle on the screen
	Canvas*			mCanvas;

	virtual void parseElement(lua_State* L);

public:
	Sprite(Scene2DNode *node);
	Sprite(Scene2DNode *, TypeLua const&);

	Canvas* getCanvas() const { return mCanvas; };
	void setCanvas(Canvas* cav) { mCanvas = cav; };

	/**
	* update the rendering data.
	* @remark
	*	the vertices data are NOT ganraunteed to be available
	*	upon next calling of getData to any sprite
	* @param 
	*	in, the starting index
	*	out, rendering data
	*/
	virtual void updateData(short, SpriteData&) const;

	/**
	* update the region data, which may depend on the scene node
	*/
	void updateRegion(Rectanglef const&);
	void forceUpdate();	// force update the region, it should be called after setFrame

	void setBound(float x, float y, float width, float height);
	void setFrame(float x, float y, float width, float height);
	
	TypeLua getBoundLua() const;
	TypeLua getFrameLua() const;

	Rectanglef const& getFrame() const {return mFrame;};
	Rectanglef getBound() const;

	static Sprite* createSprite(Scene2DNode *, TypeLua const&);

	template<class Applier, class TE>
	AnimationState*	animation( SpriteAnimation* sa ){
		typedef ConcreteAnimationState<Applier, SpriteValue, TE> CAS;
		CAS* as = new CAS(sa, Applier(this));
		as->autorelease();
		return as;
	}
};

TYPE_RET_DYN(Sprite);

class Sprite::SpriteApplier{
private:
	Sprite	*mSprite;
public:

	inline Sprite*	getSprite() const { return mSprite; };
	inline Sprite*&	getSprite() {return mSprite; };
	
	SpriteApplier(SpriteApplier const& rhs) : mSprite(rhs.mSprite) {
		if( mSprite != 0 )
			mSprite->getNode()->retain();
	};
	
	SpriteApplier& operator=(SpriteApplier const& rhs){
		if( mSprite == rhs.mSprite )
			return *this;
		
		if( mSprite != 0 )
			mSprite->getNode()->release();
		
		mSprite = rhs.mSprite;
		if( mSprite != 0 )
			mSprite->getNode()->retain();
		return *this;
	};
	
	~SpriteApplier() {
		if( mSprite != 0 ){
#if defined(DEBUG)
			LOG("SpriteAnimation deallocated (%s) %p", mSprite->getNode()->getTag().c_str(), this);
#endif
			mSprite->getNode()->release();
		}
	};
	
	SpriteApplier(Sprite* t) : mSprite(t) {
		if( t != 0 )
			t->getNode()->retain();		
	};
	
	inline bool operator() (SpriteValue const& val){
		if( val.canvas != 0 )
			mSprite->setCanvas( val.canvas );
		mSprite->setBound(val.x, val.y, val.width, val.height );
		return mSprite->getNode()->getParent() != 0;
	}
};


DECLARE_CLASS(SpriteAnimation, NilParent)
class SpriteAnimation : public ConcreteAnimation<Sprite::SpriteValue>{
public:
	SpriteAnimation(TypeLua const& lua);

};


_CHAOS_END

#endif

