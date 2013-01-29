/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_RENDER2DMANAGER_H
#define	_CHAOS_RENDER2DMANAGER_H

#include "chaos_config.h"
#include "core/math.h"
#include "core/RTTI.h"
#include <vector>

_CHAOS_BEGIN

class Texture;
class Canvas;
class Sprite;
class RenderWindow;
class RenderTexture;
class HardwareBuffer;
struct VertexData;

/**
 * Render2D Manager, an abstract factory
 * now it is a singleton, then only one renderer can be created
 * but it should be registered as an extension point in the future
 */
DECLARE_CLASS(Render2DManager, NilParent);
class Render2DManager : public Singleton<Render2DManager>{
	
public:
	enum{
		VERTEX_BUFFER,
		INDEX_BUFFER
	};

	enum{
		DYNAMIC_DRAW,
		STATIC_DRAW,
	};

	enum{
		CLEAR_COLOR = 0x1,
		CLEAR_DEPTH = 0x2
	};
	enum{
		Cache4Vertex = 256 * 36,
		Cache4Index = 384 * 2
	};

protected:
	typedef std::vector<HardwareBuffer*>	TBufferList;
	TBufferList		mCachedVB[2];	// 0, cached; 1, used
	TBufferList		mCachedIB[2];
	
public:
	Render2DManager();
	virtual ~Render2DManager();
	
	virtual void beginRender() {};
	virtual void endRender();
	virtual HardwareBuffer*	cachedVertexBuffer();
	virtual HardwareBuffer* cachedIndexBuffer();

	virtual void setModelMatrix(Matrix4f const&) = 0;
	virtual void setProjectiveMatrix(Matrix4f const&) = 0;
	virtual void setViewport(cRect const&) = 0;
	virtual void clear(int type, float const* color) = 0;

	virtual void drawSprite(Sprite*) = 0;
	virtual void bindVertex(VertexData* const&) = 0;
	virtual void bindCanvas(Canvas* const&) = 0;

	virtual Texture* createTexture(int type, int minFilter, int magFilter, int wrapS, int wrapT, bool genMipmap) = 0;
	virtual HardwareBuffer*	createHardwareBuffer(size_t sizeInBytes, void* data = 0, int type = VERTEX_BUFFER, int usage = DYNAMIC_DRAW) = 0;
	// TODO:
	//  window properties: fullscreen, position, size, etc
	virtual RenderWindow* createWindow() = 0;
	virtual RenderTexture* createRenderTexture(Texture*) = 0;
};

_CHAOS_END

#endif

