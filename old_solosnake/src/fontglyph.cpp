#include "solosnake/fontglyph.hpp"

namespace solosnake
{
    namespace
    {
        inline texquad zerotexquad()
        {
            texquad tq;
            tq.x0 = 0.0f;
            tq.y0 = 0.0f;
            tq.x1 = 0.0f;
            tq.y1 = 0.0f;
            return tq;
        }
    }

    fontglyph::fontglyph()
        : texCoords_( zerotexquad() )
        , pixelWidth_( 0 )
        , pixelHeight_( 0 )
        , pixelAdvanceX_( 0 )
        , pixelOffsetY_( 0 )
    {
    }

    fontglyph::fontglyph( const texquad& texCoords,
                          unsigned int pixelWidth,
                          unsigned int pixelHeight,
                          unsigned int pixelAdvanceX,
                          int pixelOffsetY )
        : texCoords_( texCoords )
        , pixelWidth_( pixelWidth )
        , pixelHeight_( pixelHeight )
        , pixelAdvanceX_( pixelAdvanceX )
        , pixelOffsetY_( pixelOffsetY )
    {
    }

    bool fontglyph::operator==( const fontglyph& rhs ) const
    {
        return ( texCoords_ == rhs.texCoords_ ) && ( pixelWidth_ == rhs.pixelWidth_ )
               && ( pixelHeight_ == rhs.pixelHeight_ ) && ( pixelAdvanceX_ == rhs.pixelAdvanceX_ )
               && ( pixelOffsetY_ == rhs.pixelOffsetY_ );
    }
}
