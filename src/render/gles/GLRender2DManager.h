/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLRENDER2DMANAGER_H
#define	_CHAOS_GLRENDER2DMANAGER_H

#include "chaos_config.h"
#include "2d/Render2DManager.h"
#include "2d/VertexData.h"
#include <list>

_CHAOS_BEGIN

struct GLRender2DManagerImpl;
class CHAOS_PRIVATE GLRender2DManager : public Render2DManager{
protected:
	struct CanvasContext{
		VertexData	canvasVertex;
		void	*vertexData, *vertexDataEnd;
		void	*indexData, *indexDataEnd;
		size_t	elementSize;	// size of each vertex data
		short	indexCount;
	};

	Canvas*			mCurCanvas;
	GLRender2DManagerImpl*	mImpl;
	CanvasContext	mCurContext;
	VertexData		mCurData;

	void loadExtensions();
	void commitAndCreateContext(Canvas*);
	void renderCanvas();
	void resetState();
	void createImpl();

public:
	GLRender2DManager();
	virtual ~GLRender2DManager();

	GLRender2DManagerImpl* getImpl() const { return mImpl; };

	virtual void beginRender() {};
	virtual void endRender();
	virtual void setModelMatrix(Matrix4f const&);
	virtual void setProjectiveMatrix(Matrix4f const&);
	virtual void setViewport(cRect const&);
	virtual void clear(int type, float const* color);

	virtual void drawSprite(Sprite*);
	virtual void bindVertex(VertexData* const&vd);
	virtual void bindCanvas(Canvas* const&);

	virtual RenderWindow* createWindow();
	virtual HardwareBuffer*	createHardwareBuffer(size_t sizeInBytes, void* data = 0, int type = VERTEX_BUFFER, int usage = DYNAMIC_DRAW);
	virtual Texture* createTexture(int type, int minFilter, int magFilter, int wrapS, int wrapT, bool genMipmap);
	virtual RenderTexture* createRenderTexture(Texture* tex);
};

_CHAOS_END

#endif

