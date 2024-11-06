namespace blue
{
    inline void drawmachine::draw_into( solosnake::modelscene& s ) const
    {
        visiblemachine_.node_.draw_into( s, transform4x4_ );
    }

    inline const float* drawmachine::location_4x4() const
    {
        return transform4x4_;
    }

    inline float* drawmachine::location_4x4()
    {
        return transform4x4_;
    }

    inline const Machine* drawmachine::get_machine() const
    {
        return machine_;
    }

    inline MachineReadout drawmachine::visiblemachine::emissive_intensity_src( unsigned int i, unsigned int j ) const
    {
        assert( i < 2u && j < 2u );
        return static_cast<MachineReadout>( channels_.emissive_readouts[i][j] );
    }

    inline solosnake::bgra drawmachine::visiblemachine::emissive_colour( unsigned int i ) const
    {
        assert( i < 2u );
        return channels_.emissive_colours[i];
    }

    inline void drawmachine::update_location( const Positional_update& posInfoEvent )
    {
        previous_position_ = current_position_;
        current_position_  = posInfoEvent.machine_position_info_;
        position_interpolation_ = 0.0f;
    }
}

