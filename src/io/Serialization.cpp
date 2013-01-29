#include "io/Serialization.h"
#include "io/DataSource.h"
#include "io/DataStream.h"

#include "script/bind.h"
#include "script/lua.h"
#include "script/scripting.h"

//using namespace chaos;

IMPLEMENT_CLASS(Serialization, LIB_DOMAIN )
IMPLEMENT_FUNC(save, &Serialization::save )
IMPLEMENT_END;

IMPLEMENT_CLASS(DeSerialization, LIB_DOMAIN )
IMPLEMENT_FUNC(restore, &DeSerialization::restore )
IMPLEMENT_END;

static const int _lua_class[] = {
	':nil',		//#define LUA_TNIL		0
	':bul',		//#define LUA_TBOOLEAN		1
	':udt',		//#define LUA_TLIGHTUSERDATA	2
	':num',		//#define LUA_TNUMBER		3
	':str',		//#define LUA_TSTRING		4
	':tbl',		//#define LUA_TTABLE		5
	':fun',		//#define LUA_TFUNCTION		6
	':lud',		//#define LUA_TUSERDATA		7
	':thd',		//#define LUA_TTHREAD		8
};

class LuaHelper {
	typedef unsigned short size_h;
	struct Header{
		size_h length;
		char first;
		
		char* name(){
			return &first;
		};
		
		Header( size_h l, char const* name_ ): length(l), first('\0'){
			memcpy( name(), name_, l-sizeof(length));
		};
		
		static Header* newHeader( char const* type){
			size_h l = strlen( type ) + sizeof(size_h);
			void* buf = malloc( l * sizeof(char));
			return new(buf) Header( l, type );
		};
		
		static void deleteHeader( Header* header ){
			free( (void*) header );
		};
	};
	
public:
	static void serializeTable( lua_State* L, DataSource* ds );
	
	static void serializeString( lua_State* L, DataSource* ds ){
		char const* string_ = lua_tostring(L, -1);
		size_t len = strlen(string_);
		
		ds->write( &_lua_class[LUA_TSTRING], sizeof(int));
		ds->write( &len, sizeof(len) );
		ds->write( string_, len);
	};
	
	static void deserializeString( lua_State* L, DataStream* ds ){
		size_t len;
		ds->read( &len, sizeof(len));
		
		luaL_Buffer b;
		luaL_buffinit( L, &b);
		int bufsize = len > LUAL_BUFFERSIZE ? LUAL_BUFFERSIZE : len;
		
		while ( len > 0 ) {
			char* str = luaL_prepbuffer( &b );
			size_t rl = ds->read( str, bufsize);
			luaL_addsize( &b, rl);
			len -= rl;
		};
		
		luaL_pushresult( &b );
	};
};

void Serialization::save( TypeLua const& lua){
	lua_State *L ( lua.getL() );
	lua.get( L );
	
	switch ( lua_type(L, -1) ) {
		case LUA_TSTRING:
			LuaHelper::serializeString( L, mSource );
			break;
		default:
			break;
	}
}

bool DeSerialization::end(){
	return mStream->end();
}

TypeLua DeSerialization::restore(){
	int type;
	if( mStream->read( &type, sizeof(type)) != sizeof(type) )
		return TypeLua();
	
	lua_State *L (ScriptManager::getInstance()->getState());
	switch (type) {
		case ':str':
			LuaHelper::deserializeString( L, mStream );
			break;
		default:
			break;
	}
	return TypeLua(L);
}

