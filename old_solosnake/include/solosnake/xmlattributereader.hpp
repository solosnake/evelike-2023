#ifndef solosnake_xmlattributereader_hpp
#define solosnake_xmlattributereader_hpp

#include "solosnake/iattributereader.hpp"
#include "solosnake/external/xml.hpp"

namespace solosnake
{
    //! Interface adaptor for an XML element instance. This object requires the
    //! element it is used with to remain in scope.
    class xmlattributereader : public iattributereader
    {
    public:

        explicit xmlattributereader( const TiXmlElement& );

        //! Reads the names attribute from the the data, throwing if it not optional
        //! and is not present. Assumes that the given C string is safe to use.
        bool read_attribute( const char* attribName, std::string&, bool optional ) const override;

        //! Reads named bool attribute from the data (only "true" or "false" is
        //! permitted, and fills in the value if present and returns true. Returns
        //! false if not found or invalid data was found.
        bool read_attribute( const char* attribName, bool&, bool optional ) const override;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, int&, bool optional ) const override;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, unsigned int&, bool optional ) const override;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, std::uint8_t&, bool optional ) const override;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, short&, bool optional ) const override;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, unsigned short&, bool optional ) const override;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, double&, bool optional ) const override;

        //! Reads named attribute from the data, and fills in the value if present
        //! and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, float&, bool optional ) const override;

        //! Reads named Bgra colour attribute from the data, and fills in the
        //! value if present and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, Bgra&, bool optional ) const override;

        //! Reads named Bgr colour attribute from the data, and fills in the
        //! value if present and returns true. Returns false if not found.
        bool read_attribute( const char* attribName, Bgr&, bool optional ) const override;

        //! Reads the names attribute from the the data, throwing if it not optional
        //! and is not present. Assumes that the given C string is safe to use.
        std::string read_attribute( const char* attribName, bool optional ) const override;

    private:
        xmlattributereader( const xmlattributereader& );
        xmlattributereader& operator=( const xmlattributereader& );

    private:
        const TiXmlElement* xml_;
    };
}

#endif
