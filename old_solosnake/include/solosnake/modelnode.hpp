#ifndef solosnake_modelnode_hpp
#define solosnake_modelnode_hpp

#include <cassert>
#include <vector>
#include <memory>
#include "solosnake/meshhandle.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/unaliased.hpp"
#include "solosnake/meshhandle.hpp"
#include "solosnake/deferred_renderer_meshinstancedata.hpp"

namespace solosnake
{
    class Bgr;
    class modelscene;
    class textureanimationtimeline;

    //! A modelnode renders 0 or more mesh instances (which can be of
    //! different render handles) at a location in the scene. The modelnode
    //! is hierarchical, and can have child modelnodes.
    //!
    //! It stores two possibly shared timelines - these are values used to
    //! drive the emissive texture animation - the value for a given point
    //! in a cyclic timeline is passed to the renderer shader, and where it
    //! closely matches values of texels on the model, an emissive color
    //! is added to the scenes pixel.
    class modelnode
    {
    public:
        modelnode();

        modelnode( const std::string& nodeName, const size_t expectedChildCount );

        modelnode( const std::string& nodeName,
                   const meshhandle_t,
                   const meshinstancedata&,
                   const size_t expectedChildCount = 0 );

        modelnode( const modelnode& );

        modelnode( modelnode&& );

        modelnode& operator=( const modelnode& );

        modelnode& operator=( modelnode && );

        void add_child_node( const modelnode& );

        void add_child_node( modelnode&& );

        void draw_into( modelscene& ) const;

        void draw_into( modelscene&, float const* const parentTransform4x4 ) const;

        void set_all_team_hue_shift( const std::uint8_t );

        void set_emissive_channel_rgba( const size_t channel, const Bgra& );

        void set_all_emissive_channels_colours( const Bgra& col0, const Bgra& col1 );

        void translate( const float* unaliased xyz );

        void rotate_around_x( float r );

        void rotate_around_y( float r );

        void set_y_rotation( float r );

        float rotation_x() const
        {
            return x_axis_rotation_;
        }

        float rotation_y() const
        {
            return y_axis_rotation_;
        }

        const float* location_4x4() const
        {
            return data_.location_4x4();
        }

        float* location_4x4()
        {
            return data_.location_4x4();
        }

        size_t child_count() const
        {
            return children_.size();
        }

        const meshinstancedata& instance_data() const
        {
            return data_;
        }

        meshinstancedata& instance_data()
        {
            return data_;
        }

        modelnode& child( const size_t n )
        {
            return children_[n];
        }

        const modelnode& child( const size_t n ) const
        {
            return children_[n];
        }

    private:
        std::string node_name_;
        std::vector<modelnode> children_;
        std::shared_ptr<textureanimationtimeline> timeline1_;
        std::shared_ptr<textureanimationtimeline> timeline2_;
        float y_axis_rotation_;
        float x_axis_rotation_;
        meshinstancedata data_;
        meshhandle_t handle_;
    };
}

#endif
