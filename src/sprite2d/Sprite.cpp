#include "2d/Sprite.h"
#include "2d/Scene2DNode.h"
#include "2d/Transform.h"
#include "2d/Canvas.h"
#include "2d/Texture.h"
#include "script/lua.h"
#include "script/bind.h"
#include "script/scripting.h"

#include "2d/PSSprite.h"
#include "2d/TiledSprite.h"
#include "2d/MultiSprite.h"
#include "2d/MapTiledSprite.h"
#include "2d/PartialSprite.h"
#include "2d/PartialCircleSprite.h"
#include "2d/PartialRevSprite.h"

//using namespace chaos;

IMPLEMENT_CLASS(Sprite, LIB_DOMAIN )
IMPLEMENT_FUNC(animation, (&Sprite::animation<Sprite::SpriteApplier, LinearTiming>) )
IMPLEMENT_FUNC(setBound, (&Sprite::setBound) )
IMPLEMENT_FUNC(getBound, (&Sprite::getBoundLua) )
IMPLEMENT_FUNC(setFrame, (&Sprite::setFrame) )
IMPLEMENT_FUNC(getFrame, (&Sprite::getFrameLua) )
IMPLEMENT_FUNC(update, (&Sprite::forceUpdate) )
IMPLEMENT_PROP(canvas, (&Sprite::setCanvas), &Sprite::getCanvas )
IMPLEMENT_END;

IMPLEMENT_CLASS(SpriteAnimation ,LIB_DOMAIN)
IMPLEMENT_FACTORY( (LuaRefFactory<SpriteAnimation>) )
IMPLEMENT_END;

Sprite* Sprite::createSprite(Scene2DNode *n, TypeLua const& lua){
	ASSERT( n != 0 );
	Sprite* spr(0);
	lua_State *L(lua.getL());
	int top = lua_gettop(L);
	lua.get(L);

	ASSERT( lua_istable(L,-1) );
	lua_pushstring(L, "type");
	lua_rawget(L,-2);

	if( lua_type(L,-1) == LUA_TSTRING) {
		char const* type = lua_tostring(L,-1);
		if( strcmp(type, "tiled") == 0 ){
			spr = new TiledSprite(n);
		}else if( strcmp(type, "ps") == 0 ){
			spr = new PSSprite(n);
		}else if( strcmp(type, "multi") == 0 ){
			spr = new MultiSprite(n);
		}else if( strcmp(type, "map") == 0 ){
			spr = new MapTiledSprite(n);
		}else if( strcmp(type, "part") == 0 ){
			spr = new PartialSprite(n);
		}else if( strcmp(type, "part rev") == 0 ){
			spr = new PartialRevSprite(n);
		}else if( strcmp(type, "part circle") == 0 ){
			spr = new PartialCircleSprite(n);
		}else
			LOG("Not a support sprite type.");

	}else if( lua_type(L,-1) == LUA_TUSERDATA){
		ScriptData *sd = (ScriptData*)lua_touserdata(L,-1);
		if( sd->dataType == ScriptData::Class && sd->class_ != 0 &&
			sd->class_->type == Type::CLASS)
		{
			Type *type(sd->class_->table->search(FACTORY_OP));
			if( type != 0 && type->type == Type::CONSTRUCTOR ){
				NodeFactory* factory = (NodeFactory*)type->factory;
				spr = (Sprite*)factory->createNodeObject(n);
			}else
				LOG("Unable to find the factory method.");

		}
		if( spr == 0 || !spr->getClassType()->isDerived(TYPE(Sprite) ) )
			LOG("Unable to create sprite element. expected a spite class name.");
	}else{
		spr = new Sprite(n);
	}

	if( spr != 0 ){
		lua_settop(L, top+1);
		spr->parseElement(L);
	}

	lua_settop(L, top);
	return spr;
}

Sprite::VertexData Sprite::sData;

Sprite::Sprite(Scene2DNode *node)
	: NodeComponent(node), mCanvas(0)
{
}

void Sprite::parseElement(lua_State* L){
	int top = lua_gettop(L);

	lua_rawgeti(L, top, 1);
	if( !lua_isnil(L,-1) ){
		ScriptData *data = (ScriptData*) lua_touserdata( L, -1 );

		if( data == 0 || data->dataType != ScriptData::Instance)
			//||data->type->isDerivedFrom()		//todo check rtti
			LOG("Canvas not applicable.");
		else
			mCanvas = (Canvas*)data->obj;
	}

	lua_settop( L, top );
	lua_rawgeti( L, top, 2 );
	float bound[4]={0.f,0.f,0.f,0.f};
	if( lua_istable(L,-1) ){
		LUA_GET4(L);
		setBound( bound[0]=(float)lua_tonumber(L,-4),bound[1]=(float)lua_tonumber(L,-3),
			bound[2]=(float)lua_tonumber(L,-2),bound[3]=(float)lua_tonumber(L,-1) );
	}

	lua_settop( L, top );
	lua_pushstring(L, "frame");
	lua_rawget( L, top );
	if( lua_istable(L,-1) ){
		LUA_GET4(L);
		setFrame( (float)lua_tonumber(L,-4),(float)lua_tonumber(L,-3),
			(float)lua_tonumber(L,-2),(float)lua_tonumber(L,-1) );
	}else{
		setFrame( -bound[2]/2.f,-bound[3]/2.f,bound[2],bound[3] );
	}

	lua_settop(L,top);
}

void Sprite::updateData(short startIndex, SpriteData& sd) const{
	SpriteLayout* vertex = (SpriteLayout*)(sd.vertexData = &sData.vertex);
	short* index = (short*)(sd.indexData = &sData.index);
	sd.numVertex = 4;
	sd.numIndex = 6;

	float const* color(ColorRGBA::WHITE);

	if( mNode->getColor() != 0 )
		color = mNode->getColor()->getColor();

	Rectanglef const& bound(mBound);
	Rectangle3f const& region(mRegion);

	memcpy( vertex->color, color, sizeof(float)*4 );
	memcpy( vertex->texCoord, (float const*)bound.GetMPCorner(), sizeof(float)*2);
	memcpy( vertex->vtCoord, (float const*)region.GetMMCorner(), sizeof(float)*3);
	++vertex;

	memcpy( vertex->color, color, sizeof(float)*4 );
	memcpy( vertex->texCoord, (float const*)bound.GetPPCorner(), sizeof(float)*2);
	memcpy( vertex->vtCoord, (float const*)region.GetPMCorner(), sizeof(float)*3);
	++vertex;

	memcpy( vertex->color, color, sizeof(float)*4 );
	memcpy( vertex->texCoord, (float const*)bound.GetMMCorner(), sizeof(float)*2);
	memcpy( vertex->vtCoord, (float const*)region.GetMPCorner(), sizeof(float)*3);
	++vertex;

	memcpy( vertex->color, color, sizeof(float)*4 );
	memcpy( vertex->texCoord, (float const*)bound.GetPMCorner(), sizeof(float)*2);
	memcpy( vertex->vtCoord, (float const*)region.GetPPCorner(), sizeof(float)*3);
	++vertex;

	*index++ = startIndex;
	*index++ = startIndex+1;
	*index++ = startIndex+2;
	*index++ = startIndex+1;
	*index++ = startIndex+2;
	*index++ = startIndex+3;
}

void Sprite::forceUpdate(){
	if( mNode->getTransform() == 0 )
		return;

	mNode->getTransform()->forceUpdate();
	updateRegion(getFrame());
}

void Sprite::updateRegion(Rectanglef const& frame){
	if( mNode->getTransform() == 0 )
		return;

	Matrix4f const& mat =mNode->getTransform()->getMatrix();
	Vector4f center(
		frame.Origin[0]+frame.Extent[0]/2.f,
		frame.Origin[1]+frame.Extent[1]/2.f, 0.f, 1.f);

	mRegion.Axis[0] = Vector3f(mat[0][0],mat[1][0],mat[2][0]);	// x-axis
	mRegion.Axis[1] = Vector3f(mat[0][1],mat[1][1],mat[2][1]);	// y-axis
	mRegion.Center  = mat*center;
	mRegion.Extent[0] = frame.Extent[0]/2.f;
	mRegion.Extent[1] = frame.Extent[1]/2.f;
	
}

void Sprite::setBound(float x, float y, float width, float height){
	if( mCanvas == 0 ){
		LOG("Canvas not available.");
		return;
	}

	Texture* tex = mCanvas->getTA()[0].texture;
	if( tex == 0 ){
		LOG("Texture not available");
		return;
	}

	Vector2f revSize = tex->getSize();
	mBound.Origin = Vector2f(x/revSize[0], y/revSize[1]);
	mBound.Extent[0] = width/revSize[0];
	mBound.Extent[1] = height/revSize[1];
}

void Sprite::setFrame(float x, float y, float width, float height){
	mFrame.Origin[0] = x ;
	mFrame.Origin[1] = y ;
	mFrame.Extent[0] = width;
	mFrame.Extent[1] = height;
}

Rectanglef Sprite::getBound() const{
	ASSERT( mCanvas != 0 );
	Texture* tex = mCanvas->getTA()[0].texture;
	ASSERT( tex != 0 && "Texture not available");

	Vector2f revSize = tex->getSize();
	return Rectanglef(Vector2f(mBound.Origin[0]*revSize[0], 
		mBound.Origin[1]*revSize[1]),
		mBound.Extent[0]*revSize[0],mBound.Extent[1]*revSize[1] );
}

TypeLua Sprite::getBoundLua() const{
	Texture* tex = mCanvas->getTA()[0].texture;
	if( tex == 0)
		return TypeLua();
	
	Vector2f revSize = tex->getSize();	
	lua_State *L = ScriptManager::getInstance()->getState();
	lua_newtable(L);
	lua_pushnumber(L, mBound.Origin[0]*revSize[0]);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, mBound.Origin[1]*revSize[1]);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, mBound.Extent[0]*revSize[0]);
	lua_rawseti(L, -2, 3);
	lua_pushnumber(L, mBound.Extent[1]*revSize[1]);
	lua_rawseti(L, -2, 4);
	return TypeLua(L);
}

TypeLua Sprite::getFrameLua() const{
	lua_State *L = ScriptManager::getInstance()->getState();
	lua_newtable(L);
	lua_pushnumber(L, mFrame.Origin[0]);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, mFrame.Origin[1]);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, mFrame.Extent[0]);
	lua_rawseti(L, -2, 3);
	lua_pushnumber(L, mFrame.Extent[1]);
	lua_rawseti(L, -2, 4);
	return TypeLua(L);
}

template<class KeyFrame>
struct SortFunctor{
	inline bool operator()(KeyFrame const& lhs, KeyFrame const& rhs){
		return lhs.stamp < rhs.stamp;
	}
};

SpriteAnimation::SpriteAnimation(TypeLua const& lua) {
	lua_State* L = lua.getL();

	int n = lua_gettop( L );
	do{
		lua.get(L);
		if( !lua_istable( L, -1 ) ){
			LOG("Expected a table. (SpriteAnimation)");
			break;
		}

		lua_pushnil( L );
		TKFList keyframes;
		keyframes.reserve(16);
		while( lua_next( L, -2 ) ){
			if( lua_type(L,-2) == LUA_TNUMBER ){
				if( lua_type(L,-1) == LUA_TTABLE ){
					KeyFrame kf( (float) lua_tonumber(L,-2) );
					LUA_GET4(L);
					/*lua_rawgeti( L, -1, 1);	lua_rawgeti( L, -2, 2);
					lua_rawgeti( L, -3, 3);	lua_rawgeti( L, -4, 4);*/
					kf.value.x = (float)lua_tonumber(L, -4);
					kf.value.y = (float)lua_tonumber(L, -3);
					kf.value.width = (float)lua_tonumber(L, -2);
					kf.value.height = ( float)lua_tonumber(L, -1);

					lua_rawgeti( L, -5, 5);
					ScriptData* data = (ScriptData*)lua_touserdata(L, -1);
					if( data != 0 && data->obj != 0 ){
						if( data->dataType != ScriptData::Instance ) //todo type checking
							LOG("Canvas not applicable. (SpriteAnimation)");
						else
							kf.value.canvas = (Canvas*)data->obj;
					}
					keyframes.push_back(kf);
				}
			}else if( lua_type(L,-1) == LUA_TBOOLEAN ){
				if( lua_type(L,-2) == LUA_TSTRING &&
					strcmp(lua_tostring(L,-2), "loop") == 0  )
					mLoop = lua_toboolean(L, -1) == 1;
			}

			lua_settop(L, n+2);
		}

		std::sort( keyframes.begin(), keyframes.end(), SortFunctor<KeyFrame>() );
		mKeyFrames = keyframes;

	}while(0);

	lua_settop(L, n );
}

