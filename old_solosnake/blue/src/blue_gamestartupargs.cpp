#include <sstream>
#include <vector>
#include "solosnake/blue/blue_gamestartupargs.hpp"
#include "solosnake/throw.hpp"

namespace blue
{
#   define SS_DELIMITER                ';'
#   define BLUE_GAMESTARTUPARGS_COUNT  (2)

    namespace
    {
        //! Checks for delimiter and skips it.
        template <typename T, typename TRAITS>
        std::basic_istream<T, TRAITS>& delim( std::basic_istream<T, TRAITS>& is )
        {
            if( ( is >> std::ws ).peek() != is.widen( SS_DELIMITER ) )
            {
                is.setstate( std::ios_base::failbit );
            }
            else
            {
                is.ignore();
            }

            return is;
        }

        //! Class to wrap string for specialist stream reading.
        class delimited_text_shim
        {
        public:
            delimited_text_shim( std::string& s ) : contents_( s )
            {
            }
            std::string& contents_;

        private:
            delimited_text_shim& operator=( const delimited_text_shim& );
        };

        //! Reads in a string and not the delimiter.
        std::istream& operator>>( std::istream& is, delimited_text_shim& t )
        {
            while( is.peek() != is.widen( SS_DELIMITER ) )
            {
                t.contents_.push_back( static_cast<std::string::value_type>( is.get() ) );
            }

            return is;
        }
    }

    gamestartupargs::gamestartupargs( unsigned int seed, const std::string& boardname )
        : rand_seed_( seed ), boardname_( boardname )
    {
    }

    //! Converts from a string to startup arguments. This string is likely
    //! received over the network.
    gamestartupargs::gamestartupargs( const char* txt )
    {
        std::istringstream is( txt );

        delimited_text_shim boardname( boardname_ );

        is >> rand_seed_ >> delim >> boardname >> delim;

        if( is.bad() )
        {
            ss_throw( "Failed to correctly parse 'gamestartupargs' text." );
        }
    }

    //! Converts the startup arguments to a string which can be
    //! used to initialise and recreate a gamestartupargs object.
    //! @code
    //! gamestartupargs a( 1, "starter.board" );
    //! gamestartupargs b( a.to_str().c_str() );
    //! assert( a == b );
    //! @endcode
    std::string gamestartupargs::to_str() const
    {
        std::ostringstream os;

        os << rand_seed_ << SS_DELIMITER << boardname_ << SS_DELIMITER;

        return os.str();
    }

    bool gamestartupargs::operator==( const gamestartupargs& rhs ) const
    {
        return rand_seed_ == rhs.rand_seed_ && boardname_ == rhs.boardname_;
    }

    bool gamestartupargs::operator!=( const gamestartupargs& rhs ) const
    {
        return !( *this == rhs );
    }
}
