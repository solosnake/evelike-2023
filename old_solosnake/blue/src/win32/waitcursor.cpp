#include "solosnake/waitcursor.hpp"
#include "solosnake/win32/includewindows.hpp"

namespace solosnake
{
    namespace
    {
        class win32waitcursor : public waitcursor 
        {
        public:
        
            //! Sets the default OS waitcursor for so long as this object is in scope.
            win32waitcursor() : hCursor_( ::SetCursor( ::LoadCursor(NULL, IDC_WAIT) ) )
            {
            }

            //! Restores the previous cursor.
            virtual ~win32waitcursor()
            {
                ::SetCursor( hCursor_ );
            }

        private:

            HCURSOR hCursor_;
        };
    }

    waitcursor::waitcursor()
    {        
    }

    waitcursor::~waitcursor()
    {
    }
    
    //! Returns an operating system specific waitcursor to be used as a sentinel.
    std::unique_ptr<waitcursor> waitcursor::make_sentinel()
    {
        return std::make_unique<win32waitcursor>();
    }
}

