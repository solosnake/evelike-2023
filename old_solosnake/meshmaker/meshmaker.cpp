#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <set>
#include <map>
#include <sstream>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/image.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/load_meshes.hpp"
#include "solosnake/make_normalmap.hpp"
#include "meshmaker.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/show_meshes.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/vec3.hpp"

using namespace std;
using namespace solosnake;

// Diffuse texture is created with these dimensions when not available:
#define SS_DEFAULT_TEX_WIDTH (512)
#define SS_DEFAULT_TEX_HEIGHT (512)

namespace
{
    struct textures_written;
    struct indent;
    struct emissive_names;

    //-------------------------------------------------------------------------

    aiString guess_texture_name( const char*, const string& );
    
    aiString guess_texture_name( const aiString&, const string& );

    aiString append_to_filename( const char*, const char* );

    string get_or_make_unique_name( const string&, const unsigned int, const string& );

    filepath try_find_file( const aiString&, const vector<string>& );

    string copyfile( const filepath&, const string& );

    bool change_extension_to_bmp_from( aiString&, const string&, ostringstream& );

    aiString change_extension_to_bmp( const aiString&, ostringstream& );

    void clean_string( aiString& );

    void show_output( const string&, const float );

    void write_child_nodes( const aiNode*,
                            const mesh_making_params&,
                            const map<unsigned int, string>&,
                            const map<string, shared_ptr<textures_written>>&,
                            const size_t,
                            ostream& );

    //-------------------------------------------------------------------------

    struct indent
    {
        indent( size_t n ) : spaces( n )
        {
        }
        size_t spaces;
    };

    struct textures_written
    {
        string outputDiffAndSpecName;
        string outputNormalsAndEmissName;
        string texWriteReport;
    };

    struct emissive_names
    {
        explicit emissive_names( const char* inputDiffTexName )
        {
            emissive1[0]      = guess_texture_name( inputDiffTexName, "_E" );
            emissive1[1]      = guess_texture_name( inputDiffTexName, "_E1" );
            emissive2         = guess_texture_name( inputDiffTexName, "_E2" );
            emissive_anim1[0] = guess_texture_name( inputDiffTexName, "_EA" );
            emissive_anim1[1] = guess_texture_name( inputDiffTexName, "_EA1" );
            emissive_anim2    = guess_texture_name( inputDiffTexName, "_EA2" );
        }

        aiString emissive1[2];
        aiString emissive2;
        aiString emissive_anim1[2];
        aiString emissive_anim2;
    };

    //-------------------------------------------------------------------------

    //!
    void show_output( const string& convertedFileName,
                      const float scale )
    {
        const char* name = convertedFileName.c_str();
        solosnake::show_meshes( 1, &name, scale );
    }

    //!
    aiString guess_texture_name( const char* s, const string& replacement)
    {
        string texname = s;

        auto pos = texname.find( "_C" );

        if( pos != string::npos )
        {
            texname.replace( pos, 2, replacement );
            return aiString( texname );
        }
        else 
        {
            pos = texname.find_last_of( "." );

            if( pos != string::npos )
            {
                texname.insert( pos, replacement );
                return aiString( texname );
            }
        }

        return aiString();
    }

    //!
    aiString append_to_filename( const char* f, const char* x )
    {
        string filename = f;
        auto pos = filename.find_last_of( "." );
        if( pos != string::npos )
        {
            filename.insert( pos, x );
            return aiString( filename );
        }
        else
        {
            return aiString( filename + x );
        }
    }

    //!
    aiString guess_texture_name( const aiString& s, const string& replacement )
    {
        return guess_texture_name( s.C_Str(), replacement );
    }

    //!
    string get_or_make_unique_name( const string& outname,
                                    const unsigned int n,
                                    const string& meshname )
    {
        static set<string> usedNames;

        if( usedNames.empty() )
        {
            // Ensure used names contains the invalid empty string.
            // This will force empty mesh names to be discarded.
            usedNames.insert( string( "" ) );
        }

        ostringstream ss;
        ss << outname << setw( 2 ) << setfill( '0' ) << n << meshname;
        string uniqueMeshName = ss.str();

        if( usedNames.count( uniqueMeshName ) != 0 )
        {
            static unsigned int meshCounter = 0;

            // This name is already used. Create a new one and record it.
            ss.clear();
            ss << uniqueMeshName << "_" << setw( 4 ) << setfill( '0' ) << meshCounter++;
            uniqueMeshName = ss.str();
            usedNames.insert( uniqueMeshName );
        }

        return uniqueMeshName;
    }

    //!
    string make_diffuse_and_spec_texname( const string& meshname,
                                          const size_t n )
    {
        if( n == 0 )
        {
            return meshname + "_diff_spec.bmp";
        }
        else
        {
            ostringstream ss;
            ss << meshname << '_' << n << "_diff_spec.bmp";
            return ss.str();
        }
    }

    //!
    string make_normals_and_emiss_texname(
        const string& meshname, size_t n )
    {
        if( n == 0 )
        {
            return meshname + "_norm_emiss.bmp";
        }
        else
        {
            ostringstream ss;
            ss << meshname << '_' << n << "_norm_emiss.bmp";
            return ss.str();
        }
    }

    //!
    filepath try_find_file( const aiString& filename,
                            const vector<string>& texdirs )
    {
        const filepath texname( filename.C_Str() );

        if( filename.length > 0 )
        {
            if( ! filepath_exists( texname ) )
            {
                for( size_t i = 0; i < texdirs.size(); ++i )
                {
                    const auto newpath = filepath( texdirs[i] ) / texname;
                    ss_log( "Checking for ", newpath.string() );
                    if( filepath_exists( newpath ) )
                    {
                        ss_log( newpath.string(), " found" );
                        return newpath;
                    }
                }
            }
        }

        return texname;
    }

    //!
    string copyfile( const filepath& from, const string& to )
    {
        ostringstream report;

        const filepath topath = filepath( to ) / from.filename();

        if( from != topath )
        {
            ifstream fin( from.string(), std::ios::binary | std::ios::in );

            if( fin.is_open() )
            {
                ofstream fout( topath.string(), ios::binary | ios::trunc | ios::out );
                fout << fin.rdbuf();
                report << "-- Copied " << from.string() << " to " << topath.string() << '\n';
            }
            else
            {
                report << "-- Unable to open " << from.string() << '\n';
            }
        }

        return report.str();
    }

    //!
    bool change_extension_to_bmp_from( aiString& filename,
                                       const string& ext,
                                       ostringstream& report )
    {
        string f = filename.C_Str();

        string EXT = ext;
        transform( EXT.begin(), EXT.end(), EXT.begin(), ::toupper );

        auto e = f.rfind( string( "." ) + EXT );
        if( e != string::npos )
        {
            f.replace( e, 4, ".bmp" );
            report << "-- Renamed " << filename.C_Str() << " to " << f << "\n";

            filename = aiString( f );
            return true;
        }
        else
        {
            e = f.rfind( string( "." ) + ext );
            if( e != string::npos )
            {
                f.replace( e, 4, ".bmp" );
                report << "-- Renamed " << filename.C_Str() << " to " << f << "\n";
                filename = aiString( f );
                return true;
            }
        }

        return false;
    }

    //!
    aiString change_extension_to_bmp( const aiString& filename,
                                      ostringstream& report )
    {
        aiString f = filename;

        if( !change_extension_to_bmp_from( f, "png", report ) )
        {
            if( !change_extension_to_bmp_from( f, "jpg", report ) )
            {
                change_extension_to_bmp_from( f, "tga", report );
            }
        }

        return f;
    }

    //!
    bool open_img(
        aiString filename,
        const vector<string>& texdirs,
        image& img,
        ostringstream& report )
    {
        filename = change_extension_to_bmp( filename, report );

        // Always widen images to RGBA
        if( !img.open( try_find_file( filename, texdirs ), true ) )
        {
            report << "-- Unable to read '" << filename.C_Str() << "'\n";
            return false;
        }

        return true;
    }

    //!
    bool open_img( const aiString& filename,
                   const vector<string>& texdirs,
                   image& img,
                   unsigned int& diffwidth,
                   unsigned int& diffheight,
                   ostringstream& report )
    {
        if( open_img( filename, texdirs, img, report ) )
        {
            if( img.width() != diffwidth || img.height() != diffheight )
            {
                report << "-- " << filename.C_Str()
                       << " dimensions are different to diffuse texture'\n";

                return false;
            }

            return true;
        }

        return false;
    }

    //! Creates and saves a new RGBA diffuse + specular BMP.
    //! dest.RGB = diff.RGB
    //! dest.A = spec.R
    string make_diff_and_spec( const std::string& outputDiffTeamName,
                               const aiString& srcDiffuseTexName,
                               const aiString& srcSpecTexName,
                               const string& texoutdir,
                               const vector<string>& texdirs,
                               unsigned int& diffwidth,
                               unsigned int& diffheight )
    {
        // Write a report as a Lua comment about any problems
        // during texture creation.
        ostringstream report;

        try
        {
            image diffin, specin;
            const bool imageOpened = open_img( srcDiffuseTexName, texdirs, diffin, report );

            if( imageOpened )
            {
                diffwidth = diffin.width();
                diffheight = diffin.height();
                report << "-- Diffuse texture " << diffwidth << " x " << diffheight << "\n";
            }

            if( !imageOpened || srcDiffuseTexName.length == 0 )
            {
                report << "-- Diffuse texture name is empty'\n";
                report << "-- Creating grey diffuse texture\n";
                diffwidth = SS_DEFAULT_TEX_WIDTH;
                diffheight = SS_DEFAULT_TEX_HEIGHT;
                diffin = image( diffwidth, diffheight, image::format_bgra );

                // 255 alpha needed here!!! Or black fucking textures show up.
                diffin.clear_to_bgra( 255, 255, 255, 255 );

                const bgra lightgrey( 195, 195, 195, 255 );

                for( size_t i = 0; i < SS_DEFAULT_TEX_HEIGHT; ++i )
                {
                    for( size_t j = i % 2; j < SS_DEFAULT_TEX_WIDTH; j += 2 )
                    {
                        diffin.set_pixel( j, i, lightgrey );
                    }
                }

                diffin.save( "diffin.bmp" );
            }

            if( srcSpecTexName.length > 0 )
            {
                if( open_img( srcSpecTexName, texdirs, specin, diffwidth, diffheight, report ) )
                {
                    // If we loaded a specular, copy it to the alpha channel of
                    // the diff image.
                    image::copy_layer( specin, 0, diffin, 3 );
                }
                else
                {
                    report << "-- Unable to read '" << srcSpecTexName.C_Str() << "'\n";
                }
            }

            diffin.flip_vertically();
            if( !diffin.save( outputDiffTeamName ) )
            {
                report << "-- Unable to write " << outputDiffTeamName << "'\n";
            }

            report << copyfile( outputDiffTeamName, texoutdir );
        }
        catch( ... )
        {
            report << "-- Exception while trying to make diff/team BMP\n";
        }

        assert( diffwidth != 0 );
        assert( diffheight != 0 );

        return report.str();
    }

    //!
    string make_normals_and_emissives(
        const std::string& normalsAndSpecName,
        const aiString& inputNormalstTex,
        const aiString& inputEmissTex,
        const mesh_making_params& params,
        unsigned int diffWidth,
        unsigned int diffHeight )
    {
        // Write a report as a Lua comment about any problems
        // during texture creation.
        ostringstream report;

        try
        {
            const std::string emissiveName( inputEmissTex.C_Str() );

            aiString emissives[3];

            if( !emissiveName.empty() )
            {
                emissives[0] = aiString( emissiveName );
                emissives[1] = append_to_filename( emissiveName.c_str(), "1" );
                emissives[2] = append_to_filename( emissiveName.c_str(), "2" );
            }

            image normalstex;
            image emisstex[2];

            if( diffHeight == 0 || diffWidth == 0 )
            {
                diffHeight = SS_DEFAULT_TEX_HEIGHT;
                diffWidth = SS_DEFAULT_TEX_WIDTH;
            }

            if( inputNormalstTex.length != 0 && open_img( inputNormalstTex,
                                                          params.textureSourceDirectories,
                                                          normalstex,
                                                          diffWidth,
                                                          diffHeight,
                                                          report ) )
            {
                normalstex.flip_vertically();
            }
            else
            {
                // Fake a tangent space normal map with just a plain purple texture.
                report << "-- Faking normal map\n";
                normalstex = image( static_cast<unsigned int>( diffWidth ),
                                    static_cast<unsigned int>( diffHeight ),
                                    image::format_bgra );
                normalstex.clear_to_bgra( 255, 128, 128, 0 );
            }

            // Load or make Emissives

            size_t loadedEmissives = 0;
            for( size_t i = 0u; i < 3u; ++i )
            {
                if( loadedEmissives < 2u )
                {
                    if( emissives[i].length > 0 && open_img( emissives[i],
                                                             params.textureSourceDirectories,
                                                             emisstex[loadedEmissives],
                                                             diffWidth,
                                                             diffHeight,
                                                             report ) )
                    {
                        emisstex[loadedEmissives].flip_vertically();
                        if( normalstex.width() != emisstex[loadedEmissives].width()
                                || normalstex.height() != emisstex[loadedEmissives].height() )
                        {
                            report << "-- Normal and emissive textures are not "
                                   "same dimensions.\n";

                            report << "-- Normal W x H ("
                                   << normalstex.width()
                                   << " x "
                                   << normalstex.height()
                                   << ")\n";

                            report << "-- Emissive W x H ("
                                   << emisstex[loadedEmissives].width()
                                   << " x "
                                   << emisstex[loadedEmissives].height()
                                   << ")\n";

                            ss_throw( "Normal and emissive textures are not "
                                      "same dimensions." );
                        }
                        ++loadedEmissives;
                    }
                }
            }

            // If we did not open two emissive files.
            if( loadedEmissives < 2u )
            {
                for( size_t i = loadedEmissives; i < 2; ++i )
                {
                    report << "-- Creating black emissive " << i << " texture.'\n";
                    emisstex[i] = image( diffWidth, diffHeight, image::format_bgra );
                    emisstex[1].clear_to_bgra( 0, 0, 0, 0 );
                }
            }

            // Blend the inputNormalstTex and inputSpecTex together:
            const unsigned int texW = normalstex.width();
            const unsigned int texH = normalstex.height();

            assert( normalstex.format() == image::format_bgra );

            for( unsigned int h = 0; h < texH; ++h )
            {
                for( unsigned int w = 0; w < texW; ++w )
                {
                    auto normlpix = image::pixel( w, h, normalstex, image::format_bgra );
                    auto e0 = image::pixel( w, h, emisstex[0], image::format_bgra );
                    auto e1 = image::pixel( w, h, emisstex[1], image::format_bgra );

                    // Omit the B channel (always positive) and store only the
                    // G and R channels.
                    normlpix[0] = normlpix[1];
                    normlpix[1] = normlpix[2];
                    normlpix[2] = e0[0];
                    normlpix[3] = e1[0];
                }
            }

            // Write inputNormalstTex/specular file.
            report << "-- Creating inputNormalstTex/inputSpecTex texture '" << normalsAndSpecName
                   << "'\n";

            // Save inputNormalstTex
            if( !normalstex.save( normalsAndSpecName ) )
            {
                report << "-- Unable to write " << normalsAndSpecName << "\n";
            }

            report << copyfile( normalsAndSpecName, params.texturesOutDirectory.string() );
        }
        catch( ... )
        {
            report << "-- Exception while trying to make "
                   "inputNormalstTex/specular BMP\n";
        }

        return report.str();
    }

    //!
    void clean_string( aiString& s )
    {
        string str( s.C_Str() );
        auto pos1 = str.find_first_of( "-" );
        auto pos2 = str.find_first_of( "#" );
        auto pos = pos1 < pos2 ? pos1 : pos2;

        if( pos != string::npos )
        {
            str = str.substr( 0, pos );

            // Remove trailing whitespace(s).
            while( str.length() > 0 && ( ( str.back() == ' ' ) || ( str.back() == '\t' ) ) )
            {
                str = str.substr( 0, str.length() - 1 );
            }

            s = aiString( str.c_str() );
        }

        // Remove any paths.
        const filepath pathname( str );
        str = pathname.filename().string();
        s = aiString( str.c_str() );
    }

    //!
    string write_textures(
        const aiString& inputDiffTexName,
        const std::string& outputDiffAndSpecPath,
        const std::string& outputNormalsAndEmissPath,
        const mesh_making_params& params )
    {
        // Write a report as a Lua comment about any problems
        // during texture creation.
        ostringstream report;

        aiString inputTeamTex;
        aiString inputNormalMapTex;
        aiString inputSpecMapTex;
        aiString inputEmissMapTex;

        if( inputDiffTexName.length == 0 )
        {
            ss_err( "No diffuse texture found in material." );
            report << "-- ERROR: No diffuse texture found in material.\n";
        }
        else
        {
            inputEmissMapTex  = guess_texture_name( inputDiffTexName, "_E" );
            inputNormalMapTex = guess_texture_name( inputDiffTexName, "_N" );
            inputSpecMapTex   = guess_texture_name( inputDiffTexName, "_S" );
        }

        // These are read and filled in by make_diffteam.
        unsigned int diffwidth = 0;
        unsigned int diffheight = 0;

        // Fills in the diff texture width and height so all outputs will
        // use the same sizes.
        report << make_diff_and_spec( outputDiffAndSpecPath,
                                      inputDiffTexName,
                                      inputSpecMapTex,
                                      params.texturesOutDirectory.string(),
                                      params.textureSourceDirectories,
                                      diffwidth,
                                      diffheight );

        report << make_normals_and_emissives( outputNormalsAndEmissPath,
                                              inputNormalMapTex,
                                              inputEmissMapTex,
                                              params,
                                              diffwidth,
                                              diffheight );

        return report.str();
    }

    //!
    ostream& operator<<( ostream& os, const indent& i )
    {
        size_t n = i.spaces;
        while( n > 0 )
        {
            --n;
            os << '\t';
        }

        return os;
    }

    //! Assumes a matrix containing only a rotation and translation: @a scale is
    //! applied only to the transformation in @a m4x4.
    void write_transform_matrix(
        const aiMatrix4x4& m4x4,
        const float scale,
        indent in,
        ostream& os )
    {
        os << in
           << "transform = { "
           << m4x4.a1 << ", "
           << m4x4.b1 << ", "
           << m4x4.c1 << ", "
           << m4x4.d1 << ", "
           << m4x4.a2 << ", "
           << m4x4.b2 << ", "
           << m4x4.c2 << ", "
           << m4x4.d2 << ", "
           << m4x4.a3 << ", "
           << m4x4.b3 << ", "
           << m4x4.c3 << ", "
           << m4x4.d3 << ", "
           << ( m4x4.a4 * scale ) << ", "
           << ( m4x4.b4 * scale ) << ", "
           << ( m4x4.c4 * scale ) << ", "
           << m4x4.d4
           << " },\n";
    }

    void write_node( const string& meshname, indent in, ostream& os )
    {
        os << in << "mesh = \"" << meshname << "\",\n";
    }

    void write_identity_matrix( indent in, ostream& os )
    {
        os << in
           << "transform = { "
           "1.0, 0.0, 0.0, 0.0, "
           "0.0, 1.0, 0.0, 0.0, "
           "0.0, 0.0, 1.0, 0.0, "
           "0.0, 0.0, 0.0, 1.0 },\n";
    }

    void recursively_write_node(
        const aiNode* sceneNode,
        const mesh_making_params& params,
        const map<unsigned int, string>& meshnames,
        const map<string, shared_ptr<textures_written>>& meshTexturesWritten,
        size_t r,
        ostream& modelsFile )
    {
        if( 0 == sceneNode->mNumMeshes && 0 == sceneNode->mNumChildren )
        {
            // Write nothing.
            return;
        }
        else
        {
            modelsFile << indent( r ) << "nodename = \""
                       << sceneNode->mName.C_Str() << "\",\n";

            if( sceneNode->mNumMeshes > 0 )
            {
                const unsigned int nodeMeshIndex = sceneNode->mMeshes[0];
                const string meshName = meshnames.at( nodeMeshIndex );

                write_node( meshName, r, modelsFile );

                write_transform_matrix( sceneNode->mTransformation,
                                        params.scalingFactor,
                                        r,
                                        modelsFile );
            }
            else
            {
                write_node( string(), r, modelsFile );

                write_identity_matrix( r, modelsFile );
            }

            if( sceneNode->mNumMeshes == 1 && sceneNode->mNumChildren == 0 )
            {
                // Special case for neater files:
                modelsFile << indent( r ) << "childnodes = {}\n";
            }
            else
            {
                // Begin child nodes...
                modelsFile << indent( r ) << "childnodes =\n" << indent( r ) << "{\n";

                // Write accompanying meshes as faked child nodes without children.
                if( sceneNode->mNumMeshes > 1 )
                {
                    for( unsigned int i = 1; i < sceneNode->mNumMeshes; ++i )
                    {
                        modelsFile << indent( r + 1 ) << "{\n";
                        const unsigned int nodeMeshIndex = sceneNode->mMeshes[0];
                        const string meshName = meshnames.at( nodeMeshIndex );
                        write_node( meshName, r + 2, modelsFile );
                        write_identity_matrix( r + 2, modelsFile );
                        modelsFile << indent( r + 2 ) << "childnodes = {},\n";
                        modelsFile << indent( r + 1 ) << "},\n";
                    }
                }

                // Write real child nodes:
                write_child_nodes( sceneNode, params, meshnames, meshTexturesWritten, r + 1, modelsFile );

                // End child nodes.
                modelsFile << indent( r ) << "}\n";
            }
        }
    }

    void write_child_nodes(
        const aiNode* sceneNode,
        const mesh_making_params& params,
        const map<unsigned int, string>& meshNames,
        const map<string, shared_ptr<textures_written>>& meshTexturesWritten,
        const size_t r,
        ostream& modelsFile )
    {
        for( unsigned int i = 0; i < sceneNode->mNumChildren; ++i )
        {
            const aiNode* child = sceneNode->mChildren[i];
            if( child->mNumChildren > 0 || child->mNumMeshes > 0 )
            {
                modelsFile << indent( r ) << "{\n";

                recursively_write_node( sceneNode->mChildren[i],
                                        params,
                                        meshNames,
                                        meshTexturesWritten,
                                        r + 1,
                                        modelsFile );

                modelsFile << indent( r ) << "},\n";
            }
        }
    }

    //! Checks for special case where n is node with no children and one mesh.
    bool isLeafMeshNode( const aiNode* n )
    {
        return ( n->mNumMeshes == 1 && n->mNumChildren == 0 );
    }

    //! If this node has no mesh and has a lone mesh leaf node as a child,
    //! returns it, else returns nullptr.
    const aiNode* getSoloLeaf( const aiNode* n )
    {
        if( n->mNumMeshes == 0 && n->mNumChildren > 0 )
        {
            const aiNode* leaf = nullptr;

            for( unsigned int i = 0; i < n->mNumChildren; ++i )
            {
                if( isLeafMeshNode( n->mChildren[i] ) )
                {
                    if( leaf == nullptr )
                    {
                        // Leaf found.
                        leaf = n->mChildren[i];
                    }
                    else
                    {
                        // More than one possible leaf, return nullptr.
                        return nullptr;
                    }
                }
            }

            return leaf;
        }

        return nullptr;
    }

    //! Returns a child node which has no siblings with data - it can be "hoisted"
    //! up, removing a link in the chain.
    const aiNode* getHoistableNode( const aiNode* n )
    {
        if( n->mNumMeshes == 0 )
        {
            if( n->mNumChildren == 1 )
            {
                return n->mChildren[0];
            }
            else
            {
                // More than one child, check to see if only one has meshes
                // or children.
                const aiNode* interestingChild = nullptr;
                for( unsigned int i = 0; i < n->mNumChildren; ++i )
                {
                    if( n->mChildren[i]->mNumChildren > 0 || n->mChildren[i]->mNumMeshes > 0 )
                    {
                        if( interestingChild )
                        {
                            // More than one interesting child found.
                            return nullptr;
                        }
                        else
                        {
                            interestingChild = n->mChildren[i];
                        }
                    }
                }

                return interestingChild;
            }
        }
        else
        {
            // Has meshes so not hoistable.
            return nullptr;
        }
    }

    //!
    void write_herarchical_model(
        const aiScene* scene,
        const mesh_making_params& params,
        const map<unsigned int, string>& meshNames,
        const map<string, shared_ptr<textures_written>>& meshTexturesWritten,
        ostream& modelsFile )
    {
        modelsFile << "-- Generated from "
                   << params.inputFileName
                   << "\nmodel {\nname = \""
                   << params.outputModelName
                   << "\",\nmeshfiles = { \""
                   << params.outputModelName
                   << "." SS_MESH_EXTENSION "\" },\n";

        aiNode const* const root = scene->mRootNode;

        if( const aiNode* soloLeaf = getSoloLeaf( root ) )
        {
            recursively_write_node( soloLeaf, params, meshNames, meshTexturesWritten, 0, modelsFile );
        }
        else if( const aiNode* hoistableNode = getHoistableNode( root ) )
        {
            recursively_write_node(
                hoistableNode, params, meshNames, meshTexturesWritten, 0, modelsFile );
        }
        else
        {
            write_node( "", 0, modelsFile );
            write_transform_matrix( root->mTransformation, params.scalingFactor, 0, modelsFile );

            // Begin child nodes...
            if( root->mNumChildren > 0 )
            {
                modelsFile << "childnodes =\n{\n";
                write_child_nodes( root, params, meshNames, meshTexturesWritten, 1, modelsFile );
                modelsFile << "},\n";
            }
            else
            {
                modelsFile << "childnodes = {}\n";
            }
        }

        modelsFile << "}\n";
    }

    //!
    shared_ptr<textures_written> write_material_textures( const aiMaterial* material,
                                                          const size_t texSeriesNumber,
                                                          const mesh_making_params& params )
    {
        shared_ptr<textures_written> written = make_shared<textures_written>();

        aiString inputDiffTexName;
        aiGetMaterialString( material, AI_MATKEY_TEXTURE_DIFFUSE( 0 ), &inputDiffTexName );

        // Some vendors append stuff after filename. Remove this.
        clean_string( inputDiffTexName );

        // Create textures names.
        written->outputDiffAndSpecName =
            make_diffuse_and_spec_texname( params.outputModelName, texSeriesNumber );

        const auto outputDiffAndSpecPath =
            filepath( params.textureSourceDirectories.at( 0 ) ) / filepath( written->outputDiffAndSpecName );

        written->outputNormalsAndEmissName =
            make_normals_and_emiss_texname( params.outputModelName, texSeriesNumber );

        const auto outputNormalsAndSpecPath =
            filepath( params.textureSourceDirectories.at( 0 ) ) / filepath( written->outputNormalsAndEmissName );

        written->texWriteReport += write_textures( inputDiffTexName,
                                                   outputDiffAndSpecPath.string(),
                                                   outputNormalsAndSpecPath.string(),
                                                   params );

        return written;
    }

    //!
    meshdata make_mesh( const textures_written& written,
                        const string& meshname,
                        const aiMesh* assimpMesh,
                        const aiMaterial* const,
                        const mesh_making_params& params )
    {
        meshdata md;

        const unsigned int vertCount = assimpMesh->mNumVertices;

        md.name = meshname;
        md.diffAndSpecName = written.outputDiffAndSpecName;
        md.normalsAndEmissName = written.outputNormalsAndEmissName;
        md.vertices.reserve( vertCount * 3u );
        md.indices.reserve( assimpMesh->mNumFaces * 3u );
        md.uvs.reserve( vertCount * 2u );
        md.smoothednormals.reserve( vertCount * 3u );
        md.tangents.reserve( vertCount * 3u );

        const float scale = params.scalingFactor;
        const float tx = params.translation[0];
        const float ty = params.translation[1];
        const float tz = params.translation[2];

        // 3DS Max to OpenGL : (x, y, z) -> (-x, z, y) 
        const bool max3DS = params.use3DSMaxCoords;

        for( unsigned int i = 0; i < vertCount; ++i )
        {
            auto x = scale * assimpMesh->mVertices[i].x;
            auto y = scale * assimpMesh->mVertices[i].y;
            auto z = scale * assimpMesh->mVertices[i].z;

            if( max3DS ) 
            {
                x = -x;
                std::swap( y, z );
            }

            md.vertices.push_back( tx + x );
            md.vertices.push_back( ty + y );
            md.vertices.push_back( tz + z );
        }

        for( unsigned int i = 0; i < assimpMesh->mNumFaces; ++i )
        {
            if( assimpMesh->mFaces[i].mNumIndices != 3 )
            {
                ss_throw( "Non triangular face" );
            }
            md.indices.push_back( assimpMesh->mFaces[i].mIndices[0] );
            md.indices.push_back( assimpMesh->mFaces[i].mIndices[1] );
            md.indices.push_back( assimpMesh->mFaces[i].mIndices[2] );
        }

        if( assimpMesh->mTextureCoords[0] == nullptr )
        {
            for( unsigned int i = 0; i < vertCount; ++i )
            {
                // THESE ARE NOT UVs!!! THESE ARE FAKED UVs FOR 
                // WHEN NO COORDS ARE PRESENT.
                md.uvs.push_back( md.vertices[i] );
                md.uvs.push_back( md.vertices[i] );
            }
        }
        else
        {
            for( unsigned int i = 0; i < vertCount; ++i )
            {
                md.uvs.push_back( assimpMesh->mTextureCoords[0][i].x );
                md.uvs.push_back( assimpMesh->mTextureCoords[0][i].y );
            }
        }

        for( unsigned int i = 0; i < vertCount; ++i )
        {
            auto x = ( assimpMesh->mNormals[i].x );
            auto y = ( assimpMesh->mNormals[i].y );
            auto z = ( assimpMesh->mNormals[i].z );

            if( max3DS ) 
            {
                x = -x;
                std::swap( y, z );
            }

            md.smoothednormals.push_back( x );
            md.smoothednormals.push_back( y );
            md.smoothednormals.push_back( z );
        }

        // Write tangents.
        for( unsigned int i = 0; i < vertCount; ++i )
        {
            const bool hasTangents = assimpMesh->mTangents != nullptr;

            if( hasTangents )
            {                
                auto x = assimpMesh->mTangents[i].x;
                auto y = assimpMesh->mTangents[i].y;
                auto z = assimpMesh->mTangents[i].z;

                if( max3DS ) 
                {
                    x = -x;
                    std::swap( y, z );
                }

                // http://www.gamasutra.com/view/feature/129939/messing_with_tangent_space.php?page=4
                md.tangents.push_back( x );
                md.tangents.push_back( y );
                md.tangents.push_back( z );
            }
            else
            {
                md.tangents.push_back( 1.0f );
                md.tangents.push_back( 0.0f );
                md.tangents.push_back( 0.0f );
            }
        }

        return move( md );
    }

    //!
    void write_meshes( const aiScene* scene,
                       const mesh_making_params& params,
                       const string& meshesfilename,
                       ostream& modelsFile )
    {
        // Create a unique name for each scene mesh by index.
        map<unsigned int, string> meshnames;
        for( unsigned int i = 0; i < scene->mNumMeshes; ++i )
        {
            meshnames[i] = get_or_make_unique_name( params.outputModelName,
                                                    i,
                                                    scene->mMeshes[i]->mName.C_Str() );
        }

        vector<meshdata> meshdatas;
        meshdatas.reserve( scene->mNumMeshes );

        map<string, shared_ptr<textures_written>> meshTexturesWritten;

        // We batch by material, as each mesh must share a material.
        for( unsigned int materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex )
        {
            bool texturesWrittenForThisMaterial = false;
            size_t texSeriesCounter = 0;
            shared_ptr<textures_written> materialTextures;

            for( unsigned int i = 0; i < scene->mNumMeshes; ++i )
            {
                if( scene->mMeshes[i]->mMaterialIndex == materialIndex )
                {
                    if( !texturesWrittenForThisMaterial )
                    {
                        // Only write one set of textures per material.
                        assert( !materialTextures );
                        materialTextures = write_material_textures(
                                               scene->mMaterials[materialIndex], texSeriesCounter, params );
                        ++texSeriesCounter;
                        texturesWrittenForThisMaterial = true;
                    }

                    assert( materialTextures );
                    meshTexturesWritten[meshnames[i]] = materialTextures;

                    meshdatas.emplace_back( make_mesh( *materialTextures,
                                                       meshnames[i],
                                                       scene->mMeshes[i],
                                                       scene->mMaterials[materialIndex],
                                                       params ) );
                }
            }

            if( materialTextures )
            {
                clog << materialTextures->texWriteReport;
            }
            else
            {
                clog << "No textures written.";
            }
        }

        save_mesh_datas( meshesfilename, meshdatas );

        write_herarchical_model( scene, params, meshnames,
                                 meshTexturesWritten, modelsFile );
    }

    //!
    void convert_asset_to_mesh( const mesh_making_params& params,
                                const string& meshesfilename,
                                ostream& modelfile )
    {
        Assimp::Importer importer;

        // This guarantees we have one triangular faces.
        const int typeToRemove = aiPrimitiveType_LINE | aiPrimitiveType_POINT;
        importer.SetPropertyInteger( AI_CONFIG_PP_SBP_REMOVE, typeToRemove );

        const unsigned int importerFlags = aiProcessPreset_TargetRealtime_MaxQuality;
        auto scene = importer.ReadFile( params.inputFileName, importerFlags );

        if( nullptr != scene )
        {
            if( 0 == ( scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ) )
            {
                write_meshes( scene, params, meshesfilename, modelfile );
            }
            else
            {
                ss_err( params.inputFileName, " scene incomplete." );
            }
        }
        else
        {
            ss_err( params.inputFileName, " ", importer.GetErrorString() );
        }
    }

} // Anonymous namespace

namespace solosnake
{
    //! Main driver function.
    void make_mesh( const mesh_making_params& params, const bool showmesh )
    {
        ofstream modelfile( params.outputModelFilepath.string() );

        if( modelfile.is_open() )
        {
            convert_asset_to_mesh( params, params.outputMeshFilepath.string(), modelfile );
            modelfile.close();

            if( showmesh )
            {
                show_output( params.outputMeshFilepath.string(), params.scalingFactor );
            }
        }
        else
        {
            if( !modelfile.is_open() )
            {
                ss_err( "Cannot open ", params.outputModelFilepath.string(), " for writing." );
            }

            ss_throw( "Cannot open output file for writing" );
        }
    }
}
