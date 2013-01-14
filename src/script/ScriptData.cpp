#include "script/ScriptData.h"

//using namespace chaos;

const char* ScriptData::typeName(int type){

#define RETURN_TYPE(type)	case type: return #type
	switch( type )
	{
	RETURN_TYPE(Class);
	RETURN_TYPE(Instance);
	RETURN_TYPE(This);
	RETURN_TYPE(State);
	RETURN_TYPE(StateDef);
	RETURN_TYPE(GOClass);
	RETURN_TYPE(GOInst);
	};

	return "Unavailable";
}