#ifndef solosnake_iwidgetname_hpp
#define solosnake_iwidgetname_hpp

#include <string>
#include <utility>

namespace solosnake
{
    //! Enforces the constraint that the names must be non whitespace,
    //! and named similarly to C variables.
    class iwidgetname
    {
    public:

        iwidgetname( const std::string& name );

        bool operator==( const std::string& w ) const;

        bool operator==( const iwidgetname& w ) const;

        bool operator==( const char* w ) const;

        bool operator!=( const char* w ) const;

        bool operator!=( const iwidgetname& w ) const;

        bool operator<( const iwidgetname& w ) const;

        bool operator>( const iwidgetname& w ) const;

        operator const std::string& () const;

        const std::string& str() const;

        const char* c_str() const;

    private:

        std::string name_;
    };


    //-------------------------------------------------------------------------


    inline bool iwidgetname::operator==( const std::string& w ) const
    {
        return name_ == w;
    }

    inline bool iwidgetname::operator==( const iwidgetname& w ) const
    {
        return name_ == w.name_;
    }

    inline bool iwidgetname::operator==( const char* w ) const
    {
        return w && ( 0 == name_.compare( w ) );
    }

    inline bool iwidgetname::operator!=( const char* w ) const
    {
        return w && ( 0 != name_.compare( w ) );
    }

    inline bool iwidgetname::operator!=( const iwidgetname& w ) const
    {
        return name_ != w.name_;
    }

    inline bool iwidgetname::operator<( const iwidgetname& w ) const
    {
        return name_ < w.name_;
    }

    inline bool iwidgetname::operator>( const iwidgetname& w ) const
    {
        return name_ > w.name_;
    }

    inline iwidgetname::operator const std::string& () const
    {
        return name_;
    }

    inline const std::string& iwidgetname::str() const
    {
        return name_;
    }

    inline const char* iwidgetname::c_str() const
    {
        return name_.c_str();
    }

    inline bool operator==( const std::string& s, const iwidgetname& w )
    {
        return ( w == s );
    }

    inline bool operator!=( const std::string& s, const iwidgetname& w )
    {
        return ( w != s );
    }
}

#endif
