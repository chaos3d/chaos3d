#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "common/common.h"

#include <string>
#include <vector>

_CHAOS_BEGIN

class MaterialQuality;
class MaterialChooser{
public:
	virtual bool pick(MaterialQuality const&) const = 0;
};

class MaterialQuality{
public:
	typedef std::vector<std::string> VertexAttribute;
	enum {
		MAX_PASSES = 8;
		MAX_TEXTURES = 8;	// ogl es 2.0 supports 8
	};

	~MaterialQuality();

private:
	// properties that decide to choose this quality, LOD, global settings, device, etc
	// TODO:

	int _passTags[MAX_PASSES];	// pass tags: in which pass this material will be applied. pass filter
	Texture* _unit[MAX_TEXTURES];
	VertexAttribute _vertexAttr;	// vertex attributes: normal, color, position, etc
	Shader* _shader;
};

class Material : public ReferencedCount{
public:
	void select(MaterialChoooser const&);

	/* attributes for all qualities */
	virtual unit8_t numOfUniforms() const;
	virtual bool setUniform(const char* name, Vector3f const&);
	virtual uint8_t numOfAttributes() const;
	virtual std::string getAttribute(uint8_t idx);

private:
	std::vector<MaterialQuality> _qualities;	// usually just one
	uint8_t _activeQuality;
};

_CHAOS_END

#endif
