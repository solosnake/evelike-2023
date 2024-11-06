#ifndef SOLOSNAKE_OPENAL_HPP
#define SOLOSNAKE_OPENAL_HPP

/**
* Added to get OpenAL / ALUT to compile.
*/
#ifdef ALUT_API
#undef ALUT_API
#endif
#define ALUT_API
#define ALUT_APIENTRY
#define HAVE_STDINT_H 1
#define HAVE__STAT 1

#include "../../../../external/freealut-1.1.0-src/src/config.h"
#include "AL/alut.h"

#ifndef ALUT_API_MAJOR_VERSION
#error Linking against an ALUT 0.x.x header file?
#endif

namespace solosnake
{
void alut_init();

void alut_release();
}

#endif
