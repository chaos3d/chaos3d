#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "common/common.h"
#include "VertexData.h"

#include <string>
#include <vector>

_CHAOS_BEGIN

class MaterialQuality;
class Texture;
class GpuProgram;

class MaterialChooser{
public:
	virtual bool pick(MaterialQuality const&) const = 0;
};

class MaterialQuality{
public:
	enum {
		MAX_PASSES = 8,
		MAX_TEXTURES = 8,	// ogl es 2.0 supports 8
	};

	~MaterialQuality();

private:
	// properties that decide to choose this quality, LOD, global settings, device, etc
	// TODO:

	int _passTags[MAX_PASSES];	// pass tags: in which pass this material will be applied. pass filter
	Texture* _unit[MAX_TEXTURES];
	GpuProgram* _program[2];    // vertex and frag
};

class Material : public ReferencedCount{
    typedef Wm4::Vector3f Vector3f;
public:
	void select(MaterialChooser const&);

	/* attributes for all qualities */
    // these are probably for editor or inspection
	virtual uint8_t numOfUniforms() const;
	virtual bool setUniform(const char* name, Vector3f const&);
	virtual uint8_t numOfAttributes() const;
	virtual std::string getAttribute(uint8_t idx);

private:
	std::vector<MaterialQuality> _qualities;	// usually just one
	uint8_t _activeQuality;
};

_CHAOS_END

#endif
