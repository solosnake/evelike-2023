#include <cassert>
#include <cstdarg>
#include <sstream>
#include "solosnake/iluawidget_make_function.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/external/lua.hpp"
#include "solosnake/iwidgetname.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    std::string make_function( lua_State* const L,
                               const iwidgetname& name,
                               const char* xmlHandleName,
                               const TiXmlElement& xmlWidget,
                               const unsigned int nArgs,
                               ... )
    {
        assert( xmlHandleName );
        assert( L );

        std::string functionName;

        const TiXmlElement* handlerElement = xmlWidget.FirstChildElement( xmlHandleName );

        if( handlerElement && handlerElement->GetText() )
        {
            std::ostringstream os;
            os << name.str() << '_' << xmlHandleName;
            functionName = os.str();

            // Reset the string stream and re-use it.
            os.str( "" );

            // Open function signature.
            os << "function " << functionName << " (";

            va_list marker;
            va_start( marker, nArgs );
            for( unsigned int i = 0; i < nArgs; ++i )
            {
                os << va_arg( marker, const char* );
                // If another arg follows this one, write
                // comma separator.
                if( i + 1 < nArgs )
                {
                    os << ',';
                }
            }
            va_end( marker );

            // Close function signature.
            os << ")\n" << handlerElement->GetText() << "\nend\n";

            // Add new function to lua.
            const std::string functionBody = os.str();

            auto loaded = luaL_loadbuffer( L, functionBody.c_str(), functionBody.length(), functionName.c_str() );

            if( 0 == loaded )
            {
                auto called = lua_pcall( L, 0, LUA_MULTRET, 0 );

                if( 0 != called )
                {

                    // Something went wrong.
                    ss_err( "Error loading widget handler '",
                            xmlHandleName,
                            "' into Lua '",
                            name.str(),
                            "' for widget '",
                            lua_tostring( L, -1 ) );

                    lua_pop( L, 1 ); // remove error message

                    ss_throw( "Error loading widget handler into Lua." );
                }
            }
            else
            {
                auto err = lua_tostring( L, -1 );

                // Something went wrong.
                ss_err( "Error creating widget handler function named '",
                        functionName,
                        "' with function body:\n'",
                        functionBody,
                        "',\nERROR: ",
                        err );

                lua_pop( L, 1 ); // remove error message

                ss_throw( "Error creating widget handler in Lua." );
            }
        }

        return functionName;
    }
}
