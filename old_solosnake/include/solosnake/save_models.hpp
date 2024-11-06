#ifndef solosnake_save_models_hpp
#define solosnake_save_models_hpp
#include <iosfwd>
#include <string>

namespace solosnake
{
    class imeshgeometry;
    class imeshtexturecoords;

    //! Intended as a shim class to force the user to add the required missing
    // information
    //! allowing the imeshtextureinfo to be saved to file (the names of its
    // textures).
    struct writetexinfo
    {
        writetexinfo( const std::string& name,
                      const imeshtexturecoords& tex,
                      const std::string& diffuse_and_team,
                      const std::string& normals_with_occlusion_and_specular,
                      const std::string& emissive );

        const std::string name_;
        const imeshtexturecoords& tex_;
        const std::string diffuse_and_team_;
        const std::string normals_with_occlusion_and_specular_;
        const std::string emissive_;

    private:
        writetexinfo& operator=( const writetexinfo& );
    };
}

#endif
