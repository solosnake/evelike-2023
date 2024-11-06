#ifndef solosnake_tsvattributereader_hpp
#define solosnake_tsvattributereader_hpp

#include <map>
#include <string>
#include "solosnake/iattributereader.hpp"

namespace solosnake
{
    class tsv;

    //! Interface adaptor for a tsv instance. This object requires the tsv
    //! object it is used with to remain in scope. This reader assumes that line
    //! zero of the tsv object represents the column names, and that lines 1...n
    //! represent values of the named columns.
    //! Requires that the TSV has a row of heading names, and that each
    //! tab separated name in this row is unique.
    class tsvattributereader : public iattributereader
    {
    public:

        using iattributereader::read_attribute;

        //! Constructs a reader which treats the zero column of the tsv as
        //! defining the attribute names for the following rows.
        //! The initial current line is set to be #0, the attribute names line,
        //! as it is possible that the tsv contains no data. The tsv is
        //! permitted to be completely empty.
        //! All attribute name strings in row #0 are required to be unique.
        //! Throws row 0 contains strings which are not unique.
        explicit tsvattributereader( const tsv& );

        //! Returns the current line of tab separated values being read from.
        size_t current_line() const;

        //! Returns the total number of lines including the column header line.
        size_t line_count() const;

        //! Sets the line to be read from to @a line. Throws if line number is
        //! out of range.
        void set_line( const size_t line );

        //! Returns true if the tsv contained no data rows. Equivalent to
        //! checking if the line count is 0 or 1.
        bool empty() const;

        //! Reads an attribute associated with the current line. Throws if the line
        //! does not exist.
        bool read_attribute( const char* attribName, std::string&, bool optional ) const override;

    private:

        tsvattributereader( const tsvattributereader& );
        tsvattributereader& operator=( const tsvattributereader& );

    private:

        std::map<std::string, size_t>   attribute_columns_;
        const tsv*                      tsv_;
        size_t                          line_number_;
    };

    //-------------------------------------------------------------------------

    inline size_t tsvattributereader::current_line() const
    {
        return line_number_;
    }
}

#endif
