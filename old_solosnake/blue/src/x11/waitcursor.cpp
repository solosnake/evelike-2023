#include "solosnake/waitcursor.hpp"
#include "solosnake/x11/x11.hpp"

namespace solosnake
{
    namespace
    {
        class x11waitcursor : public waitcursor 
        {
        public:
        
            //! Sets the default OS waitcursor for so long as this object is in scope.
            x11waitcursor()
            {
            }

            //! Restores the previous cursor.
            virtual ~x11waitcursor()
            {                
            }

        private:

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
        return std::make_unique<x11waitcursor>();
    }
}

