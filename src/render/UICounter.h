/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_UICOUNTER_H
#define	_CHAOS_UICOUNTER_H

#include "chaos_config.h"
#include "NodeUI.h"
#include "Scene2DNode.h"

_CHAOS_BEGIN

class AnimationState;
namespace ui{
	class UICounter;
};
DECLARE_CLASS_A(ui::UICounter, NodeUI);

namespace ui{
	class UICounter : public NodeUI{
		DYNAMIC_CLASS(UICounter);

	protected:
		int		mWidth, mTrail, mBase;
		float	mNum;

		virtual void parseElement(lua_State* L);
		void	updateMap();

	public:
		class NumberApplier;

		UICounter(Scene2DNode*);

		void	setWidth(int);
		void	setNum(float);
		void	setBase(int);
		void	setTrail(int t) { mTrail = t; };
		int		getWidth() const { return mWidth; };
		float	getNum() const { return mNum; };
		int		getBase() const { return mBase; };
		int		getTrail() const { return mTrail; };

		AnimationState* animation(int);
	};

	class UICounter::NumberApplier{
	protected:
		UICounter *mCounter;
	public:
		inline UICounter*	getCounter() const { return mCounter; };
		inline UICounter*&	getCounter() {return mCounter; };

		NumberApplier(UICounter* t) : mCounter(t) {};
		inline bool operator() (float val){
			mCounter->setNum(val);
			return mCounter->getNode()->getParent() != 0;
		}
	};

};

_CHAOS_END

#endif

