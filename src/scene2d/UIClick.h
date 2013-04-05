/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_UICLICK_H
#define	_CHAOS_UICLICK_H

#include "chaos_config.h"
#include "2d/NodeUI.h"
#include "2d/EventUI.h"
#include "anim/anim.h"
#include "Sprite.h"

_CHAOS_BEGIN

namespace ui{

	class UIClick : public NodeUI{
		static Vector3fAnimation	sScaleAnim;
	protected:
		AnimationState*	mClickAnim;
		Sprite::SpriteValue	mRectOrig, mRectClick;
		bool	mReplaced;

	public:
		UIClick(Scene2DNode*);
		~UIClick();

		void setClickAnim(AnimationState*);
		void touchBegan(EventUITouchBegan&);
		void touchEnded(EventUITouchEnded&);

		virtual void parseElement(lua_State* L);
	};

}

_CHAOS_END

#endif

