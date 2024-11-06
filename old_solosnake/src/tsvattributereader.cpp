#include "solosnake/tsvattributereader.hpp"
#include "solosnake/tsv.hpp"
#include "solosnake/throw.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace
{
    map<string, size_t> make_column_names( const solosnake::tsv& t )
    {
        map<string, size_t> headings;

        if( t.line_count() > 0 )
        {
            const auto line = t.line( 0 );
            for( size_t i = 0; i < line.size(); ++i )
            {
                headings.insert( make_pair( line[i], i ) );
            }
        }

        return headings;
    }
}

namespace solosnake
{
    tsvattributereader::tsvattributereader( const tsv& t )
        : attribute_columns_( make_column_names( t ) )
        , tsv_( &t )
        , line_number_( 0 )
    {
    }

    bool tsvattributereader::read_attribute( const char* attribName,
                                             string& value,
                                             bool optional ) const
    {
        bool found_and_read = false;

        const auto it = attribute_columns_.find( string( attribName ) );

        if( it != attribute_columns_.end() )
        {
            if( it->second < tsv_->column_count( line_number_ ) )
            {
                value = tsv_->entry( line_number_, it->second );
                found_and_read = true;
            }
        }

        if( !optional && !found_and_read )
        {
            ss_err( "Required TSV attribute missing: ", attribName );
            ss_throw( "Required TSV attribute not found." );
        }

        return found_and_read;
    }

    size_t tsvattributereader::line_count() const
    {
        return tsv_->line_count();
    }

    void tsvattributereader::set_line( const size_t line )
    {
        if( line < line_count() )
        {
            line_number_ = line;
        }
        else
        {
            ss_throw( "tsvattributereader 'set_line' value out of range." );
        }
    }
}
