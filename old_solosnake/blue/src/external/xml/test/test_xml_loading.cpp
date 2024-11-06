#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "solosnake/testing/testing.hpp"
#include "solosnake/external/xml.hpp"

void ParseAndSave( const char* xml, const char* filename );
bool TestLoad( const char* filename );

/**
 * Example actor system XML.
 */
const char* locationXML = "<?xml version=\"1.0\">\n"
                          "<actorsystemvalues name=\"default_location\">\n"
                          "<values>\n"
                          "<trait name=\"LocationX\" value=\"0.0\" />\n"
                          "<trait name=\"LocationZ\" value=\"0.0\" />\n"
                          "</values>\n"
                          "</actorsystemvalues>\n";

/**
* Example actor system XML.
*/
const char* movementXML = "<?xml version=\"1.0\">\n"
                          "<actorsystemvalues name=\"megathron_movement\">\n"
                          "<subsystems>\n"
                          "<subsystem name=\"default_location\" />\n"
                          "</subsystems>\n"
                          "<values>\n"
                          "<trait name=\"DirectionY\" value=\"1.4\" />\n"
                          "</values>\n"
                          "</actorsystemvalues>\n";

TEST( xml_loading, xml1 )
{
    {
        {
            ParseAndSave( locationXML, "location1.xml" );
            ParseAndSave( movementXML, "movement1.xml" );
        }

        EXPECT_TRUE( TestLoad( "location1.xml" ) );
        EXPECT_TRUE( TestLoad( "movement1.xml" ) );
    }
}

bool TestLoad( const char* filename )
{
    TiXmlDocument doc( filename );
    return doc.LoadFile();
}

void ParseAndSave( const char* xml, const char* filename )
{
    TiXmlDocument doc( filename );
    doc.Parse( xml );
    if( doc.Error() )
    {
        printf( "Error in %s: %s\n", doc.Value(), doc.ErrorDesc() );
        exit( 1 );
    }
    doc.SaveFile();
}
