#include "game_object.h"

game_object* game_object::find_by_tag(char const* tag, bool recursive) {
	game_object *node = nullptr;
	for(auto *child = _first_child;
        child != 0 && node == 0;
        child = child->_next_sibling)
        if(node->tag() == tag)
            return node;
    
    if(recursive) {
        for(auto *child = _first_child;
            child != 0 && node == 0;
            child = child->_next_sibling)
            node = child->find_by_tag(tag, true);
    }
    return node;
}


game_object& game_object::add_child(game_object* child, game_object* after) {
    return *this;
}

game_object& game_object::remove_all() {
    return *this;
}

game_object& game_object::remove_self() {
	if( _parent == 0 )
		return *this;
    
	if( _next_sibling != 0 )
        _next_sibling->_pre_sibling = _pre_sibling;
	if( _pre_sibling != 0 )
		_pre_sibling->_next_sibling = _next_sibling;
	
	if( _parent->_first_child == this )
		_parent->_first_child = _next_sibling;
	
	_parent = _next_sibling = _pre_sibling = 0;
    return *this;
}

game_object& game_object::move_upward() {
    return *this;
}

game_object& game_object::move_downward() {
    return *this;
}

game_object& game_object::move_top() {
    return *this;
}

game_object& game_object::move_bottom() {
    return *this;
}

game_object& game_object::move_afterward(game_object*) {
    return *this;
}

#if 0
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

void Scene2DNode::moveUpward(){
	if( mNextSibling == 0 || mParent == 0)
		return;
    
	mNextSibling->mPreSibling = mPreSibling;
	mPreSibling = mNextSibling;
    
	mNextSibling = mNextSibling->mNextSibling;
	mPreSibling->mNextSibling = this;
    
	if( mPreSibling == 0 )
		mParent->mFirstChild = this;
}

void Scene2DNode::moveDownward(){
	if( mPreSibling == 0 || mParent == 0)
		return;
    
	mPreSibling->mNextSibling = mNextSibling;
	mNextSibling = mPreSibling;
    
	mPreSibling = mPreSibling->mPreSibling;
	mNextSibling->mPreSibling = this;
    
	if( mPreSibling == 0 )
		mParent->mFirstChild = this;
}

void Scene2DNode::moveTop(){
	if( mParent == 0 || mNextSibling == 0)
		return;
    
	Scene2DNode* last = mParent->lastChild();
    
	mNextSibling->mPreSibling = mPreSibling;
	if( mPreSibling != 0 )
		mPreSibling->mNextSibling = mNextSibling;
	else
		mParent->mFirstChild = mNextSibling;
    
	last->mNextSibling = this;
	mPreSibling = last;
	mNextSibling = 0;
}

void Scene2DNode::moveBottom(){
	if( mParent == 0 || mPreSibling == 0)
		return;
	Scene2DNode *parent = mParent;
    
	this->retain();
	removeSelf();
	parent->addChild( this );
	this->release();
}
#endif
