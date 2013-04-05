/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_UIRATING_H
#define	_CHAOS_UIRATING_H

#include "chaos_config.h"
#include "NodeUI.h"

_CHAOS_BEGIN

namespace ui{
	class UIRating;
};
DECLARE_CLASS_A(ui::UIRating, NodeUI);

namespace ui{

	class UIRating : public NodeUI{
		DYNAMIC_CLASS(UIRating);

	protected:
		int		mMax, mCurrent;
		bool	mOverlapper;
		
		virtual void parseElement(lua_State* L);
		void	updateMap();

		void touchBegan(EventUITouchBegan&);
		void touchEnded(EventUITouchEnded&);
		void touchMoved(EventUITouchMoved&);
	public:
		UIRating(Scene2DNode*);

		void setOverlapped( bool );
		void setMax( int );
		void setCurrent( int);
		int getMax() const { return mMax; }
		int getCurrent() const { return mCurrent; }
		bool getOverlapped() const { return mOverlapper; }
		
	};
};

_CHAOS_END

#endif

