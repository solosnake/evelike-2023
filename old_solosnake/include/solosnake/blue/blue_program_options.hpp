#ifndef blue_program_options_hpp
#define blue_program_options_hpp

#include <string>

namespace blue
{
    //! Class responsible for parsing the command line and checking to see
    //! if the user has specified the settings file and the optional save
    //! directory and game data dirs string. If these are not specified, and
    //! depending on the platform, it will attempt to fill these in.
    class blue_program_options
    {
    public:

        blue_program_options( const int argc, char const *const *const argv );

        const std::string& settings_file() const;

        const std::string& user_save_dir() const;

        const std::string& game_data_dirs() const;

        unsigned int logging_level() const;

        //! Returns true if the logging should be directed to cout.
        bool log_to_cout() const;

        //! Returns true if there were no options such as version etc
        //! specified.
        bool run_blue() const { return run_; }

        bool show_help() const { return show_help_; }

        bool show_version() const { return show_version_; }

        const std::string& version() const { return version_; }

        const std::string& description() const { return desc_; }

    private:

        std::string     settingsFile_;
        std::string     saveDir_;
        std::string     dataDirs_;
        std::string     desc_;
        std::string     version_;
        unsigned int    loggingLevel_;
        bool            log_to_cout_;
        bool            run_;
        bool            show_help_;
        bool            show_version_;
    };
}
#endif
