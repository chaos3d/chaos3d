#ifndef _RENDERPASS_H
#define _RENDERPASS_H

#include <string>

#include "common/common.h"
#include "common/ReferencedCount.h"
#include "wm4-algebra/Vector3.h"
#include "RenderTarget.h"
#include "RenderQueue.h"

_CHAOS_BEGIN

struct Viewport{
	uint16_t x, y, width, height;
};

class RenderPass : public ReferencedCount{
public:
	typedef Vector3f ClearColor;
	enum {
		COLOR = 0x1,
		DEPTH = 0x2,
		STENCIL = 0x4,
		COLOR_RED = 0x8,
		COLOR_GREEN = 0x10,
		COLOR_BLU = 0x20,
	};

	explicit RenderPass(std::string const&);
	float recentFPS();
	virtual void doRender();

	std::string const& tag();

private:
	std::string _tag;
	Viewport _port;
	uint8_t _clearMask;
	uint8_t _renderMask;
	ClearColor _clearColor;

	RenderTarget* _target;
	RenderQueue* _renderQueue;
};

_CHAOS_END

#endif
