#ifndef solosnake_waitcursor_hpp
#define solosnake_waitcursor_hpp

#include <memory>

namespace solosnake
{
    //! This class sets a waitcursor as the cursor so long as it is in scope.
    class waitcursor 
    {
    public:
        
        //! Sets the default OS waitcursor for so long as this object is in scope.
        waitcursor();

        //! Restores the previous cursor.
        virtual ~waitcursor() = 0;
        
        waitcursor(const waitcursor&) = delete;

        waitcursor& operator= (const waitcursor&) = delete;
        
        //! Returns an operating system specific waitcursor to be used as a sentinel.
        static std::unique_ptr<waitcursor> make_sentinel();
    };

}

#endif 
