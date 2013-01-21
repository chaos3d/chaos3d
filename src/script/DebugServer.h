/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_DEBUGSERVER_H
#define _CHAOS_DEBUGSERVER_H

#include "common.h"
#include "ScriptManager.h"
#include <set>
#include <stack>
#include <string>

struct lua_State;
struct lua_Debug;

_CHAOS_BEGIN

namespace script{
	class CHAOS_API DebugServer : public ScriptManager::Debugger{
	public:
		struct Location{
			std::string		fileName;
			int				line;

			Location(Location const&rhs):fileName(rhs.fileName), line(rhs.line)
			{};

			Location& operator=(Location const&rhs){
				fileName = rhs.fileName;
				line = rhs.line;
				return *this;
			};

			Location(){};

			Location(std::string const& fn, int l) : fileName(fn), line(l){};

			inline
			bool operator<(Location const&rhs) const{
				if( line < rhs.line )
					return true;
				else if( line == rhs.line )
					return fileName.compare( rhs.fileName ) < 0;
				else return false;
			}
		};

		struct Function{
			char const*	name;
			char const* namewhat;
			char const* what;
			char const* source;
			int currentline;
			int linedefined;
			int lastlinedefined;

			Function(){};
			Function(char const* n, char const*nw, char const*w, char const*s, int l, int ld, int lld)
				: name(n), namewhat(nw), what(w), source(s),currentline(l), linedefined(ld),lastlinedefined(lld)
			{};
		};

		enum BreakMode{
			ModeStepInto,
			ModeStepOver,
			ModeStepOut,
			ModeBreakpoint,
		};

		typedef	std::set<Location>		TBreakpoint;
		typedef std::stack<Function>	TFuncStack;
		typedef std::map<lua_State*, TFuncStack>	TStack;

	protected:
		TBreakpoint	mBreakpoint;
		TStack		mStack;
		BreakMode	mMode;
		int			mStopLevel;

		virtual void line(lua_State*, lua_Debug *);
		virtual void return_(lua_State*, lua_Debug *);
		virtual void call(lua_State*, lua_Debug *);
		virtual void count(lua_State*, lua_Debug *);
		virtual void breakpoint(lua_State*, lua_Debug *){};

	public:
		DebugServer();
		virtual ~DebugServer();

		/**
		* debugging commands.
		*
		* @remark
		*	Issuing commands won't run scripts but set conditions. Scripts run
		*	under the control of the native engine. Thus to debug, a certain
		*	command should be issued before starting the engine.
		*
		*	The function will return immediately. Once specified conditions
		*	are matched, breakpoint is called where the script is suspended
		*	until it returns. Further conditions can then be specified.
		*	
		*/
		void	stepInto();
		void	stepOver();
		void	stepOut();
		void	continue_();

		TStack&		getStack() { return mStack; };
		BreakMode	getMode() const { return mMode; };

		/**
		* add a breakpoint
		* @remark
		*	file name is the same pattern as used for url, starting with "app://"
		*	or such. If nil, then bootstrap script is default.
		*/
		void		addBreakpoint( int line, char const* fileName = 0);
		TBreakpoint&	getBreakpoint() { return mBreakpoint;};

		TBreakpoint const&	getBreakpoint() const { return mBreakpoint;};
	};
}

_CHAOS_END;

#endif