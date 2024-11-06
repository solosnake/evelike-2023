#ifndef blue_datapaths_hpp
#define blue_datapaths_hpp
#include <map>
#include <vector>
#include <string>
#include "solosnake/filepath.hpp"

namespace blue
{
    //! Class that will return guaranteed game data file paths, taking into account multiple 
    //! possible data folders and their hierarchies (to permit modding etc). It assumes that
    //! beneath each root folder lies a known folder hierarchy in which varies types of files
    //! will be found. Some file types assume the folder contains a manifest file detailing the
    //! contents of that folder.
    class datapaths
    {
    public:

        //! datapaths can have multiple paths to many roots. This is intended to
        //! allow for mods etc. The paths are searched in first come first serve 
        //! priority order.
        //! @param roots    A string containing one or more directory paths 
        //!                 specifying root directories to be searched for
        //!                 game data assets. Each root directory should be 
        //!                 separated by the token UO_DATA_ROOT_SEPERATOR.
        explicit datapaths( const std::string& roots );

        solosnake::filepath get_boards_filepath( const std::string& filename ) const;

        solosnake::filepath get_chassis_filepath( const std::string& filename ) const;

        solosnake::filepath get_fonts_filepath( const std::string& filename ) const;

        solosnake::filepath get_gui_filepath( const std::string& filename ) const;

        solosnake::filepath get_hardpoints_filepath( const std::string& filename ) const;

        solosnake::filepath get_models_filepath( const std::string& filename ) const;

        solosnake::filepath get_meshes_filepath( const std::string& filename ) const;

        solosnake::filepath get_scripts_filepath( const std::string& filename ) const;

        solosnake::filepath get_styles_filepath( const std::string& filename ) const;

        solosnake::filepath get_softpoints_filepath( const std::string& filename ) const;

        solosnake::filepath get_sounds_filepath( const std::string& filename ) const;

        solosnake::filepath get_skyboxes_filepath( const std::string& filename ) const;

        solosnake::filepath get_suns_filepath( const std::string& filename ) const;

        solosnake::filepath get_textures_filepath( const std::string& filename ) const;

        solosnake::filepath get_thrusters_filepath( const std::string& filename ) const;

        solosnake::filepath get_translations_filepath( const std::string& filename ) const;

        std::vector<solosnake::filepath> get_chassis_manifests() const;

        std::vector<solosnake::filepath> get_softpoints_manifests() const;

        std::vector<solosnake::filepath> get_hardpoints_manifests() const;

        std::vector<solosnake::filepath> get_thrusters_manifests() const;

        std::vector<solosnake::filepath> get_translations_manifests() const;

    private:
        
        bool try_add_new_data_root( const std::string& rootPath );

    private:

        std::map<std::string, std::vector<solosnake::filepath>> path_lookups_;
    };
}

#endif
