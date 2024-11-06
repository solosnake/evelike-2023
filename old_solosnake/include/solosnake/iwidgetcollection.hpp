#ifndef solosnake_iwidgetcollection_hpp
#define solosnake_iwidgetcollection_hpp

#include <list>
#include <memory>
#include "solosnake/iwidget.hpp"

namespace solosnake
{
    class iwidget;

    //! Container type to which widgets cannot be added or removed.
    //! Internally the widgets will be sorted by z layer order, highest first.
    //! All widgets in a collection are guaranteed to have unique names. This
    //! is required to meaningfully search for a named widget.
    class iwidgetcollection
    {
    public:

        typedef std::list<std::shared_ptr<iwidget>>::iterator iterator;

        typedef std::list<std::shared_ptr<iwidget>>::const_iterator const_iterator;

        iwidgetcollection();

        explicit iwidgetcollection( std::list<std::shared_ptr<iwidget>>&& w );

        iwidgetcollection( iwidgetcollection&& );

        iwidgetcollection& operator=( iwidgetcollection && );

        iterator begin();

        const_iterator cbegin() const;

        iterator end();

        const_iterator cend() const;

        size_t size() const;

        size_t empty() const;

        void swap( iwidgetcollection& );

    private:

        std::list<std::shared_ptr<iwidget>> widgets_;
    };


    //-------------------------------------------------------------------------


    inline iwidgetcollection::iwidgetcollection() : widgets_()
    {
    }

    inline iwidgetcollection::iterator iwidgetcollection::begin()
    {
        return widgets_.begin();
    }

    inline iwidgetcollection::const_iterator iwidgetcollection::cbegin() const
    {
        return widgets_.cbegin();
    }

    inline iwidgetcollection::iterator iwidgetcollection::end()
    {
        return widgets_.end();
    }

    inline iwidgetcollection::const_iterator iwidgetcollection::cend() const
    {
        return widgets_.cend();
    }

    inline size_t iwidgetcollection::size() const
    {
        return widgets_.size();
    }

    inline size_t iwidgetcollection::empty() const
    {
        return widgets_.empty();
    }

    inline void iwidgetcollection::swap( iwidgetcollection& w )
    {
        widgets_.swap( w.widgets_ );
    }

    inline iwidgetcollection::iwidgetcollection( iwidgetcollection&& rhs )
        : widgets_( std::move( rhs.widgets_ ) )
    {
    }

    inline iwidgetcollection& iwidgetcollection::operator=( iwidgetcollection && rhs )
    {
        if( this != &rhs )
        {
            widgets_ = std::move( rhs.widgets_ );
        }

        return *this;
    }
}

#endif
