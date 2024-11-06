#include "solosnake/xmlattributereader.hpp"

namespace solosnake
{
    xmlattributereader::xmlattributereader( const TiXmlElement& tinyXML ) : xml_( &tinyXML )
    {
    }

    bool xmlattributereader::read_attribute( const char* attribName,
                                             std::string& value,
                                             bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    std::string xmlattributereader::read_attribute( const char* attribName, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, bool& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, int& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, unsigned int& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, std::uint8_t& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, unsigned short& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, short& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, double& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, float& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, bgra& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }

    bool xmlattributereader::read_attribute( const char* attribName, bgr& value, bool optional ) const
    {
        return solosnake::read_attribute( *xml_, attribName, value, optional );
    }
}
