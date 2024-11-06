#ifndef solosnake_glew_hpp
#define solosnake_glew_hpp

#ifdef WIN32
#include "solosnake/win32/includewindows.hpp"
#endif

// We want always to use GLEW in a static context.
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>

#ifdef WIN32
#include <GL/wglew.h>
#endif

#endif
