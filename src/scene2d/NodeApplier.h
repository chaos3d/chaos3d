/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NODEAPPLIER_H
#define	_CHAOS_NODEAPPLIER_H

#include "common/common.h"
#include "Scene2DNode.h"
#include "NodeColor.h"
#include "Transform.h"

_CHAOS_BEGIN

template <typename Componenet>
class ComponenetApplier {
public:
	ComponenetApplier(Componenet* t) : _component(t) {
		if( t != 0 )
			t->getNode()->retain();
	};
	
	ComponenetApplier& operator=(ComponenetApplier const& rhs){
		if( _component == rhs._component )
			return *this;
		
		if( _component != 0 )
			_component->getNode()->release();
            
        _component = rhs._component;
		if( _component != 0 )
			_component->getNode()->retain();
            return *this;
	};

    ~ComponenetApplier(){
        if(_component != NULL){
            assert(_component->getNode() != NULL);
            _component->getNode()->release();
        }
    }

    inline Componenet* getComponent() const { return _component; };
	//inline Componenet*&	getColor() {return _componenet; };

protected:
    Componenet* _component;
};

///--------------------------------------------------------
///--------------------------------------------------------
class NodeColor::ColorApplier : public ComponenetApplier<NodeColor>{
public:
	inline bool operator() (Wm4::ColorRGBA const& color) const{
		_component->setColor( color );
		return _component->getNode()->getParent() != 0;
	}
};

///--------------------------------------------------------
///--------------------------------------------------------
class Transform::TranslateApplier : public ComponenetApplier<Transform>{
public:
	inline bool operator() (Wm4::Vector3f const& pos) const{
		_component->setTranslate( pos );
		return _component->getNode()->getParent() != 0;
	}
};


class Transform::RotateApplier : public ComponenetApplier<Transform>{
public:
	inline bool operator() (Wm4::Quaternionf const& rot) const{
		_component->setRotate( rot );
		return _component->getNode()->getParent() != 0;
	}
};

class Transform::ScaleApplier : public ComponenetApplier<Transform>{
	inline bool operator() (Wm4::Vector3f const& scale) const{
		_component->setScale( scale );
		return _component->getNode()->getParent() != 0;
	}
};

_CHAOS_END

#endif

