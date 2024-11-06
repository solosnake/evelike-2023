
#include "solosnake/external/xml.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/bgra.hpp"

// Note: CPP includes!
//lint -save -w0
#include <tinyxml.cpp>
#include <tinyxmlerror.cpp>
#include <tinyxmlparser.cpp>
//lint -restore

namespace solosnake
{
    std::string read_attribute( const TiXmlElement& xml, const char* attribName )
    {
        return read_attribute( xml, attribName, false );
    }

    std::string read_attribute( const TiXmlElement& xml, const char* attribName, bool optional )
    {
        assert( attribName );

        std::string result;

        const std::string* a = xml.Attribute( std::string( attribName ) );

        if( nullptr != a )
        {
            result = *a;
        }
        else
        {
            if( !optional )
            {
                ss_err( "XML is missing attribute ", attribName );
                ss_throw( "Missing attribute in xml definition." );
            }
        }

        return result;
    }

    bool read_attribute( const TiXmlElement& xmlWidget, const char* attribName, int& v, bool optional )
    {
        assert( attribName );

        if( xmlWidget.QueryIntAttribute( attribName, &v ) == TIXML_SUCCESS )
        {
            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'int' attribute : ", attribName );
                ss_throw( "Mising 'int' attribute " );
            }

            return false;
        }
    }

    bool read_attribute( const TiXmlElement& xmlWidget,
                         const char* attribName,
                         unsigned int& v,
                         bool optional )
    {
        assert( attribName );

        int i;
        if( xmlWidget.QueryIntAttribute( attribName, &i ) == TIXML_SUCCESS )
        {
            if( i < 0 )
            {
                ss_err( "unsigned int attribute '", attribName, "' must not have a negative value." );

                ss_throw( "Attribute out of range." );
            }
            else
            {
                v = static_cast<unsigned int>( i );
            }

            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'unsigned int' attribute : ", attribName );
                ss_throw( "Mising 'unsigned int' attribute " );
            }

            return false;
        }
    }

    bool read_attribute( const TiXmlElement& xmlWidget,
                         const char* attribName,
                         std::uint8_t& v,
                         bool optional )
    {
        assert( attribName );

        int i;
        if( xmlWidget.QueryIntAttribute( attribName, &i ) == TIXML_SUCCESS )
        {
            if( i < 0 )
            {
                ss_err( "uint8_t attribute '", attribName, "' must not have a negative value." );

                ss_throw( "Attribute out of range." );
            }
            else
            {
                v = static_cast<std::uint8_t>( i );
            }

            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'uint8_t' attribute : ", attribName );
                ss_throw( "Mising 'uint8_t' attribute " );
            }

            return false;
        }
    }

    bool read_attribute( const TiXmlElement& xmlWidget, const char* attribName, short& v, bool optional )
    {
        assert( attribName );

        int i;
        if( xmlWidget.QueryIntAttribute( attribName, &i ) == TIXML_SUCCESS )
        {
            v = static_cast<short>( i );

            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Mising 'short' attribute : ", attribName );
                ss_throw( "Mising 'short' attribute " );
            }

            return false;
        }
    }

    bool read_attribute( const TiXmlElement& xmlWidget,
                         const char* attribName,
                         unsigned short& v,
                         bool optional )
    {
        assert( attribName );

        int i;
        if( xmlWidget.QueryIntAttribute( attribName, &i ) == TIXML_SUCCESS )
        {
            if( i < 0 )
            {
                ss_err( "unsigned short attribute '", attribName, "' must not have a negative value." );
                ss_throw( "Attribute out of range." );
            }
            else
            {
                v = static_cast<unsigned short>( i );
            }

            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Mising 'unsigned short' attribute : ", attribName );
                ss_throw( "Mising 'unsigned short' attribute " );
            }

            return false;
        }
    }

    bool read_attribute( const TiXmlElement& xmlWidget, const char* attribName, float& v, bool optional )
    {
        assert( attribName );

        if( xmlWidget.QueryFloatAttribute( attribName, &v ) == TIXML_SUCCESS )
        {
            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'float' attribute : ", attribName );
                ss_throw( "Mising 'float' attribute " );
            }

            return false;
        }
    }

    bool read_attribute( const TiXmlElement& xmlWidget, const char* attribName, double& v, bool optional )
    {
        assert( attribName );

        if( xmlWidget.QueryDoubleAttribute( attribName, &v ) == TIXML_SUCCESS )
        {
            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'double' attribute : ", attribName );
                ss_throw( "Mising 'double' attribute " );
            }

            return false;
        }
    }

    bool read_attribute( const TiXmlElement& xmlWidget,
                         const char* attribName,
                         std::string& v,
                         bool optional )
    {
        assert( attribName );

        if( xmlWidget.QueryStringAttribute( attribName, &v ) == TIXML_SUCCESS )
        {
            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'string' attribute : ", attribName );
                ss_throw( "Mising 'string' attribute " );
            }

            return false;
        }
    }

    bool read_attribute( const TiXmlElement& xmlWidget, const char* attribName, bool& v, bool optional )
    {
        assert( attribName );

        std::string sbool;
        if( xmlWidget.QueryStringAttribute( attribName, &sbool ) == TIXML_SUCCESS )
        {
            if( 0 == sbool.compare( "true" ) )
            {
                v = true;
                return true;
            }
            else if( 0 == sbool.compare( "false" ) )
            {
                v = false;
                return true;
            }
            else
            {
                ss_err( "Invalid 'bool' value : ", sbool );
                return false;
            }
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'bool' attribute : ", attribName );
                ss_throw( "Mising 'bool' attribute " );
            }

            return false;
        }
    }

    //! Reads named Bgra colour attribute from XML.
    bool read_attribute( const TiXmlElement& xmlWidget, const char* attribName, Bgra& c, bool optional )
    {
        assert( attribName );

        std::string strColour;

        if( xmlWidget.QueryStringAttribute( attribName, &strColour ) == TIXML_SUCCESS )
        {
            c = Bgra( strColour );
            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'Bgra' attribute : ", attribName );
                ss_throw( "Mising 'Bgra' attribute " );
            }

            return false;
        }
    }

    // Reads named bgr colour attribute from XML.
    bool read_attribute( const TiXmlElement& xmlWidget, const char* attribName, bgr& c, bool optional )
    {
        assert( attribName );

        std::string strColour;

        if( xmlWidget.QueryStringAttribute( attribName, &strColour ) == TIXML_SUCCESS )
        {
            c = bgr( strColour );
            return true;
        }
        else
        {
            if( !optional )
            {
                ss_err( "Missing 'bgr' attribute : ", attribName );
                ss_throw( "Mising 'bgr' attribute " );
            }

            return false;
        }
    }

    // Returns false when errors, else true.
    bool xml_doc_no_err( const TiXmlDocument& doc )
    {
        if( doc.Error() )
        {
            ss_err( "XML error, ", doc.ErrorDesc(), " row: ", doc.ErrorRow(), " col: ", doc.ErrorCol() );
            return false;
        }

        return true;
    }

    //! Standardises output from failures to load XML files.
    bool try_load_xml_doc( TiXmlDocument& doc, const char* filename )
    {
        if( nullptr == filename )
        {
            ss_err( "Attempt to open document with nullptr string name." );
            return false;
        }

        if( doc.LoadFile( filename, TIXML_ENCODING_UTF8 ) )
        {
            return xml_doc_no_err( doc );
        }
        else
        {
            ss_err( "Unable to load XML file : ", filename );
            ss_err( doc.ErrorDesc() );
        }

        return false;
    }

    // Standardises exception from failures to load XML files.
    void load_xml_doc( TiXmlDocument& doc, const char* filename )
    {
        if( !try_load_xml_doc( doc, filename ) )
        {
            ss_throw( "Error opening XML file." );
        }
    }

    TiXmlElement& get_child_element( TiXmlDocument& e, const char* const name )
    {
        assert( name );

        TiXmlElement* const c = e.FirstChildElement( name );

        if( c == nullptr )
        {
            ss_err( "Unable to find child element named ", name, " in TiXmlDocument." );
            ss_throw( "Unable to find named child element in TiXmlDocument." );
        }

        return *c;
    }

    const TiXmlElement& get_child_element( const TiXmlDocument& e, const char* const name )
    {
        assert( name );

        const TiXmlElement* const c = e.FirstChildElement( name );

        if( c == nullptr )
        {
            ss_err( "Unable to find (const) child element named ", name, " in TiXmlDocument." );
            ss_throw( "Unable to find (const) named child element in TiXmlDocument." );
        }

        return *c;
    }

    TiXmlElement& get_child_element( TiXmlElement& e, const char* const name )
    {
        assert( name );

        TiXmlElement* const c = e.FirstChildElement( name );

        if( c == nullptr )
        {
            ss_err( "Unable to find child element named ", name );
            ss_throw( "Unable to find named child element." );
        }

        return *c;
    }

    const TiXmlElement& get_child_element( const TiXmlElement& e, const char* const name )
    {
        assert( name );

        const TiXmlElement* const c = e.FirstChildElement( name );

        if( c == nullptr )
        {
            ss_err( "Unable to find (const) child element named ", name );
            ss_throw( "Unable to find (const) named child element." );
        }

        return *c;
    }

    TiXmlElement& get_sibling_element( TiXmlElement& e, const char* const name )
    {
        assert( name );

        TiXmlElement* const c = e.NextSiblingElement( name );

        if( c == nullptr )
        {
            ss_err( "Unable to find sibling element named ", name );
            ss_throw( "Unable to find named sibling element." );
        }

        return *c;
    }

    const TiXmlElement& get_sibling_element( const TiXmlElement& e, const char* const name )
    {
        assert( name );

        const TiXmlElement* const c = e.NextSiblingElement( name );

        if( c == nullptr )
        {
            ss_err( "Unable to find (const) sibling element named ", name );
            ss_throw( "Unable to find (const) named sibling element." );
        }

        return *c;
    }

}
