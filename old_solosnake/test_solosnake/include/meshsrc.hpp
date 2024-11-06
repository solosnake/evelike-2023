#ifndef test_solosnake_meshsrc_hpp
#define test_solosnake_meshsrc_hpp

#include "solosnake/deferred_renderer_types.hpp"

//! Class used when testing meshes and mesh related types.
class meshsrc : public solosnake::imesh
{
public:
    meshsrc(std::shared_ptr<solosnake::imeshgeometry> g,
            std::shared_ptr<solosnake::imeshtextureinfo> t)
        : geometry_(g), texInfo_(t)
    {
    }

    virtual ~meshsrc() SS_NOEXCEPT
    {
    }

    virtual std::string get_mesh_name() const
    {
        return geometry_->name();
    }

    virtual std::shared_ptr<solosnake::imeshgeometry> get_meshgeometry()
    {
        return geometry_;
    }

    virtual std::shared_ptr<solosnake::imeshtextureinfo> get_meshgeometrytexinfo()
    {
        return texInfo_;
    }

private:
    std::shared_ptr<solosnake::imeshgeometry> geometry_;
    std::shared_ptr<solosnake::imeshtextureinfo> texInfo_;
};

#endif