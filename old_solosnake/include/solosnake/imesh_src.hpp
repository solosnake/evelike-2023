#ifndef solosnake_imesh_src_hpp
#define solosnake_imesh_src_hpp

#include <string>
#include <memory>

namespace solosnake
{
    class imesh;

    //! An interface used by something which wishes load a mesh via its name but
    //! which does not really care where the mesh really comes from.
    class imesh_src
    {
    public:
        virtual ~imesh_src();

        virtual std::shared_ptr<imesh> get_imesh( const std::string& ) = 0;
    };
}

#endif
