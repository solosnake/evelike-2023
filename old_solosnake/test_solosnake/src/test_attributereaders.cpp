#include "solosnake/testing/testing.hpp"
#include <memory>
#include <fstream>
#include "solosnake/bgra.hpp"
#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/tsv.hpp"
#include "solosnake/tsvattributereader.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/xmlattributereader.hpp"

using namespace solosnake;
using namespace std;

namespace
{
// Test expect strings 'SampleXML' and 'SampleTSV' to be identical.

//! Contents must be same as SampleTSV.
static const char SampleXML[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<elements_group>"
                                "<element "
                                "a=\"Hello World\" "
                                "b=\"0.0\" "
                                "c=\"1\" "
                                "d=\"-1\" "
                                "e=\"true\" "
                                "f=\"0xFFFFFFFF\" "
                                ""
                                "/>"
                                "<element "
                                "a=\"solosnake\" "
                                "b=\"100000\" "
                                "c=\"1.0\" "
                                "d=\"-1000000\" "
                                "e=\"false\" "
                                "f=\"123\" "
                                ""
                                "/>"
                                "</elements_group>";

//! Contents must be same as SampleXML.
static const char SampleTSV[] = "a\tb\tc\td\te\tf\t\n"
                                "Hello World\t0.0\t1\t-1\ttrue\t0xFFFFFFFF\n"
                                "solosnake\t100000\t1.0\t-1000000\tfalse\t123\n";

//! Writes contents to file and returns freshly opened file.
shared_ptr<ifstream> create_test_file(const char* contents, const string& filename)
{
    ofstream ofile(filename.c_str());
    ofile << contents;
    ofile.flush();
    ofile.close();
    return make_shared<ifstream>(filename.c_str());
}

void test_read_of_element_1(const iattributereader& reader)
{
    string s;
    ASSERT_TRUE(reader.read_attribute("a", s, false));
    ASSERT_EQ("Hello World", s);

    float b = 1.0f;
    ASSERT_TRUE(reader.read_attribute("b", b, false));
    ASSERT_EQ(0.0f, b);

    unsigned int c = 0;
    ASSERT_TRUE(reader.read_attribute("c", c, false));
    ASSERT_EQ(1, c);

    int d = 0;
    ASSERT_TRUE(reader.read_attribute("d", d, false));
    ASSERT_EQ(-1, d);

    bool e = false;
    ASSERT_TRUE(reader.read_attribute("e", e, false));
    ASSERT_TRUE(e == true);

    bgra f("0xFF00AA42");
    ASSERT_TRUE(reader.read_attribute("f", f, false));
    ASSERT_TRUE(bgra("0xFFFFFFFF") == f);
}

void test_read_of_element_2(const iattributereader& reader)
{
    string a;
    ASSERT_TRUE(reader.read_attribute("a", a, false));
    ASSERT_EQ("solosnake", a);

    float b = 1.0f;
    ASSERT_TRUE(reader.read_attribute("b", b, false));
    ASSERT_EQ(100000.0f, b);

    float c = 0.0f;
    ASSERT_TRUE(reader.read_attribute("c", c, false));
    ASSERT_EQ(1.0f, c);

    int d = 0;
    ASSERT_TRUE(reader.read_attribute("d", d, false));
    ASSERT_EQ(-1000000, d);

    bool e = true;
    ASSERT_TRUE(reader.read_attribute("e", e, false));
    ASSERT_TRUE(e == false);

    unsigned short f;
    ASSERT_TRUE(reader.read_attribute("f", f, false));
    ASSERT_EQ(123, f);

    // Non existent optional element.
    unsigned short g;
    ASSERT_FALSE(reader.read_attribute("g", g, true));
}

class test_elements_reader : public ixmlelementreader
{
public:
    test_elements_reader() : ixmlelementreader("elements_group"), element_counter_(0)
    {
    }

private:
    virtual bool read_from_element(const TiXmlElement& xmlElement)
    {
        xmlattributereader reader(xmlElement);

        if (0 == element_counter_)
        {
            test_read_of_element_1(reader);
        }
        else if (1 == element_counter_)
        {
            test_read_of_element_2(reader);
        }
        else
        {
            ss_throw("Unexpected number of elements read.");
        }

        ++element_counter_;
        return true;
    }

private:
    unsigned int element_counter_;
};
}

TEST(attributereader, xml)
{
    auto file = create_test_file(SampleXML, "SampleXML.txt");

    EXPECT_TRUE(file->is_open());
    file->close();

    test_elements_reader fileReader;
    ixmlelementreader::read("SampleXML.txt", fileReader);
}

TEST(attributereader, tsv)
{
    auto file = create_test_file(SampleTSV, "SampleTSV.txt");

    EXPECT_TRUE(file->is_open());
    tsv tsvFromFile(*file);
    tsvattributereader tsvReader(tsvFromFile);

    EXPECT_TRUE(tsvReader.line_count() == 3);
    EXPECT_TRUE(tsvReader.current_line() == 0);

    tsvReader.set_line(1);
    test_read_of_element_1(tsvReader);

    tsvReader.set_line(2);
    test_read_of_element_2(tsvReader);

    // Cannot set read line 3.
    EXPECT_THROW(tsvReader.set_line(3));
}
