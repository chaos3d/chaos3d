#include "2d/DrawingUtil.h"
#include "core/RTTI.h"
#include "script/bind.h"

//using namespace chaos;

IMPLEMENT_CLASS(DrawingUtil, LIB_DOMAIN)
IMPLEMENT_FUNC(drawText, &DrawingUtil::drawText)
IMPLEMENT_END;

#if defined(WIN32) || defined(ANDROID)

void DrawingUtil::drawText(chaos::Texture*, char const*, TypeLua const& settings){
}

#endif
