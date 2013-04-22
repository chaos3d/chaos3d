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

#include "common/common.h"
#include "common/ReferencedCount.h"
#include "wm4-algebra/algebra.h"

#include <string>
#include <stack>

_CHAOS_BEGIN

class Scene2DManager;
class Sprite;
class NodeUI;
class NodeFrame;
class Transform;
class NodeColor;

/*
scene node
*/

// TODO:
// * component as a list of template variables
// * copy create component
// * manages the hierachy of the scene tree and update status for each component (dirty flag)
class CHAOS_API Scene2DNode : public ReferencedCount{
public:
	friend class Scene2DManager;

	enum{	// dirty flag
		D_CLEAR = 0,
		D_TRANSFORM = 1,
		D_FRAME = 1<<1,
		D_COLOR = 1<<2,
		D_ALL = -1,
	};
    
    class iterator{
        friend class Scene2DNode;
        
        iterator(Scene2DNode* node) : _current(node){
            SAFE_RETAIN(_current);
        };
        
    public:
        iterator(const iterator& it){
            _current = it._current;
            SAFE_RETAIN(_current);
        }
        
        ~iterator(){
            SAFE_RELEASE0(_current);
        }
        
        iterator& operator=(const iterator& rhs){
            SAFE_REFAGN(_current, rhs._current);
            return *this;
        }
        
        iterator& operator++() {
            assert(_current != NULL);
            Scene2DNode* cur = _current;
            _current = _current->nextSibling();
            cur->release();
            SAFE_RETAIN(_current);
            return *this;
        }
        
        iterator& operator--() {
            assert(_current != NULL);
            Scene2DNode* cur = _current;
            _current = _current->preSibling();
            cur->release();
            SAFE_RETAIN(_current);
            return *this;
        }
        
        bool operator==(iterator const& rhs) const {
            return _current == rhs._current;
        }
        
        Scene2DNode* operator*() const{
            return _current;
        }
        
        static iterator end() {
            return iterator(NULL);
        }

    private:
        Scene2DNode *_current;
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
	//virtual void parseLua(TypeLua const&);

    // TODO: copy construction for components
    template<typename C>
    C* create(C const& comp){
        return NULL;
    };
    
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
	void		addChild( Scene2DNode* child, Scene2DNode* after = 0 );
	void		removeAllChildren( );
	void		removeSelf();
	void		relocateTo(Scene2DNode* parent, Scene2DNode* after = 0);
	void		moveUpward();
	void		moveDownward();
	void		moveTop();
	void		moveBottom();
	void		moveAfterward(Scene2DNode*);
	void		removeWhenDone(void *);
	iterator	childIterator() const { return iterator(mFirstChild); };

	//void		addChildren( TypeLua const& lua, Scene2DNode* after = 0 );
	//void		removeChildren( TypeLua const& lua ); // this can be easily done by removeSelf

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
	//TypeLua const& getLua() { return mLua; };
	//void setLua(TypeLua const&lua) { mLua = lua; };

	virtual void		update();
	virtual void		postUpdate(){};

    // depth-first/pre-order iterating all nodes
    // if the visitor returns:
    enum{
        DFS_CONTINUE,   // continue to next node
        DFS_PRUNE,      // stop visiting the children
        DFS_STOP,       // stop and return the node
    };
    // if all nodes are visited, it returns null
    template<class F>
    Scene2DNode* dfs( F const& visitor ){
        int result = visitor(this);
        
        if(result != DFS_STOP)
            return this;
        
        if(mFirstChild == 0)
            return NULL;
        
        std::stack<Scene2DNode*> nodes;
        nodes.push( mFirstChild );
        
        Scene2DNode* node = nodes.top();
        
        do{
            result = visitor( node );
            
            if(result == DFS_STOP)
                return node;
            
            nodes.pop();
            if( node->mNextSibling != 0 )
                nodes.push( node->mNextSibling );
            
            if(result != DFS_PRUNE && node->mFirstChild != 0 )
                nodes.push( node->mFirstChild );
            
            if( nodes.empty() )
                break;
            else
                node = nodes.top();
        }while(1);
        return NULL;
    }

    class Updater{
    public:
        inline int operator() (Scene2DNode* node) const{
            node->update();
            return DFS_CONTINUE;
        };
    };
    
    class ClearFlag{
    public:
        inline int operator() (Scene2DNode* node) const{
            node->clearFlag();
            return DFS_CONTINUE;
        };
    };
};

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
