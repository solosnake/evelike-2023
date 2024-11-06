//#define RUN_TESTS
#ifdef RUN_TESTS
#include "solosnake/testing/testing.hpp"
#include <cassert>
#include <tuple>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "solosnake/sphere_geometry.hpp"
#include "solosnake/quickmodel.hpp"
#include "solosnake/point.hpp"

using namespace solosnake;
using namespace std;

TEST(sphere, make_sphere_models)
{
    try
    {
        sphere_geometry g(1);

        /*for( size_t i=0; i<g.spheres_count(); ++i )
        {
            quickmodel(
                g.sphere_index_count(i),
                g.sphere_indices(i),
                g.spheres_vertex_count(),
                g.spheres_vertices(),
                nullptr );
        }*/

        std::ofstream ofile("sphere2.txt");

        ofile << "{\n";

        unsigned short maxIndex = 0;

        set<unsigned short> usedIndices;

        for (size_t i = 0; i < g.sphere_index_count(1); i += 3)
        {
            maxIndex = max(g.sphere_indices(1)[i], maxIndex);
            usedIndices.insert(g.sphere_indices(1)[i]);
            ofile << g.sphere_indices(1)[i] << ", ";

            maxIndex = max(g.sphere_indices(1)[i + 1], maxIndex);
            usedIndices.insert(g.sphere_indices(1)[i + 1]);
            ofile << g.sphere_indices(1)[i + 1] << ", ";

            maxIndex = max(g.sphere_indices(1)[i + 2], maxIndex);
            usedIndices.insert(g.sphere_indices(1)[i + 2]);
            ofile << g.sphere_indices(1)[i + 2] << ",\n";
        }

        ofile << "}\n\n{\n";

        for (size_t i = 0; i < min<size_t>(3 * maxIndex, g.spheres_vertex_count()); i += 3)
        {
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << g.spheres_vertices()[i] << "f, ";
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << g.spheres_vertices()[i + 1] << "f, ";
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << g.spheres_vertices()[i + 2] << "f, // " << i << "\n";
        }

        ofile << "}\n// ";

        std::for_each(usedIndices.cbegin(),
                      usedIndices.cend(),
                      [&](unsigned short i)
        { ofile << i << " "; });

        ofile << "\n\nsphere = \n";

        float p0[] = { 1.0f, 0.0f, 0.0f };
        float p1[] = { +0.7071f, +0.0000f, -0.7071f };
        float m1[3];

        solosnake::midpoint3(p0, p1, m1);
        const float length = length3(m1);

        const float inflateToEncloseUnitSphere = 1.0f / length;

        for (size_t i = 0; i < g.sphere_index_count(1); ++i)
        {
            auto ix = g.sphere_indices(1)[i];
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << inflateToEncloseUnitSphere * g.spheres_vertices()[3 * ix + 0] << "f, ";
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << inflateToEncloseUnitSphere * g.spheres_vertices()[3 * ix + 1] << "f, ";
            ofile << setprecision(4) << right << fixed << showpos << showpoint
                  << inflateToEncloseUnitSphere * g.spheres_vertices()[3 * ix + 2]
                  << "f, // Vertex " << i << " from index " << g.sphere_indices(1)[i] << "\n";
        }

        ofile.close();
    }
    catch (...)
    {
        ADD_FAILURE() << "Unexpected exception.";
    }
}

#endif // RUN_TESTS
