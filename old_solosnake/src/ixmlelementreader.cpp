#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/logging.hpp"

namespace solosnake
{
    ixmlelementreader::ixmlelementreader( const std::string& groupName ) : groupName_( groupName )
    {
    }

    ixmlelementreader::~ixmlelementreader()
    {
    }

    bool ixmlelementreader::can_read_version( const std::string& ) const
    {
        return true;
    }

    bool ixmlelementreader::read_ended( bool goodRead )
    {
        return goodRead;
    }

    bool ixmlelementreader::read( const TiXmlNode* pNode, ixmlelementreader& reader )
    {
        bool loadedOK = false;

        if( pNode && pNode->ToElement() )
        {
            const TiXmlElement* pElem = pNode->ToElement();

            bool read_error = false;

            // Version is optional.
            const std::string* version = pElem->Attribute( std::string( "version" ) );

            if( version )
            {
                if( false == reader.can_read_version( *version ) )
                {
                    ss_err(
                        "ixmlelementreader: Cannot read version ", *version, " of ", reader.groupName_ );

                    read_error = true;
                }
            }

            if( false == read_error )
            {
                // Allow the reader to check the group elements contents.
                read_error = ( false == reader.read_group_element( *pElem ) );

                // And skip to first (optional) child node (which might be a
                // comment).
                pElem = pNode->FirstChildElement();

                while( pElem && ( false == read_error ) )
                {
                    read_error = ( false == reader.read_from_element( *pElem ) );
                    pElem = pElem->NextSiblingElement();
                }

                if( false == read_error )
                {
                    loadedOK = true;
                }
            }
        }

        return loadedOK;
    }

    bool ixmlelementreader::read( const TiXmlDocument& doc, ixmlelementreader& reader )
    {
        bool loadedOK = false;

        const TiXmlNode* pNode = doc.FirstChild( reader.groupName_ );

        if( pNode && pNode->ToElement() )
        {
            loadedOK = read( pNode, reader ) && ( false == doc.Error() );
        }

        if( doc.Error() )
        {
            ss_err( "Error reading XML : ",
                    doc.ErrorDesc(),
                    ", row ",
                    doc.ErrorRow(),
                    ", col ",
                    doc.ErrorCol() );
        }

        return loadedOK;
    }

    bool ixmlelementreader::read( const char* filename, ixmlelementreader& reader )
    {
        TiXmlDocument doc;

        const bool loadedOK = try_load_xml_doc( doc, filename ) && read( doc, reader );

        return reader.read_ended( loadedOK );
    }

    bool ixmlelementreader::read( const std::string& filename, ixmlelementreader& reader )
    {
        return ixmlelementreader::read( filename.c_str(), reader );
    }

    bool ixmlelementreader::read_group_element( const TiXmlElement& )
    {
        return true;
    }
}
