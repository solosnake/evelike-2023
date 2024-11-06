#ifdef WIN32
#include "solosnake/external/openal.hpp"
#pragma comment(lib, "opengl32.lib") // link with Microsoft OpenGL lib
#pragma comment(lib, "glu32.lib")    // link with Microsoft OpenGL Utility lib
#pragma comment(lib, "OpenAL32.lib") // OpenAL lib
#pragma comment(lib, "Shlwapi.lib")  // Required for 'PathAppendW'  etc in Windows. 
#else
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__) ") : Warning Msg: "
#pragma message(__LOC__ "Use of link file for non WIN32 platform.")
#undef __STR2__
#undef __STR1__
#undef __LOC__
#endif
