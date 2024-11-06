#ifndef solosnake_html_hpp
#define solosnake_html_hpp

#include <string>

namespace solosnake
{
    //! Returns the html escape code for the first character in the string,
    //! else returns the string.
    //! htmlescape( "hello" );    // returns "hello"
    //! htmlescape( "&&&" );      // returns "&amp;"
    const char* htmlescape( const char* );

    //! Returns the html escape code for the first character in the string,
    //! else returns the string.
    //! htmlescape( "hello" ); // returns "hello" as a copy of the original string.
    //! htmlescape( "&&&" );   // returns "&amp;"
    std::string htmlescape( const std::string& );
}

#endif
