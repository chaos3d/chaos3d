#include "Scene2DNode.h"

//#include "anim/anim.h"
#include "Transform.h"
#include "NodeUI.h"
#include "NodeFrame.h"
#include "NodeColor.h"

//using namespace chaos;

static void mutexCallback(void*, void *ud){
	if( ((Scene2DNode*)ud)->getParent() != 0 )
		((Scene2DNode*)ud)->removeSelf();
};

Scene2DNode::Scene2DNode(std::string const& tag):mDirtyFlag(D_CLEAR),
	mTransform(0), mColor(0), mFrame(0), mSprite(0),
	mParent(0), mFirstChild(0), mNextSibling(0),mPreSibling(0),
	mTag(tag), mUI(0)//, mLuaSuspended(false)
{
}

Scene2DNode::~Scene2DNode(){
	ASSERT( mParent == 0 );	// no parent reference

	Scene2DNode* child = mFirstChild;
	while(child != 0){
		Scene2DNode* del = child;
		child = child->mNextSibling;
		del->mParent = 0;
		del->mPreSibling = 0;
		del->mNextSibling = 0;
		del->release();		
	}

	SAFE_DELETE0(mTransform);
	SAFE_DELETE0(mColor);
	SAFE_DELETE0(mFrame);
	SAFE_DELETE0(mUI);
	//SAFE_DELETE0(mSprite);
#if defined(DEBUG)
	//LOG("Scene2D node is destroyed (%s: %08x)", mTag.c_str(), this);
#endif
	mFirstChild = 0;
};

Scene2DNode* Scene2DNode::lastChild() const{
	Scene2DNode *node(mFirstChild);
	for(;node != 0 && node->nextSibling() != 0 ; node = node->nextSibling() )
		;
	return node;
}

/*
TypeLua Scene2DNode::getChildren() const{
    lua_State *L = ScriptManager::getInstance()->getState();
    ScriptState ss(L);
	
    lua_newtable(L);
    int n = lua_gettop(L);
    int i = 0;
    for(Scene2DNode* node = mFirstChild; node != 0; node = node->nextSibling()){
        ss.push_(node);
        lua_rawseti(L, n, ++i);
    }
    lua_settop(L,n);
    return TypeLua(L);
}*/

#if 0

void Scene2DNode::addChildren( TypeLua const& lua, Scene2DNode* after){
	lua_State *L(lua.getL());
	int n = lua_gettop(L);
	lua.get(L);
	if( !lua_istable(L,-1) ){
		LOG("Expected a table of scene nodes.");
		lua_settop(L,n);
		return;
	}

#if 1
	for(int i=1, t=luaL_getn(L, -1); i<=t; ++i){
		lua_rawgeti(L, n+1, i);
		ScriptData *udata = (ScriptData*)lua_touserdata(L,-1);
		if( udata == 0 || udata->dataType != ScriptData::Instance
		   /*|| udata->type->isDerived( )*/){
			LOG( "Child is not a scene node."  );
		}else
			addChild( (Scene2DNode*)udata->obj, after );
		lua_pop(L,1);
	}
#else
	lua_pushnil(L);
	while(lua_next(L,-2)){
		ScriptData *udata = (ScriptData*)lua_touserdata(L,-1);
		if( udata == 0 || udata->dataType != ScriptData::Instance
			/*|| udata->type->isDerived( )*/){
			LOG( "Child is not a scene node."  );
		}else
			addChild( (Scene2DNode*)udata->obj, after );
		lua_pop(L,1);
	};
#endif
	lua_settop(L,n);
}

void Scene2DNode::removeChildren( TypeLua const& lua ){
	lua_State *L(lua.getL());
	int n = lua_gettop(L);
	lua.get(L);
	ASSERT( lua_istable(L,-1) );

	lua_pushnil(L);
	while(lua_next(L,-2)){
		ScriptData *udata = (ScriptData*)lua_touserdata(L,-1);
		if( udata == 0 || udata->dataType != ScriptData::Instance
			/*|| udata->type->isDerived( )*/){
			LOG( "Child is not a scene node."  );
		}else
			( (Scene2DNode*)udata->obj )->removeSelf();
		lua_pop(L,1);
	};
	lua_settop(L,n);
}

#endif
/*

void Scene2DNode::addChild( Scene2DNode* child, Scene2DNode* after ){
	if( child == 0 || (child->mParent == this && child == mFirstChild))
		return;

	if( child->mParent != 0 )
		child->mParent->removeChild( child );
	child->mParent = this;

	child->mNextSibling = mFirstChild;
	child->mPreSibling = 0;

	if( mFirstChild != 0 )
		mFirstChild->mPreSibling = child;
	
	mFirstChild = child;
	child->mDirtyFlag = D_ALL;	// need to recalculate when adding to a parent node
	child->retain();

	return;
}
*/

void Scene2DNode::relocateTo(Scene2DNode* parent, Scene2DNode* after){
	if( parent == 0 || mTransform == 0 
	   || parent->getTransform() == 0 
	   || (mParent == parent && mPreSibling == after))
		return;
	
	mTransform->forceUpdate();
	mTransform->relocate(parent->getTransform());
	parent->addChild(this, after);
}

Scene2DNode& Scene2DNode::addChild( Scene2DNode* child, Scene2DNode* after ){
//	if( after == 0 )
//		after = mFirstChild;

	if( child == 0 || (child->mParent == this && (child == after|| child->mPreSibling == after) ))
		return *this;

	child->retain();
	if( child->mParent != 0 )
		( child )->removeSelf();
	child->mParent = this;

	if( after != 0 ){
		child->nextSibling() = after->nextSibling();
		child->preSibling() = after;
		after->nextSibling() = child;
		if( child->nextSibling() != 0 )
			child->nextSibling()->preSibling() = child;
	}else{
		child->nextSibling() = firstChild();
		child->preSibling() = 0;		
		if( firstChild() != 0 )
			firstChild()->preSibling() = child;
		firstChild() = child;
	}
	
	child->mDirtyFlag = D_ALL;	// need to recalculate when adding to a parent node

	return *this;
}

#if 0

void Scene2DNode::removeWhenDone(void *mutex){
	class RemoveTriggerable : public Triggerable {
		Scene2DNode *mNode;
	public:
		RemoveTriggerable(Scene2DNode *node) : mNode(node){
			mNode->retain();
		};
		virtual ~RemoveTriggerable(){
			mNode->release();
		};

		virtual void activated(){
			mNode->removeSelf();
		}
	};

	( (Triggerable*)(new RemoveTriggerable(this))->autorelease() )->wait4( mutex );
	//ScriptManager::getInstance()->wait(mutex, mutexCallback, this );
}
#endif

void Scene2DNode::removeSelf( ){
	if( mParent == 0 )
		return;

	if( mNextSibling != 0 )
			mNextSibling->mPreSibling = mPreSibling;
	if( mPreSibling != 0 )
		mPreSibling->mNextSibling = mNextSibling;
	
	if( mParent->mFirstChild == this )
		mParent->mFirstChild = mNextSibling;
	
	mParent = mNextSibling = mPreSibling = 0;
	release();
}

void Scene2DNode::removeAllChildren( ){
	Scene2DNode* child( mFirstChild );
	while(child != 0){
		Scene2DNode* del(child);
		child = child->nextSibling();

		del->mParent = del->mNextSibling = del->mPreSibling = 0;
		del->release();
	}

	mFirstChild = 0;
}

Scene2DNode& Scene2DNode::moveUpward(){
	if( mNextSibling == 0 || mParent == 0)
		return *this;

	if(mPreSibling == 0)
		mParent->mFirstChild = mNextSibling;
	else {
		mPreSibling->mNextSibling = mNextSibling;
	}
	
	mNextSibling->mPreSibling = mPreSibling;
	if(mNextSibling->mNextSibling != 0)
		mNextSibling->mNextSibling->mPreSibling = this;

	mPreSibling = mNextSibling;
	mNextSibling = mNextSibling->mNextSibling;
	mPreSibling->mNextSibling = this;
    return *this;
}

Scene2DNode& Scene2DNode::moveDownward(){
	if( mPreSibling == 0 || mParent == 0)
		return *this;

	if(mNextSibling != 0)
		mNextSibling->mPreSibling = mPreSibling;
	mPreSibling->mNextSibling = mNextSibling;
	mNextSibling = mPreSibling;

	mPreSibling = mPreSibling->mPreSibling;
	mNextSibling->mPreSibling = this;

	if( mPreSibling == 0 )
		mParent->mFirstChild = this;
	else {
		mPreSibling->mNextSibling = this;
	}
    return *this;
}

Scene2DNode& Scene2DNode::moveTop(){
	if( mParent == 0 || mNextSibling == 0)
		return *this;

	Scene2DNode* last = mParent->lastChild();

	mNextSibling->mPreSibling = mPreSibling;
	if( mPreSibling != 0 )
		mPreSibling->mNextSibling = mNextSibling;
	else
		mParent->mFirstChild = mNextSibling;

	last->mNextSibling = this;
	mPreSibling = last;
	mNextSibling = 0;
    return *this;
}

Scene2DNode& Scene2DNode::moveBottom(){
	if( mParent == 0 || mPreSibling == 0)
		return *this;
	Scene2DNode *parent = mParent;

	this->retain();
	removeSelf();
	parent->addChild( this );
	this->release();
    
    return *this;
}

Scene2DNode* Scene2DNode::childByTag( char const* tag ) const{
	Scene2DNode *node = 0;
	if( tag == 0 )
		return 0;
	if( mTag == tag )
		return (Scene2DNode *)this;
	else for(Scene2DNode *child = mFirstChild; 
		child != 0 && node == 0; 
		child = child->mNextSibling)
		node = child->childByTag( tag );
	return node;
}

Scene2DNode* Scene2DNode::childByIndex( int idx) const{
	Scene2DNode *node(0);
	if( idx < 0 ){
		node = mFirstChild;
		while( node != 0 && ++idx < 0)
			node = node->mNextSibling;
	}else{
		node = lastChild();
		while( node != 0 && --idx > 0)
			node = node->mPreSibling;
	}
	return node;
}

void Scene2DNode::setSprite(Sprite* spr){
	//if( mSprite != 0 )
	//	delete mSprite;
	
	mSprite = spr;
}

void Scene2DNode::setTransform(Transform *t){
    assert(t->getNode() == this);
    
    if(t == mTransform)
        return;
    
	if(mTransform != 0)
		delete mTransform;
	mTransform = t;
}

void Scene2DNode::setColor(NodeColor *c){
    assert(c->getNode() == this);
    
    if(c == mColor)
        return;
    
	if( mColor != 0 )
		delete mColor;
	mColor = c;
}

void Scene2DNode::setFrame(NodeFrame *f){
    assert(f->getNode() == this);
    
    if( mFrame == f)
        return;
    
	if( mFrame != 0 )
		delete mFrame;
	mFrame = f;
}

void Scene2DNode::setUI( NodeUI* ui ){
	if( mUI != 0 )
		delete mUI;
	mUI = ui;
}

void Scene2DNode::update(){
#if 0
	// run script first in case it updates transform or color
	if( !mLuaSuspended && mLua.valid() ){
		ScriptThread* thread(ScriptThread::thread(mLua));
		if( !thread->run(true) ){
			// thread suspended, wait
			class SuspendedTriggerable : public Triggerable {
				Scene2DNode *mNode;
			public:
				SuspendedTriggerable(Scene2DNode *node) : mNode(node){
					mNode->retain();
				};
				virtual ~SuspendedTriggerable(){
					mNode->release();
				};
				
				virtual void activated(){
					mNode->mLuaSuspended = false;
				}
			};
			mLuaSuspended = true;
			( (Triggerable*)(new SuspendedTriggerable(this))->autorelease() )->wait4( thread );
		}
	}
#endif
    
	// propagate dirty flag from parent node
	if( mParent != 0 )
		dirtyFlag() |= mParent->dirtyFlag();
	
	if( mTransform != 0 && transformFlag() ){
		mTransform->updateTransform( );
		//if( mSprite != 0 )
		//	mSprite->updateRegion(mSprite->getFrame());
	}
	
	if( mColor != 0 && colorFlag() ){
		mColor->updateColor();
	}
	
};
