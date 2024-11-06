#ifndef solosnake_iskybox_src_hpp
#define solosnake_iskybox_src_hpp

#include <string>
#include <memory>

namespace solosnake
{
    struct cubemap;

    //! An interface used by something which wishes load a mesh via its name but
    //! which does not really care where the mesh really comes from.
    class iskybox_src
    {
    public:
        virtual ~iskybox_src();

        virtual std::shared_ptr<cubemap> get_skybox( const std::string& ) = 0;
    };
}

#endif
