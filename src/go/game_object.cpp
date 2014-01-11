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
	if( after == nullptr )
		after = _first_child;
    
	if( child == 0 || (child->_parent == this && (child == after|| child->_next_sibling == after) ))
		return *this;
    
	child->retain();
	if(child->_parent != nullptr)
		child->remove_self();
	child->_parent = this;
    
	child->_next_sibling = after;
    
	if( after != null ){
		child->_pre_sibling = after->_pre_sibling;
		after->_pre_sibling = child;
		if( child->_pre_sibling != null )
			child->_pre_sibling->_next_sibling = child;
	}else
		child->_pre_sibling = null;
	
	if( after == _first_child )
		_first_child = child;
    
    return *this;
}

game_object* game_object::last_child() const{
	game_object* node(_first_child);
	for(;node != 0 && node->next_sibling() != null ; node = node->next_sibling() )
		;
	return node;
}

game_object& game_object::remove_all() {
	auto* child(_first_child);
	while(child != null){
		auto* del(child);
		child = child->next_sibling();
        
		del->_parent = nullptr;
        del->_next_sibling = del->_pre_sibling = null;
		del->release();
	}
    
	_first_child = null;
    return *this;
}

void game_object::remove_self() {
	if( _parent == nullptr )
		return;
    
	if( _next_sibling != null )
        _next_sibling->_pre_sibling = _pre_sibling;
	if( _pre_sibling != null )
		_pre_sibling->_next_sibling = _next_sibling;
	
	if( _parent->_first_child == this )
		_parent->_first_child = _next_sibling;
	
	_parent = nullptr;
    _next_sibling = _pre_sibling = null;
    release();
}

game_object& game_object::move_upward() {
	if( _next_sibling == null || _parent == nullptr)
		return *this;
    
	_next_sibling->_pre_sibling = _pre_sibling;
	_pre_sibling = _next_sibling;
    
	_next_sibling = _next_sibling->_next_sibling;
	_pre_sibling->_next_sibling = this;
    
	if( _pre_sibling == null )
		_parent->_first_child = this;
    return *this;
}

game_object& game_object::move_downward() {
	if( _pre_sibling == null || _parent == nullptr)
		return *this;
    
	_pre_sibling->_next_sibling = _next_sibling;
	_next_sibling = _pre_sibling;
    
	_pre_sibling = _pre_sibling->_pre_sibling;
	_next_sibling->_pre_sibling = this;
    
	if( _pre_sibling == 0 )
		_parent->_first_child = this;
    return *this;
}

game_object& game_object::move_top() {
	if( _parent == nullptr || _next_sibling == null)
		return *this;
    
	auto* last = _parent->last_child();
    
	_next_sibling->_pre_sibling = _pre_sibling;
	if( _pre_sibling != null )
		_pre_sibling->_next_sibling = _next_sibling;
	else
		_parent->_first_child = _next_sibling;
    
	last->_next_sibling = this;
	_pre_sibling = last;
	_next_sibling = null;
    return *this;
}

game_object& game_object::move_bottom() {
	if( _parent == nullptr || _pre_sibling == null)
		return *this;
    
	auto* parent = _parent;
    
	this->retain();
	remove_self();
	parent->add_child( this );
	this->release();
    return *this;
}
