#include <algorithm>
#include <fstream>
#include <sstream>
#include <memory>
#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    //-------------------------------------------------------------------------

#   define SS_PERSISTANCE_ELEMENT_NAME "persistance"
#   define SS_PERSISTANCE_VERSION "0.1"

    //! Returns the value as an integer, or throws is the value cannot
    //! be converted to an int.
    int persistance_value::as_int() const
    {
        int i = 0;
        std::istringstream is( value_ );
        is.exceptions( std::istringstream::badbit );
        is >> i;
        return i;
    }

    //! Returns the value as an unsigned integer, or throws is the value cannot
    //! be converted to an int.
    unsigned int persistance_value::as_uint() const
    {
        unsigned int i = 0u;
        std::istringstream is( value_ );
        is.exceptions( std::istringstream::badbit );
        is >> i;
        return i;
    }

    //! Returns the value as an unsigned short, or throws is the value cannot
    //! be converted to an short.
    unsigned short persistance_value::as_ushort() const
    {
        unsigned int i = as_uint();

        if( i > std::numeric_limits<unsigned short>::max() )
        {
            ss_throw( "Unsafe cast from unsigned int to unsigned short" );
        }

        return static_cast<short>( as_uint() );
    }

    //! Returns the value as an double, or throws is the value cannot
    //! be converted to an double.
    double persistance_value::as_double() const
    {
        double d = 0.0;
        std::istringstream is( value_ );
        is.exceptions( std::istringstream::badbit );
        is >> d;
        return d;
    }

    //! Returns the value as an float, or throws is the value cannot
    //! be converted to an float.
    float persistance_value::as_float() const
    {
        return static_cast<float>( as_double() );
    }

    //-------------------------------------------------------------------------

    namespace
    {
        //! <?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
        //! <persistance version="1.0">
        //! <entries>
        //! <entry key="clean-run" value="true" />
        //! <entry key="fullscreen" value="false" />
        //! <entry key="window-type" value="" />
        //! </entries>
        //! </persistance>
        class persistance_loader : public ixmlelementreader
        {
        public:
            persistance_loader( const std::string& version )
                : ixmlelementreader( SS_PERSISTANCE_ELEMENT_NAME )
                , version_( version )
                , key_( "key" )
                , value_( "value" )
            {
            }

            inline void swap( std::map<std::string, persistance_value>& other )
            {
                entries_.swap( other );
            }

        private:
            virtual bool can_read_version( const std::string& v ) const
            {
                return version_ == v;
            }

            virtual bool read_from_element( const TiXmlElement& elem )
            {
                const std::string* key = elem.Attribute( key_ );
                const std::string* value = elem.Attribute( value_ );
                std::string comment;

                if( elem.FirstChild() && TiXmlNode::TINYXML_COMMENT == elem.FirstChild()->Type() )
                {
                    comment = elem.FirstChild()->ValueStr();
                }

                if( key && value )
                {
                    entries_[*key] = persistance_value( *value, comment );
                }

                return key && value;
            }

        private:
            std::map<std::string, persistance_value> entries_;
            std::string version_;
            std::string key_;
            std::string value_;
        };

    }

    persistance::persistance()
    {
    }

    persistance::persistance( const std::string& filename )
    {
        if( false == load_from_xml( filename.c_str() ) )
        {
            ss_throw( "Failed to create/load persistance from file." );
        }
    }

    persistance::~persistance()
    {
    }

    void persistance::set( const std::string& key, const std::string& val, std::string comment )
    {
        if( key.empty() )
        {
            ss_throw( "An empty key was used to try to set a value on a persistance "
                      "object." );
        }
        else
        {
            // Keep the existing comment, if present.
            if( values_.count( key ) > 0 && comment.empty() )
            {
                comment = values_[key].comment();
            }

            values_[key] = persistance_value( val, comment );

#ifndef NDEBUG
            if( values_[key].comment().empty() )
            {
                ss_wrn( "persistance key/value: \"", key, "\"/\"", val, "\" has no comment" );
            }
#endif
        }
    }

    void persistance::set_if_not_present( const std::string& key,
                                          const std::string& val,
                                          const std::string& comment )
    {
        if( false == this->contains( key ) )
        {
            ss_log( "Setting missing variable ", key, " to ", val );
            this->set( key, val, comment );
        }
        else
        {
            // Sets the comment as the comment on the item.
            if( ( *this )[key].comment() != comment )
            {
                this->set( key, ( *this )[key], comment );
            }
        }
    }

    const persistance_value& persistance::operator[]( const std::string& key ) const
    {
        return this->value( key );
    }

    const persistance_value& persistance::value( const std::string& key ) const
    {
        auto i = values_.find( key );

        if( i == values_.end() )
        {
            ss_err( "Key not found in persistance: '", key, "'." );
            ss_throw( "A requested key was not present in the persistance." );
        }

        return i->second;
    }

    bool persistance::save_to_xml( const char* filename ) const
    {
        bool savedOK = false;

        if( filename )
        {
            TiXmlDocument doc( filename );

            if( false == doc.Error() )
            {
                doc.LinkEndChild( new TiXmlDeclaration( "1.0", "UTF-8", "yes" ) );
                TiXmlNode* entries = doc.LinkEndChild( new TiXmlElement( SS_PERSISTANCE_ELEMENT_NAME ) );
                doc.RootElement()->SetAttribute( "version", SS_PERSISTANCE_VERSION );

                if( entries )
                {
                    std::for_each( values_.cbegin(),
                                   values_.cend(),
                                   [ = ]( const keysvalues_t::value_type & i )
                    {
                        std::unique_ptr<TiXmlElement> entry( new TiXmlElement( "entry" ) );
                        entry->SetAttribute( "key", i.first );
                        entry->SetAttribute( "value", i.second.str() );

                        if( false == i.second.comment().empty() )
                        {
                            entry->LinkEndChild( new TiXmlComment( i.second.comment().c_str() ) );
                        }
                        else
                        {
                            assert( ! "EMPTY COMMENT" );
                        }

                        entries->LinkEndChild( entry.release() );
                    } );
                }

                savedOK = doc.SaveFile();
            }
        }

        return savedOK;
    }

    bool persistance::load_from_xml( const char* filename )
    {
        persistance_loader loader( SS_PERSISTANCE_VERSION );

        bool loadedOK = ixmlelementreader::read( filename, loader );

        if( loadedOK )
        {
            loader.swap( values_ );
        }

        return loadedOK;
    }

    bool persistance::operator==( const persistance& rhs ) const
    {
        return values_ == rhs.values_;
    }

    bool persistance::operator!=( const persistance& rhs ) const
    {
        return values_ != rhs.values_;
    }

    bool persistance::contains( const std::string& key ) const
    {
        return values_.find( key ) != values_.end();
    }

    bool persistance::is_boolean( const std::string& key ) const
    {
        return ( *this )[key] == "true" || ( *this )[key] == "false";
    }

    std::ostream& operator<<( std::ostream& os, const persistance_value& v )
    {
        os << v.str();
        return os;
    }
}
