#include "wrapper_lua.h"
#include "lua/lua.h"
#include "DataStream.h"
#include "DataSource.h"

static const char *streamReader (lua_State *L, void *data, size_t *size){
	static char buffer[1024];
	DataStream* ds = (DataStream*)data;

	*size = ds->read(buffer, 1024);
	return buffer;
}

static int sourceWriter (lua_State *L, const void* p, size_t sz, void* ud){
	DataSource *ds = (DataSource*)ud;
	if( ds->write( p, sz ) == sz )
		return 0;
	else
		return -1;
}

int luaio_load(lua_State *L, DataStream* ds, char const* chunkname){
	if(ds == NULL)
		return -1;	// TODO: error log

	return lua_load(L, streamReader, (void*)ds, chunkname);
}

int luaio_dump(lua_State *L, DataSource* ds){
	if(ds == NULL)
		return -1;	// TODO: error log
	return lua_dump( L, sourceWriter, ds );
}
