#include "game_object.h"
#include "component.h"
#include <stack>

uint32_t game_object::_number_of_objects = 0;

game_object::~game_object() {
    -- _number_of_objects;
}

game_object* game_object::clone() const {
    game_object* go = new game_object(parent());
    for(int i = 0; i < _components.size(); ++i) {
        if(_components[i])
            go->_components[i].reset(_components[i]->clone(go));
    }
    return go;
}

void game_object::populate_flag() {
    assert(parent()); // need a parent
    _flag |= parent()->flag();
}

game_object* game_object::find_by_tag(char const* tag, bool recursive) const{
	for(auto *child = _first_child;
        child != null; child = child->_next_sibling)
        if(child->tag() == tag)
            return child;
    
	game_object *node = nullptr;
    if(recursive) {
        for(auto *child = _first_child;
            child != null && node == nullptr;
            child = child->_next_sibling)
            node = child->find_by_tag(tag, true);
    }
    return node;
}

game_object* game_object::child_at(int idx) const {
    game_object* child = _first_child;
    if(idx < 0)
        idx = child_size() + idx;
    for(;child != null && idx > 0; -- idx)
        child = child->_next_sibling;
    return child;
}

void game_object::pre_order(uint32_t mark, iterator_t const& iter) const{
    std::stack<game_object const*> nodes;
#if 0
	nodes.push(this);
    
	game_object const* node = nodes.top();
    
	do{
		iter(*node);
        
		nodes.pop();
		if( node->next_sibling() != null )
			nodes.push(node->next_sibling());
        
		if( node->first_child() != null )
			nodes.push(node->first_child());
        
		if( nodes.empty() )
			break;
		else
			node = nodes.top();
	}while(true);
#else
    game_object const* node = this;

    do{
        if( node == null ) {
            node = nodes.top();
            nodes.pop();
            node = node->next_sibling();
        } else {
            node->_mark = mark;
            iter(*node);
            nodes.emplace(node);
            node = node->first_child();
        }
    } while(node != null || !nodes.empty());
#endif
}

#if 0
void game_object::post_order(iterator_t const& iter) const{
    std::stack<game_object const*> nodes;
	nodes.push(this);
}
#endif

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
    
    ++_child_size;
    return *this;
}

game_object* game_object::last_child() const{
	game_object* node(_first_child);
	for(;node != null && node->next_sibling() != null ; node = node->next_sibling() )
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
	
    --_parent->_child_size;
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
