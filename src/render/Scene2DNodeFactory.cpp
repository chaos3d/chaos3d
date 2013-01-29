#include "2d/Scene2DManager.h"
#include "script/lua.h"

#include "2d/Scene2DNode.h"
#include "2d/PptCameraNode.h"
#include "2d/NodeFrameGlobal.h"
#include "2d/Sprite.h"
#include "2d/NodeUI.h"
#include "2d/TiledSprite.h"

#include <cstring>

//using namespace chaos;

Scene2DNode* Scene2DManager::createNode( char const* tag, char const* type ){
	if( strcmp( type, "node" ) == 0 ){
		return new Scene2DNode(tag);
	}else if( strcmp( type, "ppt camera" ) == 0 ){
		return new PptCameraNode(tag);
	}else
		return 0;
}

Scene2DNode* Scene2DManager::createNode( TypeLua const& lua ){
	lua_State* L(lua.getL());
	int n = lua_gettop( L );
	lua.get(L);
	if( !lua_istable(L,-1) ){
		if( lua_isnoneornil(L,-1) ){
			lua_newtable(L);
			TypeLua lua(lua_ref(L,1));
			Scene2DNode* node( createNode("", "node") );
			node->parseLua( lua );
			node->autorelease();
			return node;
		}else{
			LOG("Expected a table.");
			lua_pop(L,1);
			return 0;
		}
	}else{
		lua_rawgeti( L, -1, 1);
		char const* type(lua_tostring(L, -1));

		lua_rawgeti( L, -2, 2);
		char const* tag(lua_tostring(L, -1));

		if( type == 0 ) type = "node";
		if( tag == 0 ) tag = "";

		Scene2DNode* node( createNode(tag, type) );
		ASSERT( node != 0 );
		node->parseLua( lua );

		lua_settop(L, n);
		node->autorelease();
		return node;
	}
}

void Scene2DNode::parseLua(TypeLua const& lua){
	lua_State* L(lua.getL());
	int n = lua_gettop( L );
	lua.get(L);
	ASSERT( lua_istable(L,-1) );

	lua_pushstring(L, "children" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		addChildren( TypeLua(L));

	lua_settop(L, n+1);
	lua_pushstring(L, "transform");
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setTransform( new Transform(this, TypeLua(L)) );
	else if( !lua_isboolean(L,-1) || lua_toboolean(L,-1) != 0 ){
		setTransform( new Transform(this) );
	}
	
	lua_settop(L, n+1);
	lua_pushstring(L, "sprite" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setSprite( Sprite::createSprite(this, TypeLua(L)) );

	lua_settop(L, n+1);
	lua_pushstring(L, "color" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setColor( new NodeColor(this, TypeLua(L)) );

	lua_settop(L, n+1);
	lua_pushstring(L, "script" );
	lua_rawget(L, n+1);
	if( lua_isfunction(L, -1) )
		mLua = TypeLua(lua_ref(L,1));

	lua_settop(L, n+1);
	lua_pushstring(L, "ui" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setUI( NodeUI::createUI(this, TypeLua(L)) );

	lua_settop(L, n+1);
	lua_pushstring(L, "frame" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setFrame( new NodeFrame(this, TypeLua(L)) );
	else if( lua_isboolean(L,-1) ){
		if( lua_toboolean(L,-1) ){
			if( mSprite != 0 ){
				NodeFrame *frame( new NodeFrame(this) );
				frame->setFrameFromeSprite();
				setFrame( frame );
			}else{
				LOG("Unable to define default frame. (no sprite found)");
			}
		}
	}else if( mFrame == 0 ){
		if( mUI != 0 && mSprite != 0 ){
			NodeFrame *frame( new NodeFrame(this) );
			frame->setFrameFromeSprite();
			setFrame( frame );
		}else
			setFrame( new NodeFrameGlobal(this) );
	}

	if( mUI != 0 && mFrame != 0 && mTransform == 0 )
		setTransform( new Transform(this) );
	else if( mSprite != 0 && mTransform == 0 )
		setTransform( new Transform(this) );
	
	lua_settop(L,n);
}
