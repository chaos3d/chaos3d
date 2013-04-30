#ifndef _RENDERTARGET_H
#define _RENDERTARGET_H

#include <wm4-algebra/algebra.h>
#include <common/common.h>
#include <common/ReferencedCount.h>

class RenderTarget : public ReferencedCount{
public:
	typedef Wm4::Vector2f Size;
    
	enum {
		Format565,
		Format8888,
	};

	virtual Size getSize() = 0;
	virtual int getColorFormat() = 0;
	virtual int getDepthFormat() = 0;
	virtual int getStencilFormat() = 0;

	virtual void bind() = 0;
	virtual void flush() = 0;
	virtual void screenshot() = 0;
};

#endif