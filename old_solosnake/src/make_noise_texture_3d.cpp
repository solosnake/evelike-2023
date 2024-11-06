#include "solosnake/make_noise_texture_3d.hpp"
#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>

class Perlin
{
public:

    explicit Perlin( unsigned long seed );

    void set_noise_frequency( unsigned int f );

    double noise3( const double v[3] ) const;

private:

    void setup( double v, int& b0, int& b1, double& r0, double& r1 ) const;

    void reset( unsigned int f );

    int random();

private:

    struct triple
    {
        double d3_[3];

        operator const double* () const
        {
            return d3_;
        }

        operator double* ()
        {
            return d3_;
        }

        double mul( double x, double y, double z ) const
        {
            return x * d3_[0] + y * d3_[1] + z * d3_[2];
        }
    };

    unsigned long       seed_;
    unsigned int        frequency_;
    std::mt19937        random_;
    std::vector<int>    p_;
    std::vector<triple> g3_;
};

namespace
{
    inline double lerp( double t, double a, double b )
    {
        return ( a + t * ( b - a ) );
    }

    inline double s_curve( double t )
    {
        return ( t * t * ( 3.0 - 2.0 * t ) );
    }

    inline void normalize3( double v[3] )
    {
        const double s = std::sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
        v[0] /= s;
        v[1] /= s;
        v[2] /= s;
    }
}

Perlin::Perlin( unsigned long seed )
    : seed_( seed )
    , frequency_()
    , random_( seed )
{
    set_noise_frequency( 4u );
}

void Perlin::set_noise_frequency( unsigned int frequency )
{
    reset( frequency );
}

void Perlin::setup( double v, int& b0, int& b1, double& r0, double& r1 ) const
{
    const unsigned int bm = frequency_ - 1u;
    const double size = 4096.0;
    const double td = v + size;
    const int ti = static_cast<int>( td );
    b0 = ti & bm;
    b1 = ( ( ti & bm ) + 1 ) & bm;
    r0 = td - ti;
    r1 = r0 - 1.0;
}

int Perlin::random()
{
    return random_();
}

double Perlin::noise3( const double vec[3] ) const
{
    int bx0;
    int by0;
    int bz0;

    int bx1;
    int by1;
    int bz1;

    double rx0;
    double ry0;
    double rz0;

    double rx1;
    double ry1;
    double rz1;

    setup( vec[0], bx0, bx1, rx0, rx1 );
    setup( vec[1], by0, by1, ry0, ry1 );
    setup( vec[2], bz0, bz1, rz0, rz1 );

    const int b00 = p_[p_[bx0] + by0];
    const int b10 = p_[p_[bx1] + by0];
    const int b01 = p_[p_[bx0] + by1];
    const int b11 = p_[p_[bx1] + by1];

    double t1 = s_curve( rx0 );
    double sy = s_curve( ry0 );
    double sz = s_curve( rz0 );

    double u1 = g3_[b00 + bz0].mul( rx0, ry0, rz0 );
    double v1 = g3_[b10 + bz0].mul( rx1, ry0, rz0 );
    double u2 = g3_[b01 + bz0].mul( rx0, ry1, rz0 );
    double v2 = g3_[b11 + bz0].mul( rx1, ry1, rz0 );
    double u3 = g3_[b00 + bz1].mul( rx0, ry0, rz1 );
    double v3 = g3_[b10 + bz1].mul( rx1, ry0, rz1 );
    double u4 = g3_[b01 + bz1].mul( rx0, ry1, rz1 );
    double v4 = g3_[b11 + bz1].mul( rx1, ry1, rz1 );

    double a1 = lerp( t1, u1, v1 );
    double b1 = lerp( t1, u2, v2 );
    double c1 = lerp( sy, a1, b1 );
    double a2 = lerp( t1, u3, v3 );
    double b2 = lerp( t1, u4, v4 );

    return lerp( sz, c1, lerp( sy, a2, b2 ) );
}

void Perlin::reset( const unsigned int newFrequency )
{
    random_.seed( seed_ );
    frequency_ = newFrequency;

    const unsigned int freqx2 = newFrequency + newFrequency;

    p_.resize( freqx2 + 2u );
    std::iota( p_.begin(), p_.end(), 0 );

    g3_.resize( freqx2 + 2u );

    for( unsigned int i = 0; i < newFrequency; i++ )
    {
        g3_[i][0] = static_cast<double>( ( random() % freqx2 ) - newFrequency ) / newFrequency;
        g3_[i][1] = static_cast<double>( ( random() % freqx2 ) - newFrequency ) / newFrequency;
        g3_[i][2] = static_cast<double>( ( random() % freqx2 ) - newFrequency ) / newFrequency;

        normalize3( g3_[i] );
    }

    for( unsigned int i = 0; i < newFrequency; ++i )
    {
        std::swap( p_[i], p_[random() % newFrequency] );
    }

    for( unsigned int i = 0; i < newFrequency + 2; ++i )
    {
        p_[newFrequency + i] = p_[i];

        g3_[newFrequency + i][0] = g3_[i][0];
        g3_[newFrequency + i][1] = g3_[i][1];
        g3_[newFrequency + i][2] = g3_[i][2];
    }
}

namespace solosnake
{
    std::vector<std::uint8_t> make_noise_texture_3d( power_of_2 size, unsigned int startingFrequency )
    {
        const unsigned int sidelength = size.value();
        std::vector<std::uint8_t> texture_buffer( sidelength * sidelength * sidelength * 4 );

        Perlin perlin( 1234 );

        double amp = 0.5;
        unsigned int frequency = startingFrequency;

        for( unsigned int f = 0; f < 4; ++f )
        {
            const double step = 1.0 / ( sidelength / frequency );

            double ni[3] = { 0.0, 0.0, 0.0 };

            perlin.set_noise_frequency( frequency );

            unsigned char* ptr = texture_buffer.data();

            for( unsigned int i = 0; i < sidelength; ++i )
            {
                for( unsigned int j = 0; j < sidelength; ++j )
                {
                    for( unsigned int k = 0; k < sidelength; ++k )
                    {
                        ptr[f] = static_cast<unsigned char>( ( ( perlin.noise3( ni ) + 1.0 ) * amp ) * 128.0 );

                        ptr += 4;
                        ni[2] += step;
                    }

                    ni[1] += step;
                }

                ni[0] += step;
            }

            frequency *= 2u;
            amp *= 0.5;
        }

        return texture_buffer;
    }
}
