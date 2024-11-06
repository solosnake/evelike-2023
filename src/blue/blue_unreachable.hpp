#ifndef solosnake_unreachable_hpp
#define solosnake_unreachable_hpp

// ss_unreachable

#ifdef __cpp_lib_unreachable
#   include <utility>
#   define ss_unreachable   std::unreachable()
#else
#   ifdef _MSC_VER
#       ifndef ss_unreachable
            // See http://msdn.microsoft.com/en-us/library/1b3fsfxw%28v=vs.100%29.aspx
            // Use __assume(0) to indicate a code path that cannot be reached.
#           ifdef NDEBUG
#               define ss_unreachable __assume(0)
#           else
#               include <cassert>
#               define ss_unreachable assert(!"Unreachable code encountered!"); throw("Unreachable reached");
#           endif
#       endif
#   elif defined __GNUC__
        // See
        // http://gcc.gnu.org/onlinedocs/gcc-4.5.0/gcc/Other-Builtins.html#Other-Builtins
#       define ss_unreachable __builtin_unreachable()
#   else
#       define ss_unreachable
#   endif
#endif

#endif
