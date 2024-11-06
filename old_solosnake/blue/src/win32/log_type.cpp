#include "solosnake/logging.hpp"
#include "solosnake/win32/includewindows.hpp"

#include <chrono>
#include <regex>

namespace solosnake
{
#ifndef SS_COMPILE_WITHOUT_LOGGING

    void log_type( unsigned int lvl ) SS_NOEXCEPT
    {
        static const WORD colours[14] =    
        { 
          FOREGROUND_RED,                                                       // 0 Red
          FOREGROUND_GREEN,                                                     // 1 Green
          FOREGROUND_BLUE,                                                      // 2 Blue
          FOREGROUND_RED | FOREGROUND_GREEN,                                    // 3 Yellow
          FOREGROUND_RED | FOREGROUND_BLUE,                                     // 4 Purple/pink
          FOREGROUND_GREEN | FOREGROUND_BLUE,                                   // 5 Cyan
          FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,                  // 6 White
          FOREGROUND_INTENSITY | FOREGROUND_RED,                                // 7 Bright Red
          FOREGROUND_INTENSITY | FOREGROUND_GREEN,                              // 8 Bright Green
          FOREGROUND_INTENSITY | FOREGROUND_BLUE,                               // 9 Bright Blue
          FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,             // 10 Bright Yellow
          FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,              // 11 Bright Pink
          FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,            // 12 Bright Cyan
          FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        };

        // Clamp to <= 13.
        lvl = lvl > 13 ? 13 : lvl;

        HANDLE h = 0 == lvl ? ::GetStdHandle( STD_ERROR_HANDLE ) : ::GetStdHandle( STD_OUTPUT_HANDLE );

        ::SetConsoleTextAttribute( h, colours[lvl] );
    }

#endif SS_COMPILE_WITHOUT_LOGGING
}
