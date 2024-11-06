#ifndef solosnake_persistance_hpp
#define solosnake_persistance_hpp

#include <iosfwd>
#include <map>
#include <memory>
#include <string>

namespace solosnake
{
    //! Type returned as value from persistance. Helper class for
    //! conversions of the key values to types such as booleans and
    //! integers etc. Automatically works as a string.
    //! The comments do not take part in comparison operations.
    class persistance_value
    {
    public:

        persistance_value() : value_(), comment_()
        {
        }

        persistance_value( const persistance_value& rhs ) : value_( rhs.value_ ), comment_( rhs.comment_ )
        {
        }

        persistance_value( persistance_value&& rhs )
            : value_( std::move( rhs.value_ ) ), comment_( std::move( rhs.comment_ ) )
        {
        }

        persistance_value& operator=( persistance_value && rhs )
        {
            if( this != &rhs )
            {
                value_ = std::move( rhs.value_ );
                comment_ = std::move( rhs.comment_ );
            }
            return *this;
        }

        persistance_value& operator=( const persistance_value& rhs )
        {
            if( this != &rhs )
            {
                value_ = rhs.value_;
                comment_ = rhs.comment_;
            }
            return *this;
        }

        explicit persistance_value( const std::string& s ) : value_( s ), comment_()
        {
        }

        persistance_value( const std::string& s, const std::string& c ) : value_( s ), comment_( c )
        {
        }

        bool is_true() const
        {
            return value_ == "true";
        }

        bool operator<( const persistance_value& rhs ) const
        {
            return value_ < rhs.value_;
        }

        bool operator==( const persistance_value& rhs ) const
        {
            return value_ == rhs.value_;
        }

        bool operator!=( const persistance_value& rhs ) const
        {
            return value_ != rhs.value_;
        }

        bool operator==( const std::string& rhs ) const
        {
            return value_ == rhs;
        }

        bool operator!=( const std::string& rhs ) const
        {
            return value_ != rhs;
        }

        bool operator==( const char* rhs ) const
        {
            return rhs && 0 == value_.compare( rhs );
        }

        bool operator!=( const char* rhs ) const
        {
            return rhs && 0 == value_.compare( rhs );
        }

        const std::string::value_type* c_str() const
        {
            return value_.c_str();
        }

        const std::string& str() const
        {
            return value_;
        }

        const std::string& comment() const
        {
            return comment_;
        }

        operator const std::string& () const
        {
            return value_;
        }

        int as_int() const;

        const std::string& as_string() const
        {
            return str();
        }

        unsigned int as_uint() const;

        //! Throws if the source values is greater than is representable by an unsigned short.
        unsigned short as_ushort() const;

        double as_double() const;

        float as_float() const;

    private:

        std::string value_;
        std::string comment_; //!< Explanation of the use or meaning of the value.
    };

    //! A string to string mapping which can be loaded and saved to XML.
    class persistance
    {
    public:

        persistance();

        explicit persistance( const std::string& filename );

        virtual ~persistance();

        //! Sets the value associated with the given key.
        //! The empty key may be used as a value, but not as a
        //! key.
        void set( const std::string& key, const std::string& value, std::string comment );

        //! Sets the value and comment only if its key does not already exist.
        //! @param key     - The key used to retrieve this value.
        //! @param value   - A string which can be retrieved as an int/string later.
        //! @param comment - Writes a comment explaining this key in the XML.
        void set_if_not_present( const std::string& key,
                                 const std::string& value,
                                 const std::string& comment );

        //! Returns the value associated with the given key.
        //! Throws if the key is not present in the mapping.
        const persistance_value& operator[]( const std::string& key ) const;

        //! Returns the value associated with the given key.
        //! Throws if the key is not present in the mapping. This one is easier
        //! to use with smart pointer objects.
        const persistance_value& value( const std::string& key ) const;

        bool save_to_xml( const char* filename ) const;

        bool load_from_xml( const char* filename );

        bool contains( const std::string& key ) const;

        bool operator==( const persistance& rhs ) const;

        bool operator!=( const persistance& rhs ) const;

        typedef std::map<std::string, persistance_value> keysvalues_t;

    protected:
        const keysvalues_t& values() const
        {
            return values_;
        }

        bool is_boolean( const std::string& key ) const;

    private:
        keysvalues_t values_;
    };

    std::ostream& operator<<( std::ostream&, const persistance_value& );
}

#endif
