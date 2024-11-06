#include <cassert>
#include <set>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include "solosnake/filepath.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/tsv.hpp"
#include "solosnake/tsvattributereader.hpp"
#include "solosnake/blue/blue_hardpoint.hpp"
#include "solosnake/blue/blue_softpoint.hpp"
#include "solosnake/blue/blue_thruster.hpp"
#include <boost/program_options.hpp>


using namespace std;
using namespace blue;

namespace
{

template <typename T>
static inline int write_component(const solosnake::tsvattributereader& data,
                                  const solosnake::filepath& filename)
{
    const T t(data);
    t.write_as_xml(filename);
    return 0;
}

static void write_manifest(const set<string>& names, const solosnake::filepath& dirpath)
{
    const auto url = dirpath / "manifest.xml";

    ofstream manifestfile(url.string().c_str());

    if (manifestfile.is_open())
    {
        manifestfile << "<?xml version=\"1.0\" encoding=\"UTF-8\" "
                        "standalone=\"yes\" ?>\n";
        manifestfile << "<manifest version=\"1.0\">\n";

        for_each(
          names.cbegin(),
          names.cend(),
          [&](const string& entry){ manifestfile << "<item name=\"" << entry << "\"/>\n"; });

        manifestfile << "</manifest>";
    }
    else
    {
        ss_err("Unable to open ", url.string(), " for writing.");
    }
}

static int write_tsv_contents(const solosnake::tsv& data, const string& dir)
{
    int errors = 0;

    const size_t lines = data.line_count();

    if (lines > 1)
    {
        set<string> allNames;
        set<string> softpointNames;
        set<string> hardpointNames;
        set<string> thrusterNames;

        solosnake::filepath dirpath(dir);

        solosnake::tsvattributereader tsvReader(data);

        // Line 0 is the columns header.
        for (size_t i = 1; i < lines; ++i)
        {
            tsvReader.set_line(i);

            const string component_type = tsvReader.read_attribute("component_type", false);
            const string component_name = tsvReader.read_attribute("name", false) + ".xml";

            if (allNames.count(component_name) > 0)
            {
                ss_err("Duplicate component name: ", component_name, " component skipped.");
                ++errors;
            }
            else
            {
                allNames.insert(component_name);

                if (0 == component_type.compare("hardpoint"))
                {
                    hardpointNames.insert(component_name);
                    errors += write_component<hardpoint>(tsvReader, dirpath / component_name);
                }
                else if (0 == component_type.compare("softpoint"))
                {
                    softpointNames.insert(component_name);
                    errors += write_component<softpoint>(tsvReader, dirpath / component_name);
                }
                else if (0 == component_type.compare("thruster"))
                {
                    thrusterNames.insert(component_name);
                    errors += write_component<thruster>(tsvReader, dirpath / component_name);
                }
                else
                {
                    ss_wrn("Unknown type, line ", i, " ", component_type);
                    ++errors;
                }
            }
        }

        write_manifest(allNames, dirpath);
    }

    return errors;
}
}

int main(int argc, char** argv)
{
    solosnake::g_ssLogLvl = SS_LOG_ALL;
    int returnCode = -1;

    try
    {
        if (argc > 1)
        {
            string dir = argc > 2 ? argv[2] : string();
            ifstream ifile(argv[1]);
            if (ifile.is_open())
            {
                solosnake::tsv data(ifile);
                returnCode = write_tsv_contents(data, dir);
            }
        }
    }
    catch (std::exception& e)
    {
        ss_err(e.what());
    }
    catch (...)
    {
        ss_err("Unknown exception.");
    }

    return returnCode;
}
