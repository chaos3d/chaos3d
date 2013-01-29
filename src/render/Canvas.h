/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_CANVAS_H
#define	_CHAOS_CANVAS_H

#include "chaos_config.h"
#include "res/Resource.h"
#include "script/LuaType.h"
#include "Texture.h"

_CHAOS_BEGIN

enum BlendFactor
{
	BF_ONE,
	BF_ZERO,
	BF_DST_COLOR,
	BF_SRC_COLOUR,
	BF_ONE_MINUS_DST_COLOR,
	BF_ONE_MINUS_SRC_COLOR,
	BF_DST_ALPHA,
	BF_SRC_ALPHA,
	BF_ONE_MINUS_DST_ALPHA,
	BF_ONE_MINUS_SRC_ALPHA,
	BF_DISABLED
};

enum BlendEquation
{
	BE_ADD,
	BE_SUBTRACT,
	BE_REV_SUBTRACT
};

enum CompareFunction
{
	CF_ALWAYS_FAIL,
	CF_ALWAYS_PASS,
	CF_LESS,
	CF_LESS_EQUAL,
	CF_EQUAL,
	CF_NOT_EQUAL,
	CF_GREATER_EQUAL,
	CF_GREATER,
	CF_DISABLED
};

enum EnvMode{
	EM_MODULATE,
	EM_ADD,
	EM_DECAL,
	EM_BLEND,
	EM_COMBINE,
	EM_REPLACE,
};

enum CombinerFunc{
	CF_REPLACE,
	CF_MODULATE,
	CF_ADD,
	CF_ADD_SIGNED,
	CF_INTERPOLATE,
	CF_SUBTRACT,
};

enum CombinerSrc{
	CS_PREVIOUS,
	CS_CONSTANT,
	CS_PRIMARY,
	CS_TEXTURE,
	CS_TEXTURE0,
	CS_TEXTURE1,
};

enum CombinerOperand{
	CO_SRC_COLOR,
	CO_ONE_MINUS_SRC_COLOR,
	CO_SRC_ALPHA,
	CO_ONE_MINUS_SRC_ALPHA
};

struct CanvasBlend{
	BlendFactor		blendSrc;
	BlendFactor		blendDest;
	BlendEquation	blendEq;

	CanvasBlend()
		: blendSrc( BF_ONE ), blendDest( BF_ONE_MINUS_SRC_ALPHA ), blendEq(BE_ADD)
	{};
};

struct CanvasDepthTest{
	CompareFunction	compareFunc;

	CanvasDepthTest() : compareFunc( CF_DISABLED ) {};
};


struct TextureAttribute{
	Texture*	texture;		// disabled if null
	FilterMode	minFilter;
	FilterMode	magFilter;
	WrapMode	wrapS;
	WrapMode	wrapT;
	
	EnvMode		envMode;
	
	CombinerFunc	combineRGB;
	CombinerFunc	combineAlpha;
	CombinerSrc		srcRGB[3];
	CombinerSrc		srcAlpha[3];
	CombinerOperand	opRGB[3];
	CombinerOperand	opAlpha[3];
	
	//float	lodBias;
	float	rgbScale;
	float	alphaScale;
	
	float	envColor[4];
	bool	sprited;		// point sprite only on GLES 1.x
	//other env par be ignored at this point

	TextureAttribute() : texture(0), sprited(false) {
		memset(this, 0, sizeof(TextureAttribute));
		rgbScale = alphaScale = 1.f;
	};
};

struct VertexData;

DECLARE_CLASS(Canvas, NilParent);
class Canvas : public Resource{
	DYNAMIC_CLASS(Canvas);
protected:
	CanvasBlend			mBlend;
	CanvasDepthTest		mDepth;
	TextureAttribute	mTex[2];

public:
	Canvas(){};
	virtual ~Canvas();
	Canvas(TypeLua const&);

	/**
	* get the size of each vertex element
	*/
	virtual size_t elementSize() const;

	/**
	* get vertex definition
	*/
	virtual void getVertexData( VertexData& ) const;

	CanvasBlend&	getBlend() { return mBlend; };
	CanvasDepthTest&	getDepthTest() { return mDepth; };
	TextureAttribute*	getTA() { return mTex; }

};

_CHAOS_END

#endif

