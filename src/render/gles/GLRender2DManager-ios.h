#ifndef	_CHAOS_GLRENDER2DMANAGER_IOS_H
#define	_CHAOS_GLRENDER2DMANAGER_IOS_H

#include "chaos_config.h"
#include <OpenGLES/EAGL.h>
#include <OpenGLES/EAGLDrawable.h>
#include <UIKit/UIKit.h>
#include <vector>

_CHAOS_BEGIN

// the iOS context
struct GLRender2DManagerImpl{
	typedef std::vector<EAGLContext*> TContext;
	UIViewController*	controller;
	TContext	context;
	
	GLRender2DManagerImpl() : controller(nil){
		context.push_back([[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1]);
	};

	~GLRender2DManagerImpl() {
		[controller release];
		for ( TContext::iterator it = context.begin(); 
				it != context.end(); ++it)
		{
			[(EAGLContext*)(*it) release];
		}
	}

	EAGLContext* createContext(){
		EAGLContext *cxt = context.back();
		cxt = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1 sharegroup:[cxt sharegroup]];

		return [cxt autorelease];
	}

	void recycleContext(EAGLContext* cxt){
		context.push_back( [cxt retain] );
	}
};

_CHAOS_END

#endif