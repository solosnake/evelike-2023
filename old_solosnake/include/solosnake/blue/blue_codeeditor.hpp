#ifndef blue_codeeditor_hpp
#define blue_codeeditor_hpp

#include <memory>
#include "solosnake/utf8text.hpp"
#include "solosnake/blue/blue_compiler.hpp"

namespace blue
{
    //! The code editor class. This encapsulates all the functionality of the code editor, and is
    //! driven by the code editor widget. This is the model, and the widget is the view and
    //! the controller.
    class codeeditor
    {
    public:

        explicit codeeditor( const std::shared_ptr<compiler>& );

        void on_text_received( const wchar_t c );

        //! Modifier flags are from input_event::modifier_type flags.
        void on_key_pressed( const unsigned short k, const unsigned int modifierFlags );

        //! Modifier flags are from input_event::modifier_type flags.
        void on_key_released( const unsigned short k, const unsigned int modifierFlags );

        solosnake::utf8text text() const;

    private:

        std::u16string              text_;
        std::shared_ptr<compiler>   bcc_;
        unsigned int                modifierFlags_;
    };
}

#endif
