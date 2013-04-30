#ifndef _CHAOS_GPUPROGRAM_H
#define _CHAOS_GPUPROGRAM_H

#include "common/common.h"

_CHAOS_BEGIN

class GpuProgram {
public:
	// TODO: protected?
	virtual bool load() = 0;
	virtual bool compile() = 0;
	virtual bool link() = 0;
	virtual bool use() = 0;

	virtual uint8 getNumOfUniforms() const = 0;
};

_CHAOS_END
#endif
