#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <stdexcept>
#include "solosnake/bgra.hpp"
#include "solosnake/iattributereader.hpp"
#include "solosnake/logging.hpp"

namespace solosnake
{
    namespace
    {
        template<typename T>
        static T from_string( const std::string& txt );

        template<>
        std::uint8_t from_string<std::uint8_t>( const std::string& txt )
        {
            const auto v = std::stoul( txt );

            if( v > std::numeric_limits<std::uint8_t>::max() )
            {
                throw std::out_of_range( "Value larger than std::uint8_t." );
            }

            return static_cast<std::uint8_t>( v );
        }

        template<>
        unsigned int from_string<unsigned int>( const std::string& txt )
        {
            const auto v = std::stoul( txt );

            if( v > std::numeric_limits<unsigned int>::max() )
            {
                throw std::out_of_range( "Value larger than unsigned int." );
            }

            return static_cast<unsigned int>( v );
        }

        template<>
        unsigned short from_string<unsigned short>( const std::string& txt )
        {
            const auto v = std::stoul( txt );

            if( v > std::numeric_limits<unsigned short>::max() )
            {
                throw std::out_of_range( "Value larger than unsigned short." );
            }

            return static_cast<unsigned short>( v );
        }

        template<>
        int from_string<int>( const std::string& txt )
        {
            return static_cast<int>( std::stoi( txt ) );
        }

        template<>
        short from_string<short>( const std::string& txt )
        {
            return static_cast<short>( std::stoi( txt ) );
        }

        template<>
        float from_string<float>( const std::string& txt )
        {
            return std::stof( txt );
        }

        template<>
        double from_string<double>( const std::string& txt )
        {
            return std::stod( txt );
        }

        template <typename T>
        static bool try_read( const iattributereader& reader,
                              T& value,
                              const char* attribName,
                              bool optional )
        {
            std::string attrib;
            if( reader.read_attribute( attribName, attrib, optional ) )
            {
                if( attrib.length() > 0 )
                {
                    try
                    {
                        // If no conversion could be performed an invalid_argument or out_of_range exception is thrown.
                        value = from_string<T>( attrib );
                    }
                    catch( ... )
                    {
                        return false;
                    }

                    return true;
                }
            }

            return false;
        }
    }

    iattributereader::~iattributereader()
    {
    }

    std::string iattributereader::read_attribute( const char* attribName, bool optional ) const
    {
        std::string attrib;
        read_attribute( attribName, attrib, optional );
        return attrib;
    }

    bool iattributereader::read_attribute( const char* attribName, bool& value, bool optional ) const
    {
        std::string attrib;
        const bool found = read_attribute( attribName, attrib, optional );

        if( found )
        {
            if( 0 == attrib.compare( "true" ) )
            {
                value = true;
            }
            else if( 0 == attrib.compare( "false" ) )
            {
                value = false;
            }
            else
            {
                ss_err( "Invalid 'bool' value : ", attrib );
                return false;
            }
        }

        return found;
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           int& value,
                                           bool optional ) const
    {
        return try_read( *this, value, attribName, optional );
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           unsigned int& value,
                                           bool optional ) const
    {
        return try_read( *this, value, attribName, optional );
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           std::uint8_t& value,
                                           bool optional ) const
    {
        return try_read( *this, value, attribName, optional );
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           short& value,
                                           bool optional ) const
    {
        return try_read( *this, value, attribName, optional );
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           unsigned short& value,
                                           bool optional ) const
    {
        return try_read( *this, value, attribName, optional );
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           double& value,
                                           bool optional ) const
    {
        return try_read( *this, value, attribName, optional );
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           float& value,
                                           bool optional ) const
    {
        return try_read( *this, value, attribName, optional );
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           bgra& value,
                                           bool optional ) const
    {
        std::string attrib;
        if( read_attribute( attribName, attrib, optional ) )
        {
            if( attrib.length() > 0 )
            {
                value = bgra( attrib );
                return true;
            }
        }

        return false;
    }

    bool iattributereader::read_attribute( const char* attribName,
                                           bgr& value,
                                           bool optional ) const
    {
        std::string attrib;
        if( read_attribute( attribName, attrib, optional ) )
        {
            if( attrib.length() > 0 )
            {
                value = bgr( attrib );
                return true;
            }
        }

        return false;
    }
}
