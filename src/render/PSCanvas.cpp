#include "2d/PSCanvas.h"
#include "2d/PSSprite.h"

#include "2d/Render2DManager.h"
#include "2d/VertexData.h"
#include "script/scripting.h"
#include "script/bind.h"
#include "script/lua.h"

//using namespace chaos;

IMPLEMENT_CLASS(PSCanvas, LIB_DOMAIN)
IMPLEMENT_FACTORY(LuaRefFactory<PSCanvas> )
IMPLEMENT_END;

size_t PSCanvas::elementSize() const{
	return sizeof(ParticleType::Particle);
}

void PSCanvas::getVertexData( VertexData& vd) const{
	typedef ParticleType::Particle Particle;
	Render2DManager* rm = Render2DManager::getInstance();

	vd.type = VertexData::POINT_LIST;
	vd.indexBuffer = 0;
	vd.vertexBuffer = rm->cachedVertexBuffer();

	VertexAttribute& pos = vd.vertexAttr[0];
	VertexAttribute& size = vd.vertexAttr[1];
	VertexAttribute& color = vd.vertexAttr[2];

	Particle layout;
	pos.idx = VertexAttribute::POSITION;
	pos.type = VertexAttribute::FLOAT;
	pos.stride = sizeof(Particle);
	pos.size = 2;
	pos.offset = (char*)&layout.pos[0] - (char*)&layout;

	size.idx = VertexAttribute::POINT_SPRITE_SIZE;
	size.type = VertexAttribute::FLOAT;
	size.stride = sizeof(Particle);
	size.size = 1;
	size.offset = (char*)&layout.size - (char*)&layout;

	color.idx = VertexAttribute::COLOR;
	color.type = VertexAttribute::FLOAT;
	color.stride = sizeof(Particle);
	color.size = 4;
	color.offset = (char*)&layout.color[0] - (char*)&layout;
}