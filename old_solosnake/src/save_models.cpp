#include "solosnake/save_models.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/deferred_renderer.hpp"
#include <ostream>
#include <iomanip>
#include <sstream>

using namespace std;

namespace solosnake
{

#define PRINT_FLOAT(F)                                                           \
    std::setw(12) << std::right << std::noshowpos << std::showpoint              \
                  << static_cast<float>((F))

#define PRINT_UINT(I) \
    std::setw(12) << std::right << static_cast<unsigned int>((I))

    writetexinfo::writetexinfo( const std::string& name,
                                const imeshtexturecoords& tex,
                                const std::string& diffuse_and_team,
                                const std::string& normals_with_occlusion_and_specular,
                                const std::string& emissive )
        : name_( name )
        , tex_( tex )
        , diffuse_and_team_( diffuse_and_team )
        , normals_with_occlusion_and_specular_( normals_with_occlusion_and_specular )
        , emissive_( emissive )
    {
    }

    // Declarations.
    ostream& operator<<( ostream& os, const imeshgeometry& g );
    ostream& operator<<( ostream& os, const writetexinfo& tinfo );
    
    ostream& operator<<( ostream& os, const imeshgeometry& g )
    {
        if( 0 != g.index_count() % 3 )
        {
            ss_throw( "Triangles mode index list is not a multiple of 3." );
        }

        ostringstream ss;

        ss << "mesh {\n\tname = \"" << g.name() << "\",\n\tcontents = \"";

        if( g.index_mode() == imeshgeometry::TrianglesMode )
        {
            ss << "Triangles";
        }
        else
        {
            ss << "Strips";
        }

        ss << "\",\n\tvertices =\n\t{\n";

        const float* vb = g.vertices();
        const size_t nv = 3 * g.vertex_count();

        for( size_t i = 0; i < nv; i += 3 )
        {
            ss << "\t\t" << PRINT_FLOAT( vb[i + 0] ) << ", " << PRINT_FLOAT( vb[i + 1] ) << ", "
               << PRINT_FLOAT( vb[i + 2] ) << ",\n";
        }

        ss << "\t},\n\tindices =\n\t{";

        const unsigned short* ib = g.indices();
        const unsigned int ni = g.index_count();

        if( g.index_mode() == imeshgeometry::TrianglesMode )
        {
            for( unsigned int i = 0; i < ni; i += 3 )
            {
                ss << "\n\t\t" << PRINT_UINT( ib[i + 0] ) << ", " << PRINT_UINT( ib[i + 1] ) << ", "
                   << PRINT_UINT( ib[i + 2] ) << ",";
            }
        }
        else
        {
            static const unsigned int row_count = 5;
            unsigned int rows = ni / row_count;

            for( unsigned int i = 0; i < rows; ++i )
            {
                ss << "\n\t\t";

                for( unsigned int r = 0; r < row_count; ++r )
                {
                    ss << PRINT_UINT( ib[row_count * i + r] ) << ", ";
                }
            }

            if( rows * row_count < ni )
            {
                ss << "\n\t\t";

                for( unsigned int i = rows * row_count; i < ni; ++i )
                {
                    ss << PRINT_UINT( ib[i] ) << ", ";
                }
            }
        }

        ss << "\n\t},\n}\n";

        os << ss.str();

        return os;
    }

    ostream& operator<<( ostream& os, const writetexinfo& tinfo )
    {
        ostringstream ss;

        ss << "textureinfo {\n\tname = \"" << tinfo.name_ << "\",\n\ttexture_coords = \n\t{";
        const float* tb = tinfo.tex_.texture_coords();
        const unsigned int nt = tinfo.tex_.texture_coords_count();

        const unsigned int row_count = 3;
        unsigned int rows = nt / row_count;

        for( unsigned int i = 0; i < rows; ++i )
        {
            ss << "\n\t\t";

            for( unsigned int r = 0; r < row_count; ++r )
            {
                ss << PRINT_FLOAT( tb[2 * row_count * i + r + 0] ) << ", ";
                ss << PRINT_FLOAT( tb[2 * row_count * i + r + 1] ) << ",   ";
            }
        }

        if( rows * row_count < nt )
        {
            ss << "\n\t\t";

            for( unsigned int i = rows * row_count; i < nt; ++i )
            {
                ss << PRINT_FLOAT( tb[2 * row_count * i + 0] ) << ", ";
                ss << PRINT_FLOAT( tb[2 * row_count * i + 1] ) << ",   ";
            }
        }

        ss << "\n\t},\n\tdiffuse_and_team       = \"" << tinfo.diffuse_and_team_
           << "\",\n\tnormals_with_occlusion_and_specular = \""
           << tinfo.normals_with_occlusion_and_specular_ << "\",\n\temissive  = \"" << tinfo.emissive_
           << "\",\n}";

        os << ss.str();

        return os;
    }
}
