#ifndef solosnake_manifest_hpp
#define solosnake_manifest_hpp

#include <vector>
#include <string>
#include "solosnake/filepath.hpp"
#include "solosnake/ixmlelementreader.hpp"

namespace solosnake
{
    //! Represents a list of strings, which are loaded from a manifest.xml file.
    //! Typically this file lists a set of other files to load. It is used within
    //! special directories, e.g. "hardpoints" to list the Hardpoint files to load.
    //!
    //! @code
    //! manifest m;
    //! ixmlelementreader::read( "myfile.xml", m );
    //! @endcode
    class manifest : public ixmlelementreader
    {
    public:

        manifest();

        std::vector<std::string>::const_iterator cbegin() const
        {
            return contents_.cbegin();
        }

        std::vector<std::string>::const_iterator cend() const
        {
            return contents_.cend();
        }

        size_t size() const
        {
            return contents_.size();
        }

        const std::string& operator[]( size_t n ) const
        {
            return contents_[n];
        }

        bool operator==( const manifest& rhs ) const
        {
            return rhs.contents_ == contents_;
        }

        bool operator!=( const manifest& rhs ) const
        {
            return rhs.contents_ != contents_;
        }

        //! Returns true if the manifest's list of strings contains a
        //! string that matches @a s.
        bool contains( const std::string& s ) const;

    private:

        bool can_read_version( const std::string& ) const override;

        bool read_from_element( const TiXmlElement& ) override;

    private:

        std::vector<std::string> contents_;
    };
}

#endif
