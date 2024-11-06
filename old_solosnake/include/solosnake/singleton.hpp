#ifndef solosnake_singleton_hpp
#define solosnake_singleton_hpp

#include <cassert>
#include <memory>
#include <stdexcept>

namespace solosnake
{
    template <typename T> class singleton
    {
    public:
        static T* instance()
        {
            assert( singleton<T>::single().get() );
            return singleton<T>::single().get();
        }

        static void instance( std::shared_ptr<T> p )
        {
            singleton<T>::single() = p;
        }

    private:
        static std::shared_ptr<T>& single()
        {
            static std::shared_ptr<T> t_;
            return t_;
        }
    };
}

#endif
