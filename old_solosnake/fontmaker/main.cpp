#include "solosnake/make_font.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/image.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/fontletters.hpp"
#include "solosnake/throw.hpp"
#include <string>
#include <iostream>
#include <exception>
#include <regex>

using namespace std;

#define SOLOSNAKE_FONTMAKER_APP_TITLE       "fontmaker"
#define SOLOSNAKE_EXCEPTION_ERRCODE         (-1)
#define SOLOSNAKE_UNKNOWNEXCEPTION_ERRCODE  (-2)

namespace solosnake
{
    namespace
    {
        class missingparam_exception : public exception
        {
        public:

            missingparam_exception( const char* param ) : missing_param_( param )
            {
            }

            virtual ~missingparam_exception() throw()
            {
            }

            virtual const char* what() const throw()
            {
                return missing_param_.c_str();
            }

        private:
            string missing_param_;
        };

        template <typename T>
        T get_option_param( const char* name, boost::program_options::variables_map& vm )
        {
            if( vm.count( name ) )
            {
                return vm[name].as<T>();
            }
            else
            {
                throw missingparam_exception( name );
            }
        }

        string filename_only( const string& filepath )
        {
            const solosnake::filepath fp( filepath );
            return fp.leaf().string();
        }

        //! Returns a range. ASCII would be 0 - 128, and would not include 128.
        pair<uint32_t, uint32_t> make_range( const string& text )
        {
            static const regex decRange( "(\\d+)\\.\\.(\\d+)" );
            static const regex utf8Range( "U\\+([0-9A-F]{4})\\.\\.U\\+([0-9A-F]{4})" );

            pair<uint32_t, uint32_t> range;

            smatch what;
            if( regex_match( text, what, decRange ) )
            {
                range.first  =     atoi( static_cast<const char*>( &*what[1].first ) );
                range.second = 1 + atoi( static_cast<const char*>( &*what[2].first ) );

                if( range.first >= range.second )
                {
                    throw runtime_error( "Invalid range - range start must be less "
                                         "than or equal to range end" );
                }
            }
            else
            {
                if( regex_match( text, what, utf8Range ) )
                {
                    auto r0 = static_cast<const char*>( &*what[1].first );
                    auto r1 = static_cast<const char*>( &*what[2].first );

                    range.first  =     strtol( r0, nullptr, 16 );
                    range.second = 1 + strtol( r1, nullptr, 16 );

                    if( range.first >= range.second )
                    {
                        throw runtime_error(
                            "Invalid range - range start must be less than or equal to range end" );
                    }
                }
                else
                {
                    throw runtime_error(
                        "Unable to parse range. Expected range in format e.g. 0..127 or U+0000..U+007F" );
                }
            }

            return range;
        }

        //! Returns a vector of unique codepoints from the ranges specified.
        vector<uint32_t> make_charset( const vector<string>& strings )
        {
            set<uint32_t> chars;

            for_each( strings.cbegin(),
                      strings.cend(),
                      [&]( const string & rng )
            {
                auto r = make_range( rng );

                for( unsigned int i = r.first; i < r.second; ++i )
                {
                    chars.insert( i );
                }
            } );

            return vector<uint32_t>( chars.cbegin(), chars.cend() );
        }

        //! Class capturing the program options/config.
        class fontmaker_options
        {
        public:

            fontmaker_options() : character_size_pixels_( 0 ), padding_( 2 ), quiet_( false )
            {
            }

            vector<uint32_t>  characters_;
            string            fontfile_;
            string            texfile_;
            string            xmlfile_;
            unsigned int      character_size_pixels_;
            unsigned int      padding_;
            bool              quiet_;
        };

        template <typename T> void save( const T& t, const string& filename, bool verbose )
        {
            if( verbose )
            {
                ss_log( "Writing... ", filename );
            }

            if( false == t.save( filename.c_str() ) )
            {
                ss_err( "Unable to write to '", filename, "'" );
                ss_throw( "Unable to write to output file." );
            }
        }

        void read_options_and_make_font( int argc, char* argv[] )
        {
            namespace po = boost::program_options;

            fontmaker_options opts;

            // Declare the supported fontmaker_options.
            po::options_description desc( SOLOSNAKE_FONTMAKER_APP_TITLE " fontmaker_options" );

            desc.add_options()
            ( "help,?", "show help message" )
            ( "font,f", po::value<string>(), "font file to use" )
            ( "padding,p", po::value<unsigned int>()->default_value( 0 ), "pixel padding surrounding characters" )
            ( "quiet,q",  po::value<bool>()->default_value( false )->zero_tokens(), "toggle quiet mode" )
            ( "ranges,r", po::value<vector<string>>()->composing()->multitoken(), "inclusive character ranges, separated by '..' e.g. 0..127 is ASCII" )
            ( "size,s", po::value<unsigned int>(), "character size, pixels" )
            ( "output,t", po::value<string>(), "resulting texture filename (output)" )
            ( "xml,x", po::value<string>(), "resulting xml filename (output)" )
            ;

            po::variables_map vm;
            po::store( po::parse_command_line( argc, argv, desc ), vm );
            po::notify( vm );

            if( vm.count( "help" ) || argc == 1 )
            {
                cout << desc;
            }
            else
            {
                opts.fontfile_   = get_option_param<string>( "font", vm );
                opts.texfile_    = get_option_param<string>( "output", vm );
                opts.xmlfile_    = get_option_param<string>( "xml", vm );
                opts.character_size_pixels_ = get_option_param<unsigned int>( "size", vm );
                opts.padding_    = get_option_param<unsigned int>( "padding", vm );
                opts.quiet_      = get_option_param<bool>( "quiet", vm );
                opts.characters_ = make_charset( get_option_param<vector<string>>( "ranges", vm ) );

                const bool verbose = opts.quiet_ == false;

                auto result = make_font( opts.fontfile_,
                                         filename_only( opts.texfile_ ),
                                         opts.characters_,
                                         opts.character_size_pixels_,
                                         opts.padding_,
                                         verbose );

                save( *result.first, opts.texfile_, verbose );
                save( *result.second, opts.xmlfile_, verbose );

                if( verbose )
                {
                    ss_log( "Done." );
                }
            }
        }

        //! Driver / exception catcher.
        int run_fontmaker( int argc, char* argv[] )
        {
            int returncode = 0;

            try
            {
                read_options_and_make_font( argc, argv );
            }
            catch( const missingparam_exception& e )
            {
                cerr << "Missing param '" << e.what() << "'";
                returncode = SOLOSNAKE_EXCEPTION_ERRCODE;
            }
            catch( const exception& e )
            {
                cerr << "Exception: " << e.what();
                returncode = SOLOSNAKE_EXCEPTION_ERRCODE;
            }
            catch( ... )
            {
                cerr << "Unknown Exception";
                returncode = SOLOSNAKE_UNKNOWNEXCEPTION_ERRCODE;
            }

            return returncode;
        }
    }
}

int main( int argc, char* argv[] )
{
    return solosnake::run_fontmaker( argc, argv );
}