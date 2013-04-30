#ifndef _VERTEXDATA_H
#define _VERTEXDATA_H

#include "common/common.h"
#include "common/ReferencedCount.h"
#include <string>

_CHAOS_BEGIN

class VertexData : public ReferencedCount{
private:
    // GpuProgram should automatically map the attributes
    // to the shader and output more details i.e. diagonse
    // or errors
	struct Attribute {
		uint8_t stride;
		uint8_t offset;
        std::string name;
	};

	enum{
		MAX_ATTR = 8,	// ogl es 2.0 supports 8
	};

	Attribute _attr[MAX_ATTR];
};

_CHAOS_END

#endif
