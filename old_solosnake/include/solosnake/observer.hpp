#ifndef solosnake_observer_hpp
#define solosnake_observer_hpp

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace solosnake
{
    //! T is the type being observed, E is the type of event it broadcasts.
    template <typename E, typename T> class observer
    {
    public:

        virtual ~observer()
        {
        }

        virtual void on_event( const E&, const T& ) = 0;
    };

    //! Use a callback to bind events from an observed object to call
    //! member functions on a class that does not inherit from observer.
    template <typename E, typename T> class callback : public observer<E, T>
    {
    public:

        callback( const std::function<void( const E&, const T& )>& f ) : f_( f )
        {
        }

        void on_event( const E& e, const T& t )  override
        {
            f_( e, t );
        }

    private:
        std::function<void( const E&, const T& )> f_;
    };

    //! A collection of observers maintained by an observed object.
    //! T is the type being observed, E is the type of event it broadcasts.
    template <typename E, typename T> class observers
    {
        mutable std::vector<std::weak_ptr<observer<E, T>>> observers_;

        observers( const observers& );          // =delete
        observers& operator=( const observers& ); //=delete

    public:
        observers()
        {
        }

        observers( observers&& other ) : observers_( std::move( other.observers_ ) )
        {
        }

        observers& operator=( observers && rhs )
        {
            if( this != &rhs )
            {
                observers_ = std::move( rhs.observers_ );
            }

            return *this;
        }

        void notify_of_event( const E& e, const T& t ) const
        {
            size_t n = observers_.size();
            size_t i = 0;
            while( i < n )
            {
                if( std::shared_ptr<observer<E, T>> p = observers_[i].lock() )
                {
                    p->on_event( e, t );
                    ++i;
                }
                else
                {
                    // Expired observer.
                    --n;
                    observers_[i].swap( observers_[n] );
                    // Resize here as observers going out of scope is probably rare
                    observers_.resize( n );
                }
            }
        }

        void add_observer( std::shared_ptr<observer<E, T>> p )
        {
            const observer<E, T>* pp = p.get();

            if( pp && 
                observers_.cend() == std::find_if( observers_.cbegin(), observers_.cend(), 
                                                   [ = ]( const std::weak_ptr<observer<E, T>>& other ) 
                                                   { return other.lock().get() == pp; } ) )
            {
                observers_.push_back( std::weak_ptr<observer<E, T>>( p ) );
            }
        }

        void remove_observer( std::shared_ptr<observer<E, T>> p )
        {
            const observer<E, T>* pp = p.get();

            auto it = std::remove_if( observers_.begin(),
                                      observers_.end(),
                                      [ = ]( const std::weak_ptr<observer<E, T>>& other )
                                      { return other.expired() || other.lock().get() == pp; } );

            observers_.erase( it, observers_.end() );
        }
    };
}

#endif
