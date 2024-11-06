#ifndef solosnake_show_console_hpp
#define solosnake_show_console_hpp

namespace solosnake
{
    //! If no console is present, this call will show it and tie stderr and stdout to it.
    //! Printing to stderr and stdout should appear in this window. It is safe to call this 
    //! repeatedly, no more than one console will ever appear.
    void show_console();
}

#endif 
