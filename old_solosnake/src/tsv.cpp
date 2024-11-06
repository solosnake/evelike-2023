#include "solosnake/tsv.hpp"
#include "solosnake/throw.hpp"
#include <fstream>
using namespace std;

namespace solosnake
{
    namespace
    {
        vector<string> break_line_into_values( const string& line )
        {
            vector<string> strings;

            if( line.length() > 0 )
            {
                strings.reserve( 32 );

                string::size_type start = 0;
                string::size_type end = line.find( "\t", start );

                do
                {
                    strings.push_back(
                        line.substr( start, ( end == string::npos ) ? string::npos : end - start ) );
                    start = ( end == string::npos ) ? string::npos : end + 1;
                    end = line.find( "\t", start );
                }
                while( start != string::npos && start < line.length() );
            }

            return strings;
        }
    }

    tsv::tsv()
    {
    }

    size_t tsv::line_count() const
    {
        return contents_.size();
    }

    size_t tsv::column_count( size_t n ) const
    {
        return line( n ).size();
    }

    bool tsv::is_rectangular() const
    {
        for( size_t i = 1; i < line_count(); ++i )
        {
            if( column_count( i - 1 ) != column_count( i ) )
            {
                return false;
            }
        }

        return true;
    }

    const vector<string>& tsv::line( size_t n ) const
    {
        return contents_.at( n );
    }

    string tsv::entry( size_t row, size_t column ) const
    {
        return line( row ).at( column );
    }

    tsv::tsv( ifstream& s )
    {
        assert( s.is_open() );
        if( s.is_open() )
        {
            string rowline;
            rowline.reserve( 512 );
            contents_.reserve( 64 );
            while( s.good() )
            {
                getline( s, rowline );
                auto values = break_line_into_values( rowline );
                if( !values.empty() )
                {
                    contents_.push_back( break_line_into_values( rowline ) );
                }
            }
        }
    }

    void tsv::save( ofstream& os ) const
    {
        const size_t rows = line_count();
        for( size_t row = 0; row < rows; ++row )
        {
            const size_t cols = column_count( row );
            for( size_t col = 0; col < cols; ++col )
            {
                os << line( row ).at( col );

                if( ( col + 1 ) < cols )
                {
                    os << "\t";
                }
            }

            if( ( row + 1 ) < rows )
            {
                os << "\n";
            }
        }
    }
}