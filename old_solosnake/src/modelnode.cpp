#include <algorithm>
#include "solosnake/logging.hpp"
#include "solosnake/modelnode.hpp"
#include "solosnake/modelscene.hpp"
#include "solosnake/textureanimationtimeline.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
    modelnode::modelnode()
        : node_name_()
        , children_()
        , timeline1_()
        , timeline2_()
        , y_axis_rotation_( 0.0f )
        , x_axis_rotation_( 0.0f )
        , data_()
        , handle_()
    {
    }

    modelnode::modelnode( const std::string& nodeName, const size_t expectedChildCount )
        : node_name_( nodeName )
        , children_()
        , timeline1_()
        , timeline2_()
        , y_axis_rotation_( 0.0f )
        , x_axis_rotation_( 0.0f )
        , data_()
        , handle_()
    {
        children_.reserve( expectedChildCount );
    }

    modelnode::modelnode( const std::string& nodeName,
                          const meshhandle_t h,
                          const meshinstancedata& data,
                          const size_t expectedChildCount )
        : node_name_( nodeName )
        , children_()
        , y_axis_rotation_( 0.0f )
        , x_axis_rotation_( 0.0f )
        , data_( data )
        , handle_( h )
    {
        children_.reserve( expectedChildCount );
    }

    modelnode::modelnode( const modelnode& rhs )
        : node_name_( rhs.node_name_ )
        , children_( rhs.children_ )
        , timeline1_( rhs.timeline1_ )
        , timeline2_( rhs.timeline2_ )
        , y_axis_rotation_( rhs.y_axis_rotation_ )
        , x_axis_rotation_( rhs.x_axis_rotation_ )
        , data_( rhs.data_ )
        , handle_( rhs.handle_ )
    {
    }

    modelnode::modelnode( modelnode&& rhs )
        : node_name_( move( rhs.node_name_ ) )
        , children_( move( rhs.children_ ) )
        , timeline1_( move( rhs.timeline1_ ) )
        , timeline2_( move( rhs.timeline2_ ) )
        , y_axis_rotation_( rhs.y_axis_rotation_ )
        , x_axis_rotation_( rhs.x_axis_rotation_ )
        , data_( rhs.data_ )
        , handle_( rhs.handle_ )
    {
    }

    modelnode& modelnode::operator=( const modelnode& rhs )
    {
        if( this != &rhs )
        {
            node_name_ = rhs.node_name_;
            children_ = rhs.children_;
            timeline1_ = rhs.timeline1_;
            timeline2_ = rhs.timeline2_;
            y_axis_rotation_ = rhs.y_axis_rotation_;
            x_axis_rotation_ = rhs.x_axis_rotation_;
            data_ = rhs.data_;
            handle_ = rhs.handle_;
        }

        return *this;
    }

    modelnode& modelnode::operator=( modelnode && rhs )
    {
        if( this != &rhs )
        {
            node_name_ = move( rhs.node_name_ );
            children_ = move( rhs.children_ );
            timeline1_ = move( rhs.timeline1_ );
            timeline2_ = move( rhs.timeline2_ );
            y_axis_rotation_ = rhs.y_axis_rotation_;
            x_axis_rotation_ = rhs.x_axis_rotation_;
            data_ = rhs.data_;
            handle_ = rhs.handle_;
        }

        return *this;
    }

    void modelnode::translate( const float* unaliased xyz )
    {
        data_.translate( xyz );
    }

    void modelnode::rotate_around_x( float r )
    {
        x_axis_rotation_ = fmod( x_axis_rotation_ + r, SS_TWOPI );
    }

    void modelnode::rotate_around_y( float r )
    {
        y_axis_rotation_ = fmod( y_axis_rotation_ + r, SS_TWOPI );
    }

    void modelnode::set_y_rotation( float r )
    {
        y_axis_rotation_ = fmod( r, SS_TWOPI );
    }

    void modelnode::add_child_node( const modelnode& n )
    {
        children_.push_back( n );
    }

    void modelnode::add_child_node( modelnode&& n )
    {
        children_.push_back( std::move( n ) );
    }

    void modelnode::draw_into( modelscene& s ) const
    {
        const float identity[] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
                                 };

        draw_into( s, identity );
    }

    void modelnode::draw_into( modelscene& s, float const* const parentTransform4x4 ) const
    {
        meshinstancedata instanceData( data_, x_axis_rotation_, y_axis_rotation_, parentTransform4x4 );

        // Draw the optional instance at this location.
        if( handle_ != meshhandle_t() )
        {
            s.add_mesh_instance( handle_, instanceData );
        }

        // Draw the children of this node:
        for( size_t i = 0; i < children_.size(); ++i )
        {
            children_[i].draw_into( s, instanceData.location_4x4() );
        }
    }

    void modelnode::set_emissive_channel_rgba( const size_t channel, const bgra& c )
    {
        data_.set_emissive_channel_bgra( channel, c );

        for( size_t i = 0; i < children_.size(); ++i )
        {
            children_[i].set_emissive_channel_rgba( channel, c );
        }
    }

    void modelnode::set_all_team_hue_shift( const std::uint8_t shifted )
    {
        data_.set_team_hue_shift( shifted / 255.0f );

        for( size_t i = 0; i < children_.size(); ++i )
        {
            children_[i].set_all_team_hue_shift( shifted );
        }
    }

    void modelnode::set_all_emissive_channels_colours( const bgra& col0, const bgra& col1 )
    {
        data_.set_emissive_channels_bgra( col0, col1 );

        for( size_t i = 0; i < children_.size(); ++i )
        {
            children_[i].set_all_emissive_channels_colours( col0, col1 );
        }
    }
}
