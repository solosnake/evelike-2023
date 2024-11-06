#ifndef solosnake_case_insensitive_compare_hpp
#define solosnake_case_insensitive_compare_hpp

namespace solosnake
{
    //! Returns true if the contents of the null terminated ASCII strings lhs
    //! and rhs are the same, ignoring the case of their contents. This 
    //! comparison will use std::tolower.
    //! If both pointers are null, returns true.
    //! @note This call locally creates a locale per call.
    bool case_insensitive_compare( const char * const lhs, const char* const rhs );
}

#endif
