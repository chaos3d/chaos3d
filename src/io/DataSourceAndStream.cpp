#include "io/DataSource.h"
#include "io/DataStream.h"
#include "io/Serialization.h"

#include "script/bind.h"
#include "script/lua.h"

//using namespace chaos;

IMPLEMENT_CLASS(DataSource, LIB_DOMAIN )
IMPLEMENT_FUNC(asSerialization, &DataSource::asSerialization )
IMPLEMENT_FUNC(close, &DataSource::close )
IMPLEMENT_END;

Serialization* DataSource::asSerialization(){
	Serialization* s = new Serialization(this);
	s->autorelease();
	return s;
}

IMPLEMENT_CLASS(DataStream, LIB_DOMAIN )
IMPLEMENT_FUNC(asDeSerialization, &DataStream::asDeSerialization )
IMPLEMENT_FUNC(close, &DataStream::close )
IMPLEMENT_END;

DeSerialization* DataStream::asDeSerialization(){
	DeSerialization* s = new DeSerialization(this);
	s->autorelease();
	return s;
}