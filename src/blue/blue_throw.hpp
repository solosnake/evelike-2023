#ifndef blue_throw_hpp
#define blue_throw_hpp

#include <exception>
#include <stdexcept>
#include <string_view>

//----------------------------------------------------------------------------
// When trying to write ss_throw messages, try to formulate them so that they
// make easily read and understood sense when trying to understand what went
// wrong. Have the message state, in simple terms, what just was done wrong,
// and not what should have been done, e.g. "Range was out of bounds".
//--------------------------------------------------------------------------

class ss_exception final : public std::runtime_error
{
public:
    explicit ss_exception( char const * const m ) noexcept;
};

//-----------------------------------------------------------------------------
// CONFIGURATION OPTIONS
//-----------------------------------------------------------------------------
#define SS_THROW_WRITE_CERR

/// Use this when the compiler has issues with the preprocessor.
//#define SOLOSNAKE_SIMPLE_THROWS

/// Use this when the compiler does not support throwing. This supersedes all
/// other options.
//#define SOLOSNAKE_THROWS_EXIT

#define SS_EXCEPTION_CTOR(message) ss_exception(message)
#define SS_EXCEPTION_TYPE ss_exception

//-----------------------------------------------------------------------------
// IMPLEMENTATION
//-----------------------------------------------------------------------------
#ifdef SS_THROW_WRITE_CERR
# include "solosnake/blue/blue_logging.hpp"
#endif

#ifdef SOLOSNAKE_THROWS_EXIT
# include <cstdlib>
# define ss_throw(message) std::exit( EXIT_FAILURE )
#endif

// Simple basic throw version.
#ifndef ss_throw
# ifdef SOLOSNAKE_SIMPLE_THROWS
#      define ss_throw(message) throw SS_EXCEPTION_CTOR(message)
# endif
#endif

// Microsoft Visual Studio version:
#ifndef ss_throw
# ifdef _MSC_VER
#  ifndef SOLOSNAKE_STRINGIFY
#    define SOLOSNAKE_STRINGIFY(x) #x
#    define SOLOSNAKE_TOSTRING(x) SOLOSNAKE_STRINGIFY(x)
#  endif
#  ifndef SOLOSNAKE_FILESTAMP
#    define SOLOSNAKE_FILESTAMP __FILE__ "(" SOLOSNAKE_TOSTRING(__LINE__) "): "
#  endif
#  ifndef SOLOSNAKE_THROW_MSG
#    define SOLOSNAKE_THROW_MSG(message) SOLOSNAKE_FILESTAMP __FUNCTION__ ", '" << message << "'"
#  endif
#  ifndef ss_throw
#    ifdef SS_THROW_WRITE_CERR
#    include <iostream>
#    define ss_throw(message)                                                 \
          std::cerr << "Throwing exception: " << SOLOSNAKE_THROW_MSG(message) << std::endl; \
              throw SS_EXCEPTION_CTOR(SOLOSNAKE_THROW_MSG(message))
#    else
#      define ss_throw(message) throw SS_EXCEPTION_CTOR(SOLOSNAKE_THROW_MSG(message))
#    endif
#  endif
# endif
#endif

// Generic version.
#ifndef ss_throw
# ifdef SS_THROW_WRITE_CERR
#   include <iostream>
#   ifndef SOLOSNAKE_STRINGIFY
#     define SOLOSNAKE_STRINGIFY(x) #x
#     define SOLOSNAKE_TOSTRING(x) SOLOSNAKE_STRINGIFY(x)
#   endif
#   ifndef SOLOSNAKE_FILESTAMP
#     define SOLOSNAKE_FILESTAMP __FILE__ "(" SOLOSNAKE_TOSTRING(__LINE__) "): "
#   endif
#   ifndef SOLOSNAKE_THROW_MSG
#     define SOLOSNAKE_THROW_MSG(message) SOLOSNAKE_FILESTAMP ", '" << message << "'"
#   endif
#   define ss_throw(message)                                              \
          std::cerr << "Throwing exception: " << SOLOSNAKE_THROW_MSG(message) << std::endl; \
          throw SS_EXCEPTION_CTOR(message)
# else
#   define ss_throw(message) throw SS_EXCEPTION_CTOR(message)
# endif
#endif

#endif // blue_throw_hpp
