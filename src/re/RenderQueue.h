#ifndef _RENDERQUEUE_H
#define _RENDERQUEUE_H

#include <vector>

#include "common/common.h"
#include "common/ReferencedCount.h"
#include "Material.h"
#include "VertexData.h"

_CHAOS_BEGIN

/*
 * this could have attributes, i.e. uniforms
 * will add them later if really needed
 */
class RenderUnit{
public:
	RenderUnit(Material*, VertexData*);
	~RenderUnit();

	Material* getMaterial() const { return material; };
	VertexData* getVertexData() const { return vertexData; };

private:
	Material* material;
	VertexData* vertexData;
};

/**
 * each queue is a list of texture/shader/vertex
 * it is a separate class because it can be reused
 * or shared among render passes
 */
class RenderQueue : public ReferencedCount{
public:
	virtual void sort();	// probably based on the property of renderUnit

private:
	std::vector<RenderUnit> _units;
};

_CHAOS_END

#endif
