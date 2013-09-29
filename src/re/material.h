#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "common/common.h"
#include "VertexData.h"

#include <string>
#include <vector>

_CHAOS_BEGIN

class texture;
class gpu_program;
class gpu_properties;

class material : public ReferencedCount{
public:

private:
    gpu_program *_program;
    gpu_properties *_properties;
    
};

_CHAOS_END

#endif
