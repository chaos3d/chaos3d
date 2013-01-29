/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_UICHECKBOX_H
#define	_CHAOS_UICHECKBOX_H

#include "chaos_config.h"
#include "NodeUI.h"

_CHAOS_BEGIN

namespace ui{
	class UICheckBox;
};
DECLARE_CLASS_A(ui::UICheckBox, NodeUI);

namespace ui{

	class UICheckBox : public NodeUI{
		DYNAMIC_CLASS(UICheckBox);

	protected:
		Scene2DNode	*mCkNode;
		bool		mChecked;

		virtual void parseElement(lua_State* L);
		void touchEnded(EventUITouchEnded&);

	public:
		UICheckBox(Scene2DNode*);
		virtual ~UICheckBox();

		void setChecked(bool);
		bool getChecked() const { return mChecked; }
	};
}

_CHAOS_END

#endif

