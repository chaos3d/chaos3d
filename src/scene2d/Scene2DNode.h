/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCENE2DNODE_H
#define	_CHAOS_SCENE2DNODE_H

#include "chaos_config.h"
#include "core/RTTI.h"
#include "core/math.h"
#include "core/core.h"
#include "script/LuaType.h"
#include "script/scripting.h"

#include "NodeFrame.h"
#include "NodeColor.h"
#include "Transform.h"

#include <string>

_CHAOS_BEGIN

class Scene2DManager;
class AnimationState;
class Sprite;
class NodeUI;

/*
scene node
*/
DECLARE_CLASS(Scene2DNode, NilParent);
class CHAOS_API Scene2DNode : public ReferencedCount{
	//DYNAMIC_CLASS;

public:
	friend class Scene2DManager;
	/*
	class TranslateApplier;
	class RotateApplier;
	class ScaleApplier;

	friend class TransformApplier;
	friend class RotationApplier;
	friend class ScaleApplier;
	*/

	enum{	// dirty flag
		D_CLEAR = 0,
		D_TRANSFORM = 1,
		D_FRAME = 1<<1,
		D_COLOR = 1<<2,
		D_ALL = -1,
	};

protected:
	Scene2DNode		*mParent;			//parent node, weak reference
	Scene2DNode		*mNextSibling;		//next brother node
	Scene2DNode		*mPreSibling;		//previous brother node
	Scene2DNode		*mFirstChild;		//first child node

	Transform		*mTransform;
	NodeFrame		*mFrame;
	NodeColor		*mColor;
	NodeUI			*mUI;
	Sprite			*mSprite;
	int				mDirtyFlag;

	/**
	 * updating thread of the script that will be called every frame.
	 *
	 * the thread can also be suspended, where it will be no longer
	 * called until it is activated. So, it will wait for the event
	 * every frame. 
	 */
	TypeLua			mLua;
	bool			mLuaSuspended;

	std::string const	mTag;

public:
	Scene2DNode(std::string const& n = std::string());
	virtual ~Scene2DNode();

	std::string const& getTag() const { return mTag; };

	// lazy updating components
	int		dirtyFlag() const { return mDirtyFlag; };
	int&	dirtyFlag() { return mDirtyFlag; };
	bool	transformFlag() const { return (mDirtyFlag & D_TRANSFORM) != 0; };
	bool	colorFlag() const { return (mDirtyFlag & D_COLOR) != 0; };
	bool	frameFlag() const { return (mDirtyFlag & D_FRAME) != 0; };
	void	clearFlag() { mDirtyFlag = D_CLEAR; };

	// script intialisation
	// arg has to be a Lua Table
	virtual void parseLua(TypeLua const&);

	// Transform
	void		setTransform(Transform*);
	Transform*	getTransform() const { return mTransform; };

	// Color
	void		setColor(NodeColor*);
	NodeColor*	getColor() const { return mColor;};

	// Boundary frame
	void		setFrame(NodeFrame*);
	NodeFrame*	getFrame() const { return mFrame; };

	// ui
	void		setUI(NodeUI*);
	NodeUI*		getUI() const { return mUI; };

	// Scene tree
	void		addChildren( TypeLua const& lua, Scene2DNode* after = 0 );
	void		addChild( Scene2DNode* child, Scene2DNode* after = 0 );
	TypeLua		getChildren() const;	// script helper
	//deprecated: use removeSelf instead
	//bool		removeChild( Scene2DNode* child );
	void		removeAllChildren( );
	void		removeChildren( TypeLua const& lua );
	void		removeSelf();
	void		relocateTo(Scene2DNode* parent, Scene2DNode* after = 0);
	void		moveUpward();
	void		moveDownward();
	void		moveTop();
	void		moveBottom();
	void		moveAfterward(Scene2DNode*);
	void		removeWhenDone(void *);

	template<class F>
	void		sort(F const& compare);

	Scene2DNode*	childByTag( char const* ) const;
	Scene2DNode*	childByIndex( int ) const;
	Scene2DNode*	firstChild() const { return mFirstChild; };
	Scene2DNode*	nextSibling() const { return mNextSibling; };
	Scene2DNode*	preSibling() const { return mPreSibling; };
	Scene2DNode*	getParent() const { return mParent; }
	Scene2DNode*	lastChild() const;
	Scene2DNode*&	firstChild() { return mFirstChild; };
	Scene2DNode*&	nextSibling() { return mNextSibling; };
	Scene2DNode*&	preSibling() { return mPreSibling; };
	
	// Rendering
	void		setSprite(Sprite*);
	Sprite*		getSprite() const { return mSprite; };

	// script
	TypeLua const& getLua() { return mLua; };
	void setLua(TypeLua const&lua) { mLua = lua; };

	virtual void		update();
	virtual void		postUpdate(){};

};

TYPE_RET_REF(Scene2DNode);
//TYPE_RET_DYN(Scene2DNode);
/*
class Scene2DNode::TranslateApplier{
private:
	Scene2DNode	*mNode;
public:

	inline Scene2DNode*		getNode() const { return mNode; };
	inline Scene2DNode*&	getNode() {return mNode; };

	TranslateApplier(Scene2DNode* node) : mNode(node) {};
	inline void operator() (Vector3f const& pos){
		mNode->mTransform->setTranslate( pos );
		//mNode->dirtyFlag() |= D_TRANSFORM;
	}
};

class Scene2DNode::RotateApplier{
private:
	Scene2DNode	*mNode;
public:
	inline Scene2DNode*	getNode() const { return mNode; };
	inline Scene2DNode*&	getNode() {return mNode; };

	RotateApplier(Scene2DNode* node) : mNode(node) {};
	inline void operator() (Quaternionf const& rot){
		mNode->mTransform->setRotate( rot );
	}
};

class Scene2DNode::ScaleApplier{
private:
	Scene2DNode	*mNode;
public:
	inline Scene2DNode*	getNode() const { return mNode; };
	inline Scene2DNode*&	getNode() {return mNode; };

	ScaleApplier(Scene2DNode* node) : mNode(node) {};
	inline void operator() (Vector3f const& scale){
		mNode->mTransform->setScale( scale );
	}
};
*/

template<class F>
void Scene2DNode::sort(F const& less){
	// trivial not sort
	if( mFirstChild == 0 ||
		mFirstChild->nextSibling() == 0 )
		return;

	Scene2DNode *last(mFirstChild);
	Scene2DNode *next(last->mNextSibling);

	mFirstChild->mNextSibling = 0;

	while( next != 0 ){
		Scene2DNode *node(last);
		Scene2DNode *ins(next);
		next = next->mNextSibling;

		while( node != 0 && less(node, ins) ){
			ASSERT( !less(ins, node) && "Not strictedly less.");
			node = node->preSibling();
		}

		if( node == 0 ){
			ins->mNextSibling = mFirstChild;
			ins->mPreSibling = 0;
			mFirstChild->mPreSibling = ins;
			mFirstChild = ins;
		}else{
			ins->mNextSibling = node->mNextSibling;
			ins->mPreSibling = node;
			node->mNextSibling = ins;

			if( ins->mNextSibling == 0 ){
				last = ins;
			}else
				ins->mNextSibling->mPreSibling = ins;

		}
	}
}

_CHAOS_END

#endif
