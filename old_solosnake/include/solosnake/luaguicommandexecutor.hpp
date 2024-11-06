#ifndef solosnake_luacommandexecutor_hpp
#define solosnake_luacommandexecutor_hpp

#include <memory>
#include "solosnake/external/lua.hpp"
#include "solosnake/iscreenstate.hpp"

namespace solosnake
{
    //! Object owning the Lua state and through which the gui executes commands.
    //! Used by the Lua widgets to execute commands.
    class luaguicommandexecutor : public iscreenstate
    {
        struct HeapOnly
        {
        };

    public:

        static std::shared_ptr<luaguicommandexecutor> make_shared();

        explicit luaguicommandexecutor( const HeapOnly& );

        virtual ~luaguicommandexecutor();

        bool is_screen_ended() const override;

        nextscreen get_next_screen() const override;

        lua_State* lua();

        void set_next_screen( const std::string& );

        void end_screen();

    private:

        luaguicommandexecutor( const luaguicommandexecutor& );

        luaguicommandexecutor& operator=( const luaguicommandexecutor& );

    private:

        lua_State*  lua_;
        nextscreen  next_screen_;
        bool        screen_ended_;
    };


    ////////////////////////////// INLINES /////////////////////////////////////

    inline lua_State* luaguicommandexecutor::lua()
    {
        return lua_;
    }
}

#endif
