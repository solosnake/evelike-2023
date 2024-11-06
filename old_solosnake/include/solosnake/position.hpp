#ifndef solosnake_position_hpp
#define solosnake_position_hpp

namespace solosnake
{
    //! Class used to strongly indicate a 2D position.
    template <typename T> class position2d
    {
    public:
        position2d() : x_( T() ), y_( T() )
        {
        }

        position2d( T xx, T yy ) : x_( xx ), y_( yy )
        {
        }

        T x() const
        {
            return x_;
        }

        T y() const
        {
            return y_;
        }

        bool operator==( const position2d<T>& rhs ) const
        {
            return x_ == rhs.x_ && y_ == rhs.y_;
        }

        bool operator!=( const position2d<T>& rhs ) const
        {
            return x_ != rhs.x_ || y_ != rhs.y_;
        }

    private:
        T x_;
        T y_;
    };

    typedef position2d<float> position2df;
    typedef position2d<int> position2di;
}

#endif
