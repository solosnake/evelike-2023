#ifndef solosnake_refcounted_hpp
#define solosnake_refcounted_hpp
#include <cassert>

namespace solosnake
{
    //! Class for inheriting from for intrusive reference counted heap-allocated
    //! objects. The objects will delete themselves when their reference count
    //! reaches zero (it begins at zero and an increment is thus required when
    //! an instance is created. The owner of the heap allocated object is
    // responsible
    //! for incrementing and decrementing).
    class refcounted
    {
    protected:
        refcounted() : refcount_( 0 )
        {
        }

        virtual ~refcounted()
        {
            // Can only be reached via ref count decrementing or inheritance.
        }

        //! Default implementation calls std delete.
        virtual void deallocate()
        {
            delete this;
        }

    public:
        //! When zero there are no references to the object.
        size_t ref_count() const
        {
            return refcount_;
        }

        void inc_ref()
        {
            ++refcount_;
        }

        void dec_ref()
        {
            assert( refcount_ > 0 );

            if( 0 == --refcount_ )
            {
                destroy();
            }
        }

        //! Constructing one refcounted object from an existing one
        //! should be allowed and will create a duplicate new object
        //! with a ref count of zero.
        refcounted( const refcounted& ) : refcount_( 0 )
        {
        }

    private:
        //! This one makes no sense to allow.
        refcounted& operator=( const refcounted& )
        {
            return *this;
        }

        //! NVI patterns, calls deallocate for us, checks ref is zero.
        void destroy()
        {
            assert( 0 == ref_count() );
            this->deallocate();
        }

    private:
        size_t refcount_;
    };
}

#endif
