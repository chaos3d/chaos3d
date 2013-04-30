#include "scene2d-bind.h"

#include <luabind/luabind.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/raw_policy.hpp>

#include "scene2d/Scene2DNode.h"
#include "scene2d/Transform.h"
#include "scene2d/NodeColor.h"
#include "scene2d/NodeUI.h"

#include "scene2d/CameraNode.h"

using namespace luabind;

static Scene2DNode* create_node(luabind::object const& o);
static Scene2DNode& ext_add_children(Scene2DNode&, luabind::object const&);
//static luabind::object ext_all_children(Scene2DNode&, luabind::object&);

/**
 * this is a helper function that creates the components for
 * the scene node so that the extended classes can use to init
 * the base class while paassing the newly created scene node
 * note that the side effect of this function will re-create 
 * the components if they are already created, and this might
 * be what you want as well
 */
static Scene2DNode* create_components(Scene2DNode* node, luabind::object const&);

static void bind_scene2d(lua_State *L){
    module(L)[
              namespace_("scene2d")
              [
               def("create", &create_node)
               ]

              ,class_<ReferencedCount>("RefCount") // this is temprary solution until adding new luabind policies
              .def("retain", &ReferencedCount::retain)
              .def("release", &ReferencedCount::release)
              
              ,class_<Scene2DNode, ReferencedCount>("Scene2DNode")
              .scope[
              def("createComponents", &create_components)
              ]
              .def("addChildren", &ext_add_children, return_reference_to(_1))
              .def("removeAll", &Scene2DNode::removeAllChildren)
             // .def("children", &ext_all_children)
              //IMPLEMENT_FUNC(removeChildren, &Scene2DNode::removeChildren ) TODO
              
              .def("childByTag",&Scene2DNode::childByTag)
              .def("childByIndex",&Scene2DNode::childByIndex)
              .def("moveUp", &Scene2DNode::moveUpward, return_reference_to(_1))
              .def("moveDown", &Scene2DNode::moveDownward, return_reference_to(_1))
              .def("moveTop", &Scene2DNode::moveTop, return_reference_to(_1))
              .def("moveBottom", &Scene2DNode::moveBottom, return_reference_to(_1))
              //.def("moveAfter", &Scene2DNode::moveAfterward, return_reference_to(_1))
              .def("relocateTo", &Scene2DNode::relocateTo )
              .def("addChild", &Scene2DNode::addChild, return_reference_to(_1))
              .def("removeSelf", &Scene2DNode::removeSelf )
              //IMPLEMENT_FUNC(removeWhen, &Scene2DNode::removeWhenDone ) TODO: add poller/callback?
              .def("removeAll", &Scene2DNode::removeAllChildren )              
              
              .property("tag", &Scene2DNode::getTag)
              .property("transform", &Scene2DNode::getTransform)
              .property("color", &Scene2DNode::getColor )
              //.property(sprite, 0, &Scene2DNode::getSprite )
              //.property(script, &Scene2DNode::setLua, &Scene2DNode::getLua)
              //.property("ui", &Scene2DNode::getUI)
              .property("parent", &Scene2DNode::getParent)
              //.property("children", &Scene2DNode::getChildren)    // TODO: iterator or vector?
              
              ,class_<Transform>("Transform")
              //.def("", )
              
              ,class_<CameraNode, Scene2DNode>("CameraNode")
              .def("doRender", &CameraNode::doRender)
              ];
}

static Scene2DNode* create_node(luabind::object const& o){
    assert(type(o) == LUA_TTABLE);
    
    Scene2DNode* node = NULL;
    char const* node_tag = (type(o[1]) == LUA_TNIL) ? "" : object_cast<char const*>(o[1]);

    // this creats internal scene node, for those
    // who extends the scene node, create_components
    // can be used to init the components by passing
    // the new scene node
    if(type(o["type"]) == LUA_TSTRING){
        char const* node_type = object_cast<char const*>(o["type"]);
        if(strcmp(node_type, "camera") == 0){
            node = new CameraNode(node_tag);
        }
    }
    if(node == NULL)// default type is Scene2DNode
        node = new Scene2DNode(node_tag);
    
    return create_components(node, o);
}

static Scene2DNode* create_components(Scene2DNode* node, luabind::object const& o){
    assert(o.is_valid());
    assert(node != NULL);
    assert(type(o) == LUA_TTABLE);

    //// ----------------
    /// children = { node1, node2, ... }
    object const& children = o["children"];
    int value_type = type(children);
    if(value_type == LUA_TTABLE){
        ext_add_children(*node, children);
    }
    
    //// ----------------
    /// transform = {translate = {}, scale = {}, rotate = {}}
    object const& transform = o["transform"];
    value_type = type(transform);
    if(value_type == LUA_TTABLE){
        Transform *comp_transform = new Transform(node);
        node->setTransform(comp_transform);
        for(iterator it(transform), end; it != end; ++it){
            char const* key = object_cast<char const*>(it.key());
            if( strcmp( key, "translate") == 0 ){
                comp_transform->setTranslate(Wm4::Vector3f(object_cast<float>((*it)[1]),
                                                           object_cast<float>((*it)[2]),
                                                           object_cast<float>((*it)[3])));
            }
            
            else if( strcmp( key, "rotate") == 0 ){
                comp_transform->setRotate(object_cast<float>((*it)[1]),
                                          object_cast<float>((*it)[2]),
                                          object_cast<float>((*it)[3]));
            }
            
            else if( strcmp( key, "scale") == 0 ){
                comp_transform->setScale(Wm4::Vector3f(object_cast<float>((*it)[1]),
                                                       object_cast<float>((*it)[2]),
                                                       object_cast<float>((*it)[3])));
            }
        }
    }else if(value_type == LUA_TBOOLEAN && (object_cast<bool>(transform) == true)){
        node->setTransform(new Transform(node));
    }

#if 0
	lua_State* L(lua.getL());
	int n = lua_gettop( L );
	lua.get(L);
	ASSERT( lua_istable(L,-1) );
    
	lua_pushstring(L, "children" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		addChildren( TypeLua(L));
    
	lua_settop(L, n+1);
	lua_pushstring(L, "transform");
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setTransform( new Transform(this, TypeLua(L)) );
	else if( !lua_isboolean(L,-1) || lua_toboolean(L,-1) != 0 ){
		setTransform( new Transform(this) );
	}
	
	lua_settop(L, n+1);
	lua_pushstring(L, "sprite" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setSprite( Sprite::createSprite(this, TypeLua(L)) );
    
	lua_settop(L, n+1);
	lua_pushstring(L, "color" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setColor( new NodeColor(this, TypeLua(L)) );
    
	lua_settop(L, n+1);
	lua_pushstring(L, "script" );
	lua_rawget(L, n+1);
	if( lua_isfunction(L, -1) )
		mLua = TypeLua(lua_ref(L,1));
    
	lua_settop(L, n+1);
	lua_pushstring(L, "ui" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setUI( NodeUI::createUI(this, TypeLua(L)) );
    
	lua_settop(L, n+1);
	lua_pushstring(L, "frame" );
	lua_rawget(L, n+1);
	if( lua_istable(L, -1) )
		setFrame( new NodeFrame(this, TypeLua(L)) );
	else if( lua_isboolean(L,-1) ){
		if( lua_toboolean(L,-1) ){
			if( mSprite != 0 ){
				NodeFrame *frame( new NodeFrame(this) );
				frame->setFrameFromeSprite();
				setFrame( frame );
			}else{
				LOG("Unable to define default frame. (no sprite found)");
			}
		}
	}else if( mFrame == 0 ){
		if( mUI != 0 && mSprite != 0 ){
			NodeFrame *frame( new NodeFrame(this) );
			frame->setFrameFromeSprite();
			setFrame( frame );
		}else
			setFrame( new NodeFrameGlobal(this) );
	}
    
	if( mUI != 0 && mFrame != 0 && mTransform == 0 )
		setTransform( new Transform(this) );
	else if( mSprite != 0 && mTransform == 0 )
		setTransform( new Transform(this) );
	
	lua_settop(L,n);
#endif
    return node;
}

static Scene2DNode& ext_add_children(Scene2DNode& thiz, luabind::object const& children){
    if(type(children) != LUA_TTABLE){
        // TODO : LOG
    }else{
        for(int i=1;;++i){
            if(type(children[i]) == LUA_TNIL)
                break;
            
            // TODO: child order? add them all in once?
            Scene2DNode* node = object_cast<Scene2DNode*>(children[i]);
            thiz.addChild(node);
        }
    }
    return thiz;
}

static luabind::object ext_all_children(Scene2DNode* thiz, luabind::object& o){
    int idx = -1;
    for(Scene2DNode* cur = thiz->firstChild(); cur != NULL; cur = cur->nextSibling()){
        o[++idx] = cur;
    }
    return o;
}

extern "C" int luaopen_scene2d(lua_State* L){
    luabind::open(L);
    
    bind_scene2d(L);

    luaL_Reg entris[] = {
        //{"resume", coresume},
        {NULL, NULL},
//        {"create", create_node}     //should introduce offline factory constructor
    };
    
    luaL_register(L, "scene2d", entris);
	return 0;
}
