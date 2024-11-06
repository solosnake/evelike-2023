#ifndef solosnake_windowchange_hpp
#define solosnake_windowchange_hpp

#include <cassert>
#include <vector>
#include <string>
#include "solosnake/dimension.hpp"
#include "solosnake/fullscreen.hpp"
#include "solosnake/minimised.hpp"

namespace solosnake
{
    class windowchange
    {
    public:

        enum ChangeEventType
        {
            FullscreenChange,
            MinimisedChange,
            UnminimisedChange,
            ResizeChange,
            DragDroppedChange
        };

        explicit windowchange( const FullscreenState& f ) : event_data_( &f ), event_type_( FullscreenChange )
        {
        }

        explicit windowchange( MinimisedState m )
            : event_data_( nullptr ), event_type_( m == Minimised ? MinimisedChange : UnminimisedChange )
        {
        }

        explicit windowchange( const Dimension2d<unsigned int>& d )
            : event_data_( &d ), event_type_( ResizeChange )
        {
        }

        explicit windowchange( const std::vector<std::wstring>& dd )
            : event_data_( &dd ), event_type_( DragDroppedChange )
        {
        }

        ChangeEventType event_type() const
        {
            return event_type_;
        }

        const void* event_data() const
        {
            return event_data_;
        }

        MinimisedState minimisedstate() const
        {
            assert( event_type_ == MinimisedChange || event_type_ == UnminimisedChange );
            return event_type_ == MinimisedChange ? Minimised : NotMinimised;
        }

        const FullscreenState* fullscreenstate() const
        {
            assert( event_type_ == FullscreenChange );
            return reinterpret_cast<const FullscreenState*>( event_data_ );
        }

        const Dimension2d<unsigned int>* resized_dimensions() const
        {
            assert( event_type_ == ResizeChange );
            return reinterpret_cast<const Dimension2d<unsigned int>*>( event_data_ );
        }

        const std::vector<std::wstring>* drag_drops() const
        {
            assert( event_type_ == DragDroppedChange );
            return reinterpret_cast<const std::vector<std::wstring>*>( event_data_ );
        }

    private:
        const void*     event_data_;
        ChangeEventType event_type_;
    };
}

#endif
