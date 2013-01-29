#include "2d/EventUI.h"
#include "script/bind.h"

//using namespace chaos;

DEFINE_EVENT(EventUITouchBegan, touchBegan);
DEFINE_EVENT(EventUITouchMoved, touchMoved);
DEFINE_EVENT(EventUITouchEnded, touchEnded);
DEFINE_EVENT(EventUITouchCancelled, touchCancelled);

DEFINE_EVENT(EventUIMessage, message);

IMPLEMENT_CLASS(EventUIMessage, LIB_DOMAIN)
IMPLEMENT_FUNC(from, &EventUIMessage::getNode)
IMPLEMENT_FUNC(getInt, &EventUIMessage::getParameter<int>)
IMPLEMENT_FUNC(getFloat, &EventUIMessage::getParameter<float>)
IMPLEMENT_FUNC(getBool, &EventUIMessage::getParameter<bool>)
IMPLEMENT_END
