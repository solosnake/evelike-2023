#ifndef EXTERNAL_XML_HPP
#define EXTERNAL_XML_HPP

#ifndef TIXML_USE_STL
#   define TIXML_USE_STL
#endif

#ifdef _MSC_VER
#   ifndef _CRT_SECURE_NO_WARNINGS
#       define _CRT_SECURE_NO_WARNINGS
#   endif
#endif

//lint -save -w0
#include "xml/tinyxml.h"
//lint -restore

#include <string>
#include <cstdint>

namespace solosnake
{
    class bgr;
    class bgra;

    //! Reads the names attribute from the XML element, throwing if it is
    //! not present. Assumes that the given C string is safe to use.
    std::string read_attribute( const TiXmlElement&, const char* attribName );

    //! Reads the names attribute from the XML element, throwing if it not optional
    //! and is not present. Assumes that the given C string is safe to use.
    std::string read_attribute( const TiXmlElement&, const char* attribName, bool optional );

    //! Reads named attribute from XML element, and fills in the value if
    //! present and returns true. If not found, logs error message and throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, int&, bool optional );

    //! Reads named attribute from XML element, and fills in the value if
    //! present and returns true. If not found, logs error message and throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, unsigned int&, bool optional );

    //! Reads named attribute from XML element, and fills in the value if
    //! present and returns true. If not found, logs error message and throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, std::uint8_t&, bool optional );

    //! Reads named attribute from XML element, and fills in the value if
    //! present and returns true. If not found, logs error message and throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, unsigned short&, bool optional );

    //! Reads named attribute from XML element, and fills in the value if
    //! present and returns true. If not found, logs error message and throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, short&, bool optional );

    //! Reads named attribute from XML element, and fills in the value if
    //! present and returns true. If not found, logs error message and throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, double&, bool optional );

    //! Reads named attribute from XML element, and fills in the value if
    //! present and returns true. If not found, logs error message and throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, float&, bool optional );

    //! Reads named attribute from XML element, and fills in the value if present
    // and
    //! present and returns true. If not found, logs error message and throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, std::string&, bool optional );

    //! Reads named bool attribute from XML element ("true" or "false"), and
    //! fills in the value if present and returns true. If not found, logs error
    //! message and returns throws.
    bool read_attribute( const TiXmlElement& xmlWidget, const char* attribName, bool& v, bool optional );

    //! Reads named bgra colour attribute from XML element, and fills in the
    //! value if present and returns true. If not found, logs error message and
    //! returns throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, bgra&, bool optional );

    //! Reads named bgr colour attribute from XML element, and fills in the
    //! value if present and returns true. If not found, logs error message and
    //! returns throws.
    bool read_attribute( const TiXmlElement&, const char* attribName, bgr&, bool optional );

    //! Returns true if the XML Document has no errors, else logs the error
    //! and returns throws.
    bool xml_doc_no_err( const TiXmlDocument& );

    //! Opens and loads Tiny XML Document in UTF-8 mode, or logs and returns
    //! false if unable to open.
    //! Returns false if filename is nullptr.
    bool try_load_xml_doc( TiXmlDocument&, const char* filename );

    //! Opens and loads Tiny XML Document in UTF-8 mode, or logs and throws if
    //! unable to open.
    //! Throws if filename is nullptr.
    void load_xml_doc( TiXmlDocument&, const char* filename );
    
    //! Returns a pointer to the named child element, or throws if it could not be found.
    const TiXmlElement& get_child_element( const TiXmlDocument&, const char* const name );

    //! Returns a pointer to the named child element, or throws if it could not be found.
    TiXmlElement& get_child_element( TiXmlDocument&, const char* const name );

    //! Returns a pointer to the named child element, or throws if it could not be found.
    TiXmlElement& get_child_element( TiXmlElement&, const char* const name );
    
    //! Returns a pointer to the named sibling element, or throws if it could not be found.
    TiXmlElement& get_sibling_element( TiXmlElement&, const char* const name );
    
    //! Returns a pointer to the named child element, or throws if it could not be found.
    const TiXmlElement& get_child_element( const TiXmlElement&, const char* const name );
    
    //! Returns a pointer to the named sibling element, or throws if it could not be found.
    const TiXmlElement& get_sibling_element( const TiXmlElement&, const char* const name );
}

#endif
