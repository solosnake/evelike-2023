#ifndef solosnake_includewindows_hpp
#define solosnake_includewindows_hpp

// Note on Visual Studio version numbers:
// VS 2005 == VC8 == _MSC_VER 1400
// VS 2008 == VC9 == _MSC_VER 1500
// VS 2010 == VC10 == _MSC_VER 1600
// VS 2012 == VC11 == _MSC_VER 1700

#ifdef WIN32

#ifndef SS_OS_WINDOWS
#pragma message(__FILE__ " SS_OS_WINDOWS not defined but WIN32 defined.")
#error "SS_OS_WINDOWS not defined but WIN32 defined." 
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501
#endif

#ifndef UNICODE
#define UNICODE 1
#endif

#ifdef UNICODE
#define _UNICODE 1
#endif

#pragma warning(push, 3)
#include <Windows.h>
#include <Shellapi.h>
#include <tchar.h>
#include <windowsx.h>
#pragma warning(pop)

#else
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__) ") : Warning Msg: "
#pragma message(__LOC__ "WIN32 must be defined as platform to include <Windows.h>")
#undef __STR2__
#undef __STR1__
#undef __LOC__
#endif
#endif
