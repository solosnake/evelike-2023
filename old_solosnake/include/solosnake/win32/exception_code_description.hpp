#ifndef SOLOSNAKE_EXCEPTIONCODE_DESCRIPTION_HPP
#define SOLOSNAKE_EXCEPTIONCODE_DESCRIPTION_HPP

namespace solosnake 
{
    //! Returns a static string describing in English the likely cause of a Windows Exception Code. 
    //! See http://msdn.microsoft.com/en-us/library/windows/desktop/ms679356%28v=vs.85%29.aspx 
    //! If the exception code is not recognized, then this returns the string "Unknown.".
    //! Otherwise the format of the returned text is "<error code name>: <descriptive text>."
    const char* get_exceptioncode_description( const unsigned int );
}

#endif
