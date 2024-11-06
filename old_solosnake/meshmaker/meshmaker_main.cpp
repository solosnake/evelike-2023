#include "meshmaker.hpp"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "solosnake/logging.hpp"

using namespace std;
using namespace solosnake;

namespace
{

    // Returns the folder path to the file.
    static inline string path_to( const string& file )
    {
        const filepath url( file );
        return url.parent_path().string();
    }

    static inline string filename_from_path( const string& path )
    {
        const filepath url( path );
        return url.filename().string();
    }

    static inline string filename_without_ext_from_path( const string& path )
    {
        const filepath url( path );
        return url.stem().string();
    }

    /*
    static boost::program_options::options_description options_desc()
    {
        boost::program_options::options_description desc( "meshmaker" );

        // h, i, o, m, e, t, d, w, x, s, y
        desc.add_options()( "help,h", "show help message" )
                          ( "input,i",  boost::program_options::value<string>(), "3D source file to extract mesh from" )
                          ( "output,o", boost::program_options::value<string>(), "Name to use for output file" )
                          ( "mesh-out-dir,m", boost::program_options::value<string>(),
                            "Mesh output directory; generated mesh files are placed here. When no texture "
                            "output directory is specified, generated textures are placed here too." )
                          ( "model-out-dir,e", boost::program_options::value<string>(),
                            "Model output directory; generated model files are placed here. " )
                          ( "tex-out-dir,t", boost::program_options::value<string>(),
                            "Texture output directory; generated textures files are placed here." )
                          ( "tex-in-dirs,d", boost::program_options::value<vector<string>>(),
                            "Space separated list of directories to use when searching for textures "
                            "linked to by the input files." )
                          ( "window,w", boost::program_options::value<bool>()->default_value( true ),
                            "Opens a window and displays the converted mesh." )
                          ( "3dsmax,x", boost::program_options::value<bool>()->default_value( false ),
                            "Converts 3DS Max coordinates to OpenGL (x=-x, y=z, z=y)." )
                          ( "scale,s", boost::program_options::value<float>()->default_value( 1.0f ),
                            "Scales the mesh by this amount." )
                          ( "ty,y", boost::program_options::value<float>()->default_value( 0.0f ),
                            "Translates the mesh by this amount in Y after scaling." )
                            ;

        return desc;
    }

    static boost::program_options::variables_map parse_args( int argc, char** argv )
    {
        auto desc = options_desc();
        auto parsed = boost::program_options::parse_command_line( argc, argv, desc );
        boost::program_options::variables_map vm;
        boost::program_options::store( parsed, vm );
        boost::program_options::notify( vm );

        if( vm.count( "help" ) || argc == 1 )
        {
            cout << desc;
        }

        return vm;
    }

    template <typename T>
    static T get_option_param( const char* name, boost::program_options::variables_map& vm )
    {
        if( vm.count( name ) )
        {
            return vm[name].as<T>();
        }
        else
        {
            return T();
        }
    }

    */
}

int main( int argc, char** argv )
{
    g_ssLogLvl = SS_LOG_ALL;
    int returnCode = -1;
    struct aiLogStream stream;

    try
    {
        stream = aiGetPredefinedLogStream( aiDefaultLogStream_STDERR, NULL );
        aiAttachLogStream( &stream );

        // boost::program_options::variables_map vm = parse_args( argc, argv );

        string input, output, mesh_out_dir, model_out_dir, tex_out_dir;
        bool showmesh = true;
        bool use3DSMaxCoords = false;
        float scale = 1.0;
        float ty = 0.0f;

        // Look for case where command line contains only the filename.
        // vm always has at least 2 options:
        if ( argc == 2 )
        {
            input = argv[1];
        }
        else
        {
            /*
            input           = filepath( get_option_param<string>( "input", vm ) ).string();
            output          = filepath( get_option_param<string>( "output", vm ) ).string();
            mesh_out_dir    = filepath( get_option_param<string>( "mesh-out-dir", vm ) ).string();
            model_out_dir   = filepath( get_option_param<string>( "model-out-dir", vm ) ).string();
            tex_out_dir     = filepath( get_option_param<string>( "tex-out-dir", vm ) ).string();

            scale           = get_option_param<float>( "scale", vm );
            ty              = get_option_param<float>( "ty", vm );
            showmesh        = get_option_param<bool>( "window", vm );
            use3DSMaxCoords = get_option_param<bool>( "3dsmax", vm );
            */
        }
        
        const float translate[] = { 0.0f, ty, 0.0f };

        // output filename:
        string meshfilename;
        string outname = output.empty() ? input : output;

        if( mesh_out_dir.empty() )
        {
            // no output dir specified, use same path as input + lua.
            meshfilename = outname + "." SS_MESH_EXTENSION;
        }
        else
        {
            // output dir specified, use dir + input name + lua
            auto outpath = filepath( mesh_out_dir ) / ( filename_from_path( outname ) + ".msh" );
            meshfilename = outpath.string();
        }

        if( model_out_dir.empty() )
        {
            model_out_dir = mesh_out_dir;
        }

        // model filename
        string modelfilename;

        if( model_out_dir.empty() )
        {
            // no output dir specified, use same path as input + model.lua.
            modelfilename = outname + "model.lua";
        }
        else
        {
            // output dir specified, use dir + input name + lua
            auto outpath = filepath( model_out_dir ) / ( filename_from_path( outname ) + "model.lua" );
            modelfilename = outpath.string();
        }

        if( tex_out_dir.empty() )
        {
            tex_out_dir = path_to( meshfilename );
        }

        vector<string> texdirs;
        texdirs.push_back(".");

        // Copy users texture directories.
        /*
        auto paramdirs = get_option_param<vector<string>>( "tex-in-dirs", vm );
        for( size_t i = 0; i < paramdirs.size(); ++i )
        {
            texdirs.push_back( paramdirs[i] );
        }
        // Add input last so that users dirs override it
        texdirs.push_back( path_to( input ) );

        // Add output dir too last for displaying meshes.
        texdirs.push_back( tex_out_dir );
        */

        if( output.empty() )
        {
            output = filename_without_ext_from_path( input );
        }

        mesh_making_params params;
        params.inputFileName            = input;
        params.outputMeshFilepath       = meshfilename;
        params.outputModelFilepath      = modelfilename;
        params.outputModelName          = output;
        params.textureSourceDirectories = texdirs;
        params.texturesOutDirectory     = tex_out_dir;
        params.scalingFactor            = scale;
        params.translation[0]           = translate[0];
        params.translation[1]           = translate[1];
        params.translation[2]           = translate[2];
        params.use3DSMaxCoords          = use3DSMaxCoords;

        make_mesh( params, showmesh );
        
        returnCode = 0;
    }
    catch( exception& e )
    {
        ss_err( "Exception, ", e.what() );
    }
    catch( ... )
    {
        ss_err( "Exception, no information" );
    }

    aiDetachAllLogStreams();

    return returnCode;
}
