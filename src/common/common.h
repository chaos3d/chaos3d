#ifndef _CHAOS_COMMON_H
#define _CHAOS_COMMON_H

#define _CHAOS_BEGIN
#define _CHAOS_END

#if defined(WIN32)

#	define CHAOS_PRIVATE
#	if !defined(STATIC_LIB)
#	define	CHAOS_API
#	else
#		if defined(DLL_EXPORT)
#		define	CHAOS_API	__declspec(dllexport)
#		else
#		define	CHAOS_API	__declspec(dllimport)
#		endif	// DLL_EXPORT
#	endif	// STATIC_LIB

// WIN32
#elif defined(__GNUC__)

#	define	CHAOS_API		__attribute__((visibility("default")))
#	define	CHAOS_PRIVATE	__attribute__((visibility("hidden")))

#endif	// __GNU_C__

#if __APPLE__
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE == 1
#define PLATFORM_IOS 1
#endif

#if TARGET_IPHONE_SIMULATOR == 1
#define PLATFORM_IOS_SIM 1
#endif

#if defined(DEBUG) && !defined(NDEBUG)
#define ASSERT(x)
#else
#define ASSERT(x)
#endif

#include <cstdint>
#include <cstdlib>
#include <cstddef>

#endif
