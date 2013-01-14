#include "script/DebugServer.h"
#include "script/lua.h"
#include "io/io.h"

using namespace std;
//using namespace chaos;
using namespace script;

DebugServer::DebugServer() :
	mStopLevel(0), mMode(ModeStepInto){
}

DebugServer::~DebugServer(){
}

void DebugServer::line(lua_State *L, lua_Debug *ar){
	switch( mMode )
	{
	case ModeBreakpoint:
	{
		lua_getinfo(L, "Sl", ar);
		TBreakpoint::iterator bp = 
			mBreakpoint.find(Location(string(ar->source), ar->currentline));
		if( bp != mBreakpoint.end() )
			breakpoint( L, ar );
		break;
	}	
	case ModeStepOver:
	case ModeStepOut:
		if( mStopLevel > 0 )
			break;

	case ModeStepInto:
		breakpoint( L, ar );
		break;
	}
}

void DebugServer::return_(lua_State *L, lua_Debug *ar){
	TFuncStack &fs = mStack[L];
	//Function const& top( fs.top());

	switch( mMode )
	{
	case ModeStepOver:
	case ModeStepOut:
		-- mStopLevel;
		break;

	case ModeStepInto:
		breakpoint( L, ar );
		break;
	}

	fs.pop();
	if( fs.empty() )
		mStack.erase(L);
}

void DebugServer::call(lua_State *L, lua_Debug *ar){
	lua_getinfo(L, "Sln", ar);

	TFuncStack &fs = mStack[L];

	fs.push(Function(ar->name, ar->namewhat, ar->what, ar->source,
		ar->currentline, ar->linedefined, ar->lastlinedefined));

	switch( mMode )
	{
	case ModeStepOver:
	case ModeStepOut:
		++ mStopLevel;
		break;

	case ModeStepInto:
		breakpoint( L, ar );
		break;
	}
}

void DebugServer::addBreakpoint(int line, char const* fileName){
	if( fileName == 0 )
		mBreakpoint.insert( Location(std::string("bootstrap"), line) );
	else
		mBreakpoint.insert(Location(IOManager::getInstance()->nameByURL(fileName),line));
}


void DebugServer::count(lua_State *L, lua_Debug *ar){
}

void DebugServer::stepInto(){
	mMode = ModeStepInto;
}

void DebugServer::stepOver(){
	mMode = ModeStepOver;
	mStopLevel = 0;
}

void DebugServer::stepOut(){
	mMode = ModeStepOut;
	mStopLevel = 1;
}

void DebugServer::continue_(){
	mMode = ModeBreakpoint;
}

