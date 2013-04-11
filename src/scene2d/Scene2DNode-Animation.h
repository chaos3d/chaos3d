#ifndef _SCENE2DNODE_ANIMATIONS_H
#define _SCENE2DNODE_ANIMATIONS_H

#include "common/common.h"
#include "wm4-algebra/algebra.h"

_CHAOS_BEGIN

class AnimationState;
class Scene2DNode;

class Scene2DNodeAnimationHelper {
public:
	// Transform
	static AnimationState* translateTo(Scene2DNode*, Wm4::Vector3f const&);
	static AnimationState* translateBy(Scene2DNode*, Wm4::Vector3f const&);
	static AnimationState* rotateTo(Scene2DNode*, Wm4::Quaternionf const&);
	static AnimationState* rotateBy(Scene2DNode*, Wm4::Quaternionf const&);
	static AnimationState* scaleTo(Scene2DNode*, Wm4::Vector3f const&);
	static AnimationState* scaleBy(Scene2DNode*, Wm4::Vector3f const&);

	// Color
	static AnimationState* colorTo(Scene2DNode*, Wm4::ColorRGBA const&);
	
	// others?
};

_CHAOS_END

#endif
