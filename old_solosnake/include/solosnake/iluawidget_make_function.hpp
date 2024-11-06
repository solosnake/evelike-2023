#ifndef solosnake_iluawidget_make_function_hpp
#define solosnake_iluawidget_make_function_hpp

#include <string>

class TiXmlElement;
struct lua_State;

namespace solosnake
{
    class iwidgetname;

    //! Quite a complicate method, this reads the contents of the text in the xml element
    //! named xmlHandleName, which should be Lua code, and builds a Lua function from this
    //! code., The name of the function should be unique in the Lua context, and it is
    //! formed by combining the the widget name and handler name into a unique function
    //! name in Lua.
    //! This method is intended to allows the user to define new functions for a widget in
    //! xml. The widget should store the returned string, which is the unique name of the
    //! function, and when its own C++ version of that function is called it can check to see
    //! did the Xml definition of the luawidget define a handler for the C++ function, and if
    //! it did, call it.
    //!
    //! If the TiXmlElement does not contain any sub element named xmlHandleName then this is not
    //! an error, and no function is generated, and the returned string is empty.
    //!
    //! @param L                L must be a valid lua_State.
    //! @param xmlHandleName    xmlHandleName must be a pointer to a null terminated string.
    //! @param nArgs            The number of optional args the generated method will take.
    //! @param ...              The names of the args, passed as constant C strings. See example.
    //!
    //! Pass to this the arguments as const char*, eg.
    //!
    //! @code
    //! make_function(
    //!    L, "buttonA", "on_create", xmlElement, 2, "argA", "argB" ); // Note 2 args.
    //! @endcode
    //!
    //!
    //! This will add a function like:
    //! @code
    //! -- The names of argA and argB come from the 2 args "argA"
    //! -- and "argB" in the make_function call.
    //! function buttonA_on_create( argA, argB )
    //! <code from xmlElement text here>
    //! end
    //! @endcode
    //!
    //! Returns the name of the function as registered in Lua.
    std::string make_function( lua_State* const L,
                               const iwidgetname& name,
                               const char* xmlHandleName,
                               const TiXmlElement& xmlWidget,
                               const unsigned int nArgs,
                               ... );
}

#endif
