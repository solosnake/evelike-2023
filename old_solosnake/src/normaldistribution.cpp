#include <algorithm>
#include <cmath>
#include <random>
#include "solosnake/normaldistribution.hpp"

using namespace std;

namespace solosnake
{
    vector<float> normaldistribution( const unsigned int n, const float mean, const float stddev )
    {
        vector<float> numbers;
        numbers.reserve( n );

        random_device rd;
        mt19937 generator( rd() );

        normal_distribution<float> distribution( mean, stddev );

        for( unsigned int i = 0; i < n; ++i )
        {
            numbers.push_back( distribution( generator ) );
        }

        sort( numbers.begin(), numbers.end() );

        return numbers;
    }
}
