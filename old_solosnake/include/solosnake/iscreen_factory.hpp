#ifndef solosnake_iscreen_factory_hpp
#define solosnake_iscreen_factory_hpp
#include <memory>

namespace solosnake
{
    class iscreen;
    class nextscreen;

    //! The screen factory creates a screen object of the concrete type
    //! for the matching name.
    class iscreen_factory
    {
    public:

        virtual ~iscreen_factory();
        
        //! Sets a wait cursor and then calls the virtual create method.
        std::unique_ptr<iscreen> create_screen( const nextscreen& ) const;

    private:

        virtual std::unique_ptr<iscreen> do_create_screen( const nextscreen& ) const = 0;
    };
}

#endif
