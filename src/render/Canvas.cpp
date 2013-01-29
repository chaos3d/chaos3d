#include "2d/Canvas.h"
#include "2d/Sprite.h"
#include "2d/Texture.h"
#include "2d/Render2DManager.h"
#include "2d/VertexData.h"
#include "script/scripting.h"
#include "script/bind.h"
#include "script/lua.h"

//using namespace chaos;

IMPLEMENT_CLASS(Canvas, LIB_DOMAIN)
IMPLEMENT_FACTORY( LuaRefFactory<Canvas> )
IMPLEMENT_END;

static void setTexture( lua_State* L, TextureAttribute& ta){
	lua_pushnil( L );
	int n = lua_gettop(L);
	while( lua_next(L,-2) ){
		if( lua_type( L, -2 ) == LUA_TSTRING ){
			char const* key = lua_tostring(L, -2);

			if( strcmp( key, "tex") == 0 ){
				ScriptData *data = (ScriptData*) lua_touserdata( L, -1 );

				if( data == 0 || data->dataType != ScriptData::Instance
					|| !data->type->isDerived(TYPE_FROM_NAME(Texture)) )
					//todo check rtti
					LOG("Texture not applicable.");
				else{
					ta.texture = (Texture*)data->obj;
					ta.texture->retain();
				}
			}

			else if( strcmp( key, "sprited") == 0 ){
				ta.sprited = lua_toboolean( L, -1 ) == 1;
			}

			else if( strcmp( key, "envMode") == 0 ){
				int value = (int)lua_tonumber(L,-1);
				if( value < 0 || value > EM_REPLACE )
					LOG("EnvMode setting not applicable.");
				else
					ta.envMode = (EnvMode)value;
			}

			else if( strcmp( key, "filer") == 0 ){
				LUA_GET2(L);
				int value = (int)lua_tonumber(L,-2);
				if( value < 0 || value > LINEAR_MIPMAP_LINEAR )
					LOG("MinFilter setting not applicable.");
				else
					ta.minFilter = (FilterMode)value;
				value = (int)lua_tonumber(L,-1);
				if( value < 0 || value > LINEAR_MIPMAP_LINEAR )
					LOG("MagFilter setting not applicable.");
				else
					ta.magFilter = (FilterMode)value;
			}

			/*else if( strcmp( key, "filer") == 0 ){
				LUA_GET2(L);
				int value = (int)lua_tonumber(L,-2);
				if( value < 0 || value > LINEAR_MIPMAP_LINEAR )
					LOG("MinFilter setting not applicable.");
				else
					ta.minFilter = (FilterMode)value;
				value = (int)lua_tonumber(L,-1);
				if( value < 0 || value > LINEAR_MIPMAP_LINEAR )
					LOG("MagFilter setting not applicable.");
				else
					ta.magFilter = (FilterMode)value;
			}*/

			else if( strcmp( key, "scale") == 0 ){
				LUA_GET2(L);
				float value = (float)lua_tonumber(L,-2);
				if( value <= 0.f )
					LOG("RGB scale setting not applicable.");
				else
					ta.rgbScale = value;
				value = (float)lua_tonumber(L,-1);
				if( value <= 0.f)
					LOG("Alpha scale setting not applicable.");
				else
					ta.alphaScale = value;
			}

			else if( strcmp( key, "wrap") == 0 && lua_istable(L,-1)){
				LUA_GET2(L);
				int value = (int)lua_tonumber(L,-2);
				if( value < 0 || value > REPEAT )
					LOG("WrapMode setting not applicable.");
				else
					ta.wrapS = (WrapMode)value;
				value = (int)lua_tonumber(L,-1);
				if( value < 0 || value > REPEAT )
					LOG("WrapMode setting not applicable.");
				else
					ta.wrapT = (WrapMode)value;
			}
		}
		lua_settop(L,n);
	};
}

Canvas::Canvas( TypeLua const& lua){
	lua_State* L(lua.getL());
	lua.get(L);

	if( !lua_istable(L,-1) ){
		LOG("Expected a table. (Canvas)");
		lua_pop(L,1);
		return;
	}

	lua_pushnil(L);
	while( lua_next(L, -2 ) ){
		if( lua_type( L, -2 ) == LUA_TSTRING ){
			char const* key = lua_tostring(L, -2);

			if( strcmp( key, "depth") == 0 ){
				int value = (int)lua_tonumber(L,-1);
				if( value < 0 || value > CF_DISABLED )
					LOG("Depth setting not applicable.");
				else
					mDepth.compareFunc = (CompareFunction)value;
			}
			
			else if( strcmp( key, "blend") == 0 ){
				if( lua_isboolean(L,-1) && lua_toboolean(L,-1)){
					mBlend.blendSrc = BF_DISABLED;
				}else if( lua_istable(L,-1) ){
					lua_rawgeti(L, -1, 1);
					int value = (int)lua_tonumber(L,-1);
					if( value < 0 || value > BF_DISABLED )
						LOG("Blend setting not applicable.");
					else
						mBlend.blendSrc = (BlendFactor)value;

					lua_rawgeti(L, -2, 2);
					value = (int)lua_tonumber(L,-1);
					if( value < 0 || value > BF_DISABLED )
						LOG("Blend setting not applicable.");
					else
						mBlend.blendDest = (BlendFactor)value;

					lua_rawgeti(L, -3, 3);
					value = (int)lua_tonumber(L,-1);
					if( value < 0 || value > BE_REV_SUBTRACT )
						LOG("Blend setting not applicable.");
					else
						mBlend.blendEq = (BlendEquation)value;

					lua_pop(L,3);
				}
			}

			else if( strcmp( key, "texture0") == 0 ){
				setTexture(L, mTex[0] );
			}

			else if( strcmp( key, "texture1") == 0 ){
				setTexture(L, mTex[1] );
			}

			else 
				LOG("Not a supported canvas setting.");

		};

		lua_pop(L,1);
	};
};

size_t Canvas::elementSize() const{
	return sizeof(SpriteLayout);
}

void Canvas::getVertexData( VertexData& vd) const{
	Render2DManager* rm = Render2DManager::getInstance();

	vd.type = VertexData::TRIANGLE_LIST;
	vd.idxType = VertexData::USHORT;
	vd.indexBuffer = rm->cachedIndexBuffer();
	vd.vertexBuffer = rm->cachedVertexBuffer();

	VertexAttribute& pos = vd.vertexAttr[0];
	VertexAttribute& tex = vd.vertexAttr[1];
	VertexAttribute& color = vd.vertexAttr[2];

	pos.idx = VertexAttribute::POSITION;
	pos.type = VertexAttribute::FLOAT;
	pos.stride = sizeof(SpriteLayout);
	pos.size = 3;
	pos.offset = 0;

	tex.idx = VertexAttribute::TEXCOORD_0;
	tex.type = VertexAttribute::FLOAT;
	tex.stride = sizeof(SpriteLayout);
	tex.size = 2;
	tex.offset = sizeof(float) * 3;

	color.idx = VertexAttribute::COLOR;
	color.type = VertexAttribute::FLOAT;
	color.stride = sizeof(SpriteLayout);
	color.size = 4;
	color.offset = sizeof(float) * 5;
}

Canvas::~Canvas(){
	if( mTex[0].texture != 0 )
		mTex[0].texture->release();
	if( mTex[1].texture != 0 )
		mTex[1].texture->release();
}
