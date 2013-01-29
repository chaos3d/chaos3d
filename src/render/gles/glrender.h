#include "GLRender2DManager.h"
#include "GLTexture.h"
#include "GLRenderTexture.h"
#include "GLVertexBuffer.h"
#include "GLVertexIndexBuffer.h"

#if defined(WIN32)
#	include "GLRenderWindow.h"
#	include "GLRenderTextureES1.h"
#elif defined(CHAOS_ANDROID)
#	include "GLRenderWindow-android.h"
#elif defined(CHAOS_OSX)
//obj-c
//#	include "GLRenderWindow-ios.h"
#endif
