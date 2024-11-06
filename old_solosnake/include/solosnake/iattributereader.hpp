#ifndef solosnake_iattributereader_hpp
#define solosnake_iattributereader_hpp

#include <string>
#include <cstdint>

namespace solosnake
{
    class Bgr;
    class Bgra;

    //! Abstraction of a reader of named values. Can abstract an XML reader,
    //! or any textual data:value set.
    class iattributereader
    {
    public:

        virtual ~iattributereader();

        //! Reads the names attribute from the the data, throwing if it not optional
        //! and is not present. Assumes that the given C string is safe to use.
        //! All of the other virtual functions are by default implemented using this
        //! one, and upon a successful read of the string data associated with
        //! @a attribName, will attempt a lexical cast of the data to the returned
        //! type.
        virtual bool read_attribute( const char* attribName, std::string&, bool optional ) const = 0;

        //! Reads named bool attribute from the data (only "true" or "false" is
        //! permitted, and fills in the value if present and returns true. Returns
        //! false if not found or invalid data was found.
        virtual bool read_attribute( const char* attribName, bool&, bool optional ) const;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, short&, bool optional ) const;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, unsigned short&, bool optional ) const;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, int&, bool optional ) const;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, unsigned int&, bool optional ) const;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, std::uint8_t&, bool optional ) const;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, float&, bool optional ) const;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, double&, bool optional ) const;

        //! Reads named Bgra colour attribute from the data, and fills in the
        //! value if present and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, Bgra&, bool optional ) const;

        //! Reads named Bgr colour attribute from the data, and fills in the
        //! value if present and returns true. Returns false if not found.
        virtual bool read_attribute( const char* attribName, Bgr&, bool optional ) const;

        //! Reads the names attribute from the the data, throwing if it not optional
        //! and is not present. Assumes that the given C string is safe to use.
        virtual std::string read_attribute( const char* attribName, bool optional ) const;
    };
}

#endif
