#include "2d/gles/GLRender2DManager.h"
#include "2d/render.h"
#include "2d/gles/glrender.h"

//using namespace chaos;

static const GLenum modeMap[] = {
	GL_POINTS,
	GL_LINES,
	GL_LINE_LOOP,
	GL_LINE_STRIP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
};

static const GLenum typeMap[] = {
	GL_FLOAT,		//FLOAT
	//GL_CHAR,
};

static const GLenum blendEqMap[] = {
	GL_FUNC_ADD_OES,
	GL_FUNC_SUBTRACT_OES,
	GL_FUNC_REVERSE_SUBTRACT_OES
};

static const GLenum blendMap[] = {
	GL_ONE,
	GL_ZERO,
	GL_DST_COLOR,
	GL_SRC_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_ALPHA,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA
};

static const GLenum compareMap[] = {
	GL_NEVER,
	GL_ALWAYS,
	GL_LESS,
	GL_LEQUAL,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_GREATER
};

static const GLenum envModeMap[] ={
	GL_MODULATE,
	GL_ADD,
	GL_DECAL,
	GL_BLEND,
	GL_COMBINE,
	GL_REPLACE
};

static const GLenum texTypeMap[] = {
	0,
	GL_TEXTURE_2D,
	0,
};

static const GLenum indexTypeMap[] = {
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_SHORT

};

static const GLenum combinerFuncMap[] = {
	GL_REPLACE,
	GL_MODULATE,
	GL_ADD,
	GL_ADD_SIGNED,
	GL_INTERPOLATE,
	GL_SUBTRACT,
};

static const GLenum combinerSrcMap[] = {
	GL_PREVIOUS,
	GL_CONSTANT,
	GL_PRIMARY_COLOR,
	GL_TEXTURE,
	GL_TEXTURE0,
	GL_TEXTURE1,
};

static const GLenum combinerOperandMap[] = {
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA
};

GLRender2DManager::GLRender2DManager() : Render2DManager(), mCurCanvas(0), mImpl(0){
	loadExtensions();
	createImpl();
}

#if !defined(CHAOS_OSX)
RenderWindow* GLRender2DManager::createWindow(){
	RenderWindow* rw = new GLRenderWindow();
	rw->autorelease();
	return rw;
}

// nothing to be done here for non-iOS
void GLRender2DManager::createImpl() {
}

GLRender2DManager::~GLRender2DManager() {
}
#endif

#if defined(CHAOS_OSX) || defined(CHAOS_ANDROID)
void GLRender2DManager::loadExtensions(){
}
#endif

Texture* GLRender2DManager::createTexture(int type, int minFilter, int magFilter, int wrapS, int wrapT, bool genMipmap){
	Texture* tex = new GLTexture(type, minFilter, magFilter, wrapS, wrapT, genMipmap);
	tex->autorelease();
	return tex;
}

RenderTexture* GLRender2DManager::createRenderTexture(Texture* tex){
#if defined(CHAOS_WIN32)
	RenderTexture* rt = new GLRenderTextureES1((GLTexture*)tex);
#elif defined(CHAOS_OSX)
	RenderTexture* rt = new GLRenderTexture((GLTexture*)tex);
#elif defined(CHAOS_ANDROID)
	RenderTexture* rt = new GLRenderTexture((GLTexture*)tex);
#else
	RenderTexture* rt = 0;
	LOG("No texture support");
#endif
	rt->autorelease();

	return rt;
}


HardwareBuffer* GLRender2DManager::createHardwareBuffer(size_t sizeInBytes, void* data, int type, int usage){
	HardwareBuffer *buf = 0;

#if defined(WIN32) || defined(ANDROID)
	if( type == VERTEX_BUFFER ){
		buf = new GLVertexClientBuffer( sizeInBytes, data, usage == DYNAMIC_DRAW ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	}else if( type == INDEX_BUFFER ){
		buf = new GLVertexIndexClientBuffer( sizeInBytes, data, usage == DYNAMIC_DRAW ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	};
#else
	if( type == VERTEX_BUFFER ){
		buf = new GLVertexBuffer( sizeInBytes, data, usage == DYNAMIC_DRAW ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	}else if( type == INDEX_BUFFER ){
		buf = new GLVertexIndexBuffer( sizeInBytes, data, usage == DYNAMIC_DRAW ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );

	};
#endif

	buf->autorelease();
	return buf;
}

void GLRender2DManager::setModelMatrix(Matrix4f const& mat){
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( (float const*) mat.Transpose() );
}

void GLRender2DManager::setProjectiveMatrix(Matrix4f const&mat){
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( (float const*) mat.Transpose() );
}

void GLRender2DManager::setViewport(cRect const& rt){
	glViewport( (GLint)rt.pos.x, (GLint)rt.pos.y, (GLsizei)rt.size.width, (GLsizei)rt.size.height );
}

void GLRender2DManager::renderCanvas(){
	VertexData* const&vd = &mCurContext.canvasVertex;
	HardwareBuffer* const& vb = vd->vertexBuffer;
	HardwareBuffer* const& ib = vd->indexBuffer;

	vb->unLock();
	if( ib != 0 )
		ib->unLock();

	bindVertex( vd );
	bindCanvas( mCurCanvas );
	vb->bind();

	if( ib == 0 ){
		glDrawArrays( modeMap[vd->type], 0, mCurContext.canvasVertex.count);
	}else{
		ib->bind();
		glDrawElements( modeMap[vd->type], mCurContext.canvasVertex.count, indexTypeMap[vd->idxType], ib->getOffset(0) );
		ib->unBind();
	}

	vb->unBind();
}

void GLRender2DManager::bindCanvas(Canvas* const&canvas){
	CanvasBlend const	&mab = canvas->getBlend();
	CanvasDepthTest const	&madt = canvas->getDepthTest();

	// blend
	if( mab.blendSrc != BF_DISABLED && mab.blendDest != BF_DISABLED ){
		glEnable( GL_BLEND );
		glBlendFunc( blendMap[mab.blendSrc], blendMap[mab.blendDest] );
#if defined(CHAOS_OSX)
		glBlendEquationOES(blendEqMap[mab.blendEq]);
#endif
	}else{
		glDisable( GL_BLEND );
	}

	// depth test
	if( madt.compareFunc != CF_DISABLED ){
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( compareMap[madt.compareFunc] );
	}else{
		glDisable( GL_DEPTH_TEST );
	}

	// texture state
	for( int i=0; i<2; ++i ){
		TextureAttribute const &ta = canvas->getTA()[i];
		GLTexture* const& tex = *(GLTexture**)&ta.texture;

		glActiveTexture( GL_TEXTURE0 + i );
		if( tex != 0 ){
			glEnable( GL_TEXTURE_2D );

			glBindTexture(GL_TEXTURE_2D, tex->texId());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLTexture::filterMap[ta.minFilter]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLTexture::filterMap[ta.magFilter]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLTexture::wrapMap[ta.wrapS]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLTexture::wrapMap[ta.wrapT]);
					
			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, envModeMap[ta.envMode]);
			glTexEnvf( GL_TEXTURE_ENV, GL_RGB_SCALE, ta.rgbScale );
			glTexEnvf( GL_TEXTURE_ENV, GL_ALPHA_SCALE, ta.alphaScale );
			
			if( ta.envMode == EM_BLEND )
				glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, ta.envColor);
			else if( ta.envMode == EM_COMBINE ){
				//the constant value
				glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, ta.envColor);
				
				// RGB combiner
				glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, combinerFuncMap[ta.combineRGB]);
				glTexEnvi( GL_TEXTURE_ENV, GL_SRC0_RGB, combinerSrcMap[ta.srcRGB[0]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_SRC1_RGB, combinerSrcMap[ta.srcRGB[1]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_SRC2_RGB, combinerSrcMap[ta.srcRGB[2]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB, combinerOperandMap[ta.opRGB[0]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB, combinerOperandMap[ta.opRGB[1]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_RGB, combinerOperandMap[ta.opRGB[2]]);
				
				//Alpha combiner
				glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, combinerFuncMap[ta.combineAlpha]);
				glTexEnvi( GL_TEXTURE_ENV, GL_SRC0_ALPHA, combinerSrcMap[ta.srcAlpha[0]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_SRC1_ALPHA, combinerSrcMap[ta.srcAlpha[1]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_SRC2_ALPHA, combinerSrcMap[ta.srcAlpha[2]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, combinerOperandMap[ta.opAlpha[0]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, combinerOperandMap[ta.opAlpha[1]]);
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, combinerOperandMap[ta.opAlpha[2]]);
			}
			
			if( ta.sprited ){
				glEnable( GL_POINT_SPRITE_OES );
				glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
			}else{
				glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE );
				glDisable( GL_POINT_SPRITE_OES );
			}
		}else{
			//glDisable( GL_TEXTURE_1D );
			glDisable( GL_TEXTURE_2D );
			//glDisable( GL_TEXTURE_3D );
		}
	}
	glActiveTexture( GL_TEXTURE0 );
	
	if( glGetError() != GL_NO_ERROR )
		LOG("Unable to bind the material.");
}

void GLRender2DManager::bindVertex(VertexData* const&vd){
	VertexAttribute* va = vd->vertexAttr;
	HardwareBuffer* const& vb = vd->vertexBuffer;
	int idx = 0;

	while( ++idx <= 8 && va->idx != VertexAttribute::NONE){
		void *offset = vb->getOffset(va->offset);

		GLenum type = typeMap[va->type];
		switch( va->idx )
		{
		case VertexAttribute::POSITION:
			glVertexPointer( va->size, type, va->stride, offset);
			glEnableClientState( GL_VERTEX_ARRAY );
			break;		

		case VertexAttribute::COLOR:
			glColorPointer( va->size, type, va->stride, offset);
			glEnableClientState( GL_COLOR_ARRAY );
			break;			

		case VertexAttribute::NORMAL:
			glNormalPointer( va->size, va->stride, offset);
			glEnableClientState( GL_NORMAL_ARRAY );
			break;			

		case VertexAttribute::TEXCOORD_0:
			glClientActiveTexture( GL_TEXTURE0 );
			glTexCoordPointer( va->size, type, va->stride, offset);
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			break;			

		case VertexAttribute::TEXCOORD_1:
			glClientActiveTexture( GL_TEXTURE0 + 1);
			glTexCoordPointer( va->size, type, va->stride, offset);
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glClientActiveTexture( GL_TEXTURE0 );
			break;			

		case VertexAttribute::POINT_SPRITE_SIZE:
			glPointSizePointerOES( type, va->stride, offset);
			glEnableClientState( GL_POINT_SIZE_ARRAY_OES );
			break;

		default:
			LOG_STAT( ERROR, "Not a supported vertex attribute." );
			break;
		};
		++va;
	}
	
	if( glGetError() != GL_NO_ERROR )
		LOG("Errors when binding vertex settings.");
}


void GLRender2DManager::commitAndCreateContext(Canvas* canvas){

	if( mCurCanvas != 0 ){
		resetState();
		renderCanvas();
	}

	mCurCanvas = canvas;
	
	if( mCurCanvas != 0 ){
		mCurCanvas->getVertexData( mCurContext.canvasVertex );
		mCurContext.canvasVertex.count = 0;

		mCurContext.vertexData = mCurContext.canvasVertex.vertexBuffer->lock();
		mCurContext.vertexDataEnd = (char*)mCurContext.vertexData + Cache4Vertex;

		if( mCurContext.canvasVertex.indexBuffer != 0 ){
			mCurContext.indexData = mCurContext.canvasVertex.indexBuffer->lock();
			mCurContext.indexDataEnd = (char*)mCurContext.indexData + Cache4Index;
		}else{
			mCurContext.indexData = 0;
			mCurContext.indexDataEnd =(void*) 0xffff;	// maximize index data that index never be expired
		}

		mCurContext.indexCount = 0;
		mCurContext.elementSize = canvas->elementSize();
	}
}

void GLRender2DManager::endRender() {
	commitAndCreateContext(0);
	Render2DManager::endRender();
};

void GLRender2DManager::drawSprite(Sprite* spr){
	ASSERT( spr != 0 );

	if( mCurCanvas == 0 )
		commitAndCreateContext( spr->getCanvas() );

	SpriteData sd;
	spr->updateData(mCurContext.indexCount, sd);

	if( sd.numVertex == 0 || (sd.indexData != 0 && sd.numIndex == 0))
		return;
	
	size_t sizeVertex(0);
	size_t sizeIndex( sd.numIndex * sizeof( GLshort ) );

	// change canvas?
	if( spr->getCanvas() != mCurCanvas ||
	   // inde data overloaded?
	   (char*)mCurContext.indexData + sizeIndex > mCurContext.indexDataEnd ||
	   // vertex data overloaded?
	   (char*)mCurContext.vertexData + (sizeVertex = sd.numVertex * mCurContext.elementSize) > mCurContext.vertexDataEnd){
		// update the index counting
		short diff(mCurContext.indexCount);
		if (diff > 0 && sd.numIndex > 0 ) {
			short *idx( (short*) sd.indexData );
			for( int i = 0; i < sd.numIndex; ++i )
				*idx++ -= diff;
		}
		commitAndCreateContext(spr->getCanvas());
		sizeVertex = sd.numVertex * mCurContext.elementSize;	// canvas has been change, so that element size be changed too
	}

	memcpy( mCurContext.vertexData, sd.vertexData, sizeVertex );
	mCurContext.vertexData = (char*)mCurContext.vertexData + sizeVertex;

	if( mCurContext.indexData != 0 ){
		mCurContext.indexCount += sd.numVertex;	
		memcpy( mCurContext.indexData, sd.indexData, sizeIndex );
		mCurContext.indexData = (char*)mCurContext.indexData + sizeIndex;
		mCurContext.canvasVertex.count += sd.numIndex;
	}else
		mCurContext.canvasVertex.count += sd.numVertex;
}

void GLRender2DManager::resetState(){
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glActiveTexture( GL_TEXTURE0 );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_POINT_SPRITE_OES );

	glActiveTexture( GL_TEXTURE0 + 1 );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_POINT_SPRITE_OES );

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_POINT_SIZE_ARRAY_OES);

	glClientActiveTexture( GL_TEXTURE0 );
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture( GL_TEXTURE0 + 1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture( GL_TEXTURE0 );
}

void GLRender2DManager::clear(int type, float const* color){
	int clearBit = 0;
	if( type&CLEAR_COLOR ){
		glClearColor( color[0], color[1], color[2], color[3] );
		clearBit |= GL_COLOR_BUFFER_BIT;
	}

	if( type&CLEAR_DEPTH ){
		clearBit |= GL_DEPTH_BUFFER_BIT;
	}

	if( clearBit != 0 )
		glClear( clearBit );

}
