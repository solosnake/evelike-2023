#ifndef solosnake_tsv_hpp
#define solosnake_tsv_hpp

#include <iosfwd>
#include <vector>
#include <string>

namespace solosnake
{
    //! Tab-Separated-Value class. Reads in TSV strings from a file.
    //! The TSV values do not have to have the same number of entries per line.
    //! Attempts to query for non-existent values will throw exceptions.
    class tsv
    {
    public:
        tsv();

        //! Reads from an opened files stream.
        explicit tsv( std::ifstream& );

        size_t line_count() const;

        size_t column_count( size_t line ) const;

        bool is_rectangular() const;

        const std::vector<std::string>& line( size_t n ) const;

        std::string entry( size_t row, size_t column ) const;

        void save( std::ofstream& ) const;

    private:
        std::vector<std::vector<std::string>> contents_;
    };
}

#endif
