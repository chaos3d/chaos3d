/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_UIPROGRESS_H
#define	_CHAOS_UIPROGRESS_H

#include "chaos_config.h"
#include "NodeUI.h"
#include "Scene2DNode.h"

_CHAOS_BEGIN

class AnimationState;
namespace ui{
	class UIProgress;
};
DECLARE_CLASS_A(ui::UIProgress, NodeUI);

namespace ui{
	class UIProgress : public NodeUI{
	protected:
		float		mPercent, mStep;

		virtual void parseElement(lua_State* L);
		void touchBegan(EventUITouchBegan&);
		void touchEnded(EventUITouchEnded&);
		void touchMoved(EventUITouchMoved&);
	public:
		class PercentApplier;

		UIProgress(Scene2DNode*);

		void setPercent(float);
		void setStep(float);
		float getPercent() const { return mPercent; }
		float getStep() const { return mStep; }

		AnimationState*	animation(float percent);
	};

	class UIProgress::PercentApplier{
	protected:
		UIProgress *mProgress;
	public:
		inline UIProgress*	getProgress() const { return mProgress; };
		inline UIProgress*&	getProgress() {return mProgress; };

		PercentApplier(UIProgress* t) : mProgress(t) {};
		inline bool operator() (float pos){
			mProgress->setPercent(pos);
			return mProgress->getNode()->getParent() != 0;
		}
	};
};

_CHAOS_END

#endif

