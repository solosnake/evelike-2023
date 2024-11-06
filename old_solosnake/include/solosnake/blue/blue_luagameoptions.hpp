#ifndef blue_gameoptions_hpp
#define blue_gameoptions_hpp

#include <memory>
#include <string>

namespace solosnake
{
    class ioswindow;
    class persistance;
    class luaguicommandexecutor;
}

namespace blue
{
    class translator;

    //! Exposes an API to Lua for setting up game options. Stores its settings into
    //! the persistance object.
    class luagameoptions : public std::enable_shared_from_this<luagameoptions>
    {
    public:

        ~luagameoptions();

        luagameoptions(
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::persistance>&,
            const std::shared_ptr<translator>&,
            std::shared_ptr<solosnake::ioswindow>);

        static std::shared_ptr<luagameoptions> make_gameoptions(
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::persistance>&,
            const std::shared_ptr<translator>&,
            std::shared_ptr<solosnake::ioswindow> );

        const std::string& get_host_ip() const;

        const std::string& get_game_file() const;

        unsigned short get_host_port() const;

        unsigned int get_num_players() const;

        void set_game_file( const char* );

        void set_host_ip( const char* );

        void set_host_port( unsigned short );

        void set_num_players( unsigned int );

        void set_language_filename( const char* langfilename );

        void set_boardfilename( const char* boardfilename );

        std::string get_language_filename() const;

        std::string get_boardfilename() const;

        //std::string get_current_profile_name() const;
        
        //std::string get_profile_name( const unsigned int n ) const;

        //unsigned int get_profiles_count() const;

        //void set_as_current_profile( const unsigned int n );

        //void create_new_profile_named( const char* );

        bool is_fullscreen() const;

        void set_fullscreen( const bool );

    private:

        std::shared_ptr<solosnake::luaguicommandexecutor>   lua_;
        std::shared_ptr<solosnake::persistance>             persistance_;
        std::shared_ptr<translator>                         translator_;
        std::weak_ptr<solosnake::ioswindow>                 oswindow_;
    };
}

#endif
