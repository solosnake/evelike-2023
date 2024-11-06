#include "solosnake/persistance.hpp"
#include "solosnake/languagetexts.hpp"
#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/manifest.hpp"
#include "solosnake/external/lua.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_luagameoptions.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_translator.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    namespace
    {
        int lua_set_host_ip( lua_State* L )
        {
            // Get upvalue (the luagameoptions*)
            auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isstring( L, 1 ) )
            {
                p->set_host_ip( luaL_checkstring( L, 1 ) );
            }

            return 0;
        }

        int lua_set_game_file( lua_State* L )
        {
            // Get upvalue (the luagameoptions*)
            auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isstring( L, 1 ) )
            {
                p->set_game_file( luaL_checkstring( L, 1 ) );
            }

            return 0;
        }

        int lua_set_host_port( lua_State* L )
        {
            // Get upvalue (the luagameoptions*)
            auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isnumber( L, 1 ) )
            {
                p->set_host_port( static_cast<unsigned short>( luaL_checknumber( L, 1 ) ) );
            }

            return 0;
        }

        int lua_set_num_players( lua_State* L )
        {
            // Get upvalue (the luagameoptions*)
            auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isnumber( L, 1 ) )
            {
                p->set_num_players( static_cast<unsigned int>( luaL_checknumber( L, 1 ) ) );
            }

            return 0;
        }

        int lua_set_language_filename( lua_State* L )
        {
            // Get upvalue (the luagameoptions*)
            auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isstring( L, 1 ) )
            {
                p->set_language_filename( luaL_checkstring( L, 1 ) );
            }

            return 0;
        }

        int lua_set_boardfilename( lua_State* L )
        {
            // Get upvalue (the luagameoptions*)
            auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isstring( L, 1 ) )
            {
                p->set_boardfilename( luaL_checkstring( L, 1 ) );
            }

            return 0;
        }

        int lua_set_fullscreen( lua_State* L )
        {
            // Get upvalue (the luagameoptions*)
            auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isboolean( L, 1 ) )
            {
                p->set_fullscreen( lua_toboolean( L, 1 ) != 0 );
            }

            return 0;
        }

        int lua_get_num_players( lua_State* L )
        {
            auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            lua_pushnumber( L, static_cast<lua_Number>( p->get_num_players() ) );
            return 1;
        }

        int lua_get_host_port( lua_State* L )
        {
            auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            lua_pushnumber( L, p->get_host_port() );
            return 1;
        }

        int lua_get_game_file( lua_State* L )
        {
            auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            lua_pushstring( L, p->get_game_file().c_str() );
            return 1;
        }

        int lua_get_host_ip( lua_State* L )
        {
            auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            lua_pushstring( L, p->get_host_ip().c_str() );
            return 1;
        }

        int lua_get_language_filename( lua_State* L )
        {
            auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            lua_pushstring( L, p->get_language_filename().c_str() );
            return 1;
        }

        //int lua_get_current_profile_name( lua_State* L )
        //{
        //    auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
        //    lua_pushstring( L, p->get_current_profile_name().c_str() );
        //    return 1;
        //}

        //int lua_get_indexed_profile_name( lua_State* L )
        //{
        //    auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

        //    auto n = luaL_checknumber( L, 1 );

        //    lua_pushstring( L, p->get_profile_name(n).c_str() );

        //    return 1;
        //}

        //int lua_get_profiles_count( lua_State* L )
        //{
        //    auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
        //    lua_pushnumber( L, p->get_profiles_count() );
        //    return 1;
        //}
        
        //int lua_set_as_current_profile( lua_State* L )
        //{
        //    // Get upvalue (the luagameoptions*)
        //    auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

        //    if( lua_isnumber( L, 1 ) )
        //    {
        //        p->set_as_current_profile( luaL_checknumber( L, 1 ) );
        //    }

        //    return 0;
        //}        
        
        //int lua_create_new_profile_named( lua_State* L )
        //{
        //    // Get upvalue (the luagameoptions*)
        //    auto p = reinterpret_cast<luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

        //    if( lua_isstring( L, 1 ) )
        //    {
        //        p->create_new_profile_named( luaL_checkstring( L, 1 ) );
        //    }

        //    return 0;
        //} 

        int lua_get_boardfilename( lua_State* L )
        {
            auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            lua_pushstring( L, p->get_boardfilename().c_str() );
            return 1;
        }

        int lua_is_fullscreen( lua_State* L )
        {
            auto p = reinterpret_cast<const luagameoptions*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            lua_pushboolean( L, p->is_fullscreen() ? 1 : 0 );
            return 1;
        }

        void setup_option_boardfilename( lua_State* const L, luagameoptions* const options )
        {
            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_get_boardfilename, 1 );
            lua_setglobal( L, "get_boardfilename" );

            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_set_boardfilename, 1 );
            lua_setglobal( L, "set_boardfilename" );
        }

        void setup_option_host( lua_State* const L, luagameoptions* const options )
        {
            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_get_host_ip, 1 );
            lua_setglobal( L, "get_host_ip" );

            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_set_host_ip, 1 );
            lua_setglobal( L, "set_host_ip" );
        }

        void setup_option_port( lua_State* const L, luagameoptions* const options )
        {
            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_get_host_port, 1 );
            lua_setglobal( L, "get_host_port" );

            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_set_host_port, 1 );
            lua_setglobal( L, "set_host_port" );
        }

        void setup_option_num_players( lua_State* const L, luagameoptions* const options )
        {
            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_set_num_players, 1 );
            lua_setglobal( L, "set_num_players" );

            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_get_num_players, 1 );
            lua_setglobal( L, "get_num_players" );
        }

        void setup_option_game_file( lua_State* const L, luagameoptions* const options )
        {
            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_set_game_file, 1 );
            lua_setglobal( L, "set_game_file" );

            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_get_game_file, 1 );
            lua_setglobal( L, "get_game_file" );
        }

        //void setup_option_profiles( lua_State* const L, luagameoptions* const options )
        //{
        //    lua_pushlightuserdata( L, options );
        //    lua_pushcclosure( L, &lua_get_current_profile_name, 1 );
        //    lua_setglobal( L, "get_current_profile_name" );

        //    lua_pushlightuserdata( L, options );
        //    lua_pushcclosure( L, &lua_get_indexed_profile_name, 1 );
        //    lua_setglobal( L, "get_profile_name" );

        //    lua_pushlightuserdata( L, options );
        //    lua_pushcclosure( L, &lua_get_profiles_count, 1 );
        //    lua_setglobal( L, "get_profiles_count" );

        //    lua_pushlightuserdata( L, options );
        //    lua_pushcclosure( L, &lua_set_as_current_profile, 1 );
        //    lua_setglobal( L, "set_as_current_profile" );

        //    lua_pushlightuserdata( L, options );
        //    lua_pushcclosure( L, &lua_create_new_profile_named, 1 );
        //    lua_setglobal( L, "create_new_profile_named" );
        //}


        void setup_option_language_filename( lua_State* const L, luagameoptions* const options )
        {
            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_set_language_filename, 1 );
            lua_setglobal( L, "set_language_filename" );

            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_get_language_filename, 1 );
            lua_setglobal( L, "get_language_filename" );
        }

        void setup_option_fullscreen( lua_State* const L, luagameoptions* const options )
        {
            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_set_fullscreen, 1 );
            lua_setglobal( L, "set_fullscreen" );

            lua_pushlightuserdata( L, options );
            lua_pushcclosure( L, &lua_is_fullscreen, 1 );
            lua_setglobal( L, "is_fullscreen" );
        }
    }


    shared_ptr<luagameoptions>
    luagameoptions::make_gameoptions( const shared_ptr<luaguicommandexecutor>& luaInGui,
                                      const shared_ptr<persistance>& data,
                                      const shared_ptr<translator>& t,
                                      std::shared_ptr<ioswindow> w )
    {
        auto options = make_shared<luagameoptions>( luaInGui, data, t, w );
        auto L = luaInGui->lua();

        setup_option_host( L, options.get() );
        setup_option_port( L, options.get() );
        setup_option_num_players( L, options.get() );
        setup_option_game_file( L, options.get() );
        setup_option_language_filename( L, options.get() );
        //setup_option_profiles( L, options.get() );
        setup_option_fullscreen( L, options.get() );

        return options;
    }

    luagameoptions::luagameoptions( const shared_ptr<luaguicommandexecutor>& luaInGui,
                                    const shared_ptr<persistance>& data,
                                    const shared_ptr<translator>& t,
                                    shared_ptr<ioswindow> w )
        : lua_( luaInGui )
        , persistance_( data )
        , translator_( t )
        , oswindow_( w )
    {
        assert( luaInGui );
        assert( data );
        assert( t );
        assert( w );
    }

    luagameoptions::~luagameoptions()
    {
        ss_dbg( "luagameoptions dtor." );
    }

    const string& luagameoptions::get_host_ip() const
    {
        return ( *persistance_ )[BLUE_NUM_PLAYERS].as_string();
    }

    const string& luagameoptions::get_game_file() const
    {
        return ( *persistance_ )[BLUE_GAME_FILE].as_string();
    }

    unsigned short luagameoptions::get_host_port() const
    {
        return static_cast<unsigned short>( ( *persistance_ )[BLUE_NUM_PLAYERS].as_uint() );
    }

    unsigned int luagameoptions::get_num_players() const
    {
        return ( *persistance_ )[BLUE_NUM_PLAYERS].as_uint();
    }

    //! Makes a non judgmental use of the ip string - just stores it in
    //! the user settings.
    void luagameoptions::set_host_ip( const char* ip )
    {
        persistance_->set( BLUE_HOST_IP, ip, BLUE_HOST_IP_COMMENT );
    }

    //! Makes a non judgmental use of the game string - just stores it in
    //! the user settings.
    void luagameoptions::set_game_file( const char* ip )
    {
        persistance_->set( BLUE_GAME_FILE, ip, BLUE_GAME_FILE_COMMENT );
    }

    //! Makes a non judgmental use of the port - just stores it in
    //! the user settings.
    void luagameoptions::set_host_port( unsigned short port )
    {
        persistance_->set( BLUE_HOST_PORT,
                           to_string( static_cast<unsigned long long>( port ) ),
                           BLUE_HOST_PORT_COMMENT );
    }

    //! Makes a non judgmental use of the number of players - just stores it in
    //! the user settings.
    void luagameoptions::set_num_players( unsigned int n )
    {
        persistance_->set( BLUE_NUM_PLAYERS,
                           to_string( static_cast<unsigned long long>( n ) ),
                           BLUE_NUM_PLAYERS_COMMENT );
    }

    bool luagameoptions::is_fullscreen() const
    {
        assert( ( *persistance_ )[UO_FULLSCREEN].is_true() == oswindow_.lock()->is_fullscreen() );

        // Throws std::bad_weak_ptr if oswindow_ expired.
        return shared_ptr<ioswindow>( oswindow_ )->is_fullscreen();
    }

    void luagameoptions::set_fullscreen( const bool fs )
    {
        ss_dbg( "luagameoptions::set_fullscreen( ", fs ? "true )" : "false )" );

        // Throws std::bad_weak_ptr if oswindow_ expired.
        shared_ptr<ioswindow> w( oswindow_ );
        w->set_as_fullscreen( fs );
        persistance_->set( UO_FULLSCREEN,
                           w->is_fullscreen() ? "true" : "false",
                           UO_FULLSCREEN_COMMENT );
    }

    //! Makes a non judgmental use of the board file name - just stores it in
    //! the user settings.
    void luagameoptions::set_boardfilename( const char* boardfilename )
    {
        persistance_->set( BLUE_GAME_BOARDFILENAME, boardfilename, BLUE_GAME_BOARDFILENAME_COMMENT );
    }

    string luagameoptions::get_boardfilename() const
    {
        return persistance_->value( BLUE_GAME_BOARDFILENAME ).as_string();
    }

    void luagameoptions::set_language_filename( const char* langfilename )
    {
        translator_->change_language( langfilename );
    }

    string luagameoptions::get_language_filename() const
    {
        return translator_->current_language_filename();
    }

    //string luagameoptions::get_current_profile_name() const
    //{
    //    return persistance_->value( BLUE_GAME_PROFILE_NAME ).as_string();
    //}
    //   
    //std::string luagameoptions::get_profile_name( const unsigned int n ) const
    //{
    //}

    //unsigned int luagameoptions::get_profiles_count() const
    //{
    //}

    //void luagameoptions::set_as_current_profile( const unsigned int n )
    //{
    //}

    //void luagameoptions::create_new_profile_named( const char* )
    //{
    //    //persistance_->value( BLUE_GAME_PROFILE_NAME ).as_string();
    //}

    //string luagameoptions::set_profile_name( const char* profilename )
    //{
    //    return persistance_->value( BLUE_GAME_PROFILE_NAME ).as_string();
    //}
}