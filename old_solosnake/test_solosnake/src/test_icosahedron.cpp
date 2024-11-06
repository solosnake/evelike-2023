#define RUN_TESTS

#ifdef RUN_TESTS
#include "solosnake/testing/testing.hpp"
#include <cassert>
#include <tuple>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "solosnake/icosahedron.hpp"
#include "solosnake/quickmodel.hpp"
#include "solosnake/point.hpp"
#include "solosnake/indexed_triangles.hpp"

using namespace solosnake;
using namespace std;

TEST(icosahedron, icosahedron)
{
    try
    {
        indexed_triangles ico = make_icosahedron(1.0f);

        std::ofstream ofile("icosohedron.txt");

        for (size_t i = 0; i < ico.index_count(); ++i)
        {
            auto ix = ico.indices()[i];
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << ico.vertices()[3 * ix + 0] << "f, ";
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << ico.vertices()[3 * ix + 1] << "f, ";
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << ico.vertices()[3 * ix + 2] << "f, // Vertex " << i << " from index "
                  << ico.indices()[i] << "\n";
        }

        ofile.close();

        quickmodel(
            ico.index_count(), ico.indices(), ico.vertex_count(), ico.vertices(), nullptr, 300);
    }
    catch (...)
    {
        ADD_FAILURE("Unexpected exception.");
    }
}
#endif
