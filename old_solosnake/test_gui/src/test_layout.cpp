#include "solosnake/testing/testing.hpp"
#include "solosnake/layout.hpp"
#include "solosnake/iwidget.hpp"
#include <memory>
#include <string>

using namespace solosnake;

namespace
{
#define WIDGET_Z 0
#define BACK_Z  -1

    iwidget::KeyboardFocusing kbf_from_bool( const bool b )
    {
        return b ? iwidget::YesKeyBoardFocus : iwidget::NoKeyBoardFocus;
    }

    const std::string wname0( "WIDGET0" );
    const std::string wname1( "WIDGET1" );
    const std::string BackgroundName( "BACK" );

    // Implements pure virtuals as no op calls.
    class do_nothing_widget : public iwidget
    {
    protected:

        do_nothing_widget( const std::string& name,
                           const solosnake::rect& r,
                           const bool acceptsKeyboardFocus,
                           const int z,
                           const int tabValue )
            : iwidget( widgetparams( name, r, z, tabValue )
            , kbf_from_bool( acceptsKeyboardFocus )
            , iwidget::SharedPtrOnly() )
        {
        }

        void on_wheelmoved_inside( solosnake::screenxy, float ) override
        {
        }

        void on_wheelmoved_outside( solosnake::screenxy, float ) override
        {
        }

        void on_cursor_enter( screenxy, cursorbuttons_state ) override
        {
        }

        void on_cursor_moved( screenxy, cursorbuttons_state ) override
        {
        }

        void on_cursor_exit( screenxy, cursorbuttons_state ) override
        {
        }

        ButtonPressOutcome on_button_pressed( bool inside, screenxy, unsigned short button ) override
        {
            return ExclusiveButtonPress;
        }

        ButtonReleaseOutcome on_button_released( bool inside, screenxy, unsigned short button ) override
        {
            return ActionCompleted;
        }

        void on_clicked( unsigned short button, screenxy ) override
        {
        }

        void on_double_clicked( unsigned short b, screenxy ) override
        {
        }

        void on_key_pressed( unsigned short k, unsigned int modifierFlags ) override
        {
        }

        void on_key_released( unsigned short k, unsigned int modifierFlags ) override
        {
        }

        void on_text_received( wchar_t ) override
        {
        }

        void on_keyboard_focus_gained() override
        {
        }

        void on_keyboard_focus_lost() override
        {
        }

        void on_screen_resized( const dimension2d<unsigned int>& ) override
        {
        }

        void on_reset() override
        {
        }

        void on_dragged_over_by( const draginfo&, screenxy, cursorbuttons_state ) override
        {
        }

        void on_drag_drop_receive( std::unique_ptr<dragpackage>, screenxy ) override
        {
        }

        void advance_one_frame() override
        {
        }

        std::unique_ptr<draginfo> on_get_drag_info( screenxy ) const override
        {
            return std::unique_ptr<draginfo>();
        }

        std::unique_ptr<dragpackage> on_get_drag_package( const draginfo& ) const override
        {
            return std::unique_ptr<dragpackage>();
        }
    };

    class samplebackgroundwidget : public do_nothing_widget
    {
    public:
        explicit samplebackgroundwidget( int tab )
            : do_nothing_widget( BackgroundName, rect( 0, 0, dimension2dui( 640, 480 ) ), true, BACK_Z, tab )
        {
        }

        virtual void render( const unsigned long ) const
        {
        }
    };

    class samplewidget0 : public do_nothing_widget
    {
    public:
        explicit samplewidget0( int tab )
            : do_nothing_widget( wname0, rect( 0, 0, dimension2dui( 32, 32 ) ), true, WIDGET_Z, tab )
        {
        }

        virtual void render( const unsigned long ) const
        {
        }
    };

    class samplewidget1 : public do_nothing_widget
    {
    public:
        explicit samplewidget1( int tab )
            : do_nothing_widget( wname1, rect( 64, 64, dimension2dui( 128, 128 ) ), true, WIDGET_Z, tab )
        {
        }

        virtual void render( const unsigned long ) const
        {
        }
    };

    // Creates two widgets with tab orders 5 and 3 and a background.
    std::list<std::shared_ptr<iwidget>> make_sample_widgets()
    {
        std::list<std::shared_ptr<iwidget>> widgetlist;
        widgetlist.emplace_back( std::make_shared<samplewidget1>( 5 ) );
        widgetlist.emplace_back( std::make_shared<samplebackgroundwidget>( 42 ) );
        widgetlist.emplace_back( std::make_shared<samplewidget0>( 3 ) );
        return widgetlist;
    }
}

TEST( layout, create )
{
    std::list<std::shared_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets( std::move( widgetlist ) );
    auto layout0 = layout::make_shared( std::move( widgets ) );
    EXPECT_EQ( 3, layout0->widgets().size() );
}

TEST( layout, no_duplicate_widgetnames )
{
    std::list<std::shared_ptr<iwidget>> widgetlist;
    widgetlist.emplace_back( std::make_shared<samplewidget1>( 5 ) );
    widgetlist.emplace_back( std::make_shared<samplewidget1>( 6 ) );
    EXPECT_THROW( iwidgetcollection widgets( std::move( widgetlist ) ) );
}

TEST( layout, find_widget )
{
    std::list<std::shared_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets( std::move( widgetlist ) );
    auto layout0 = layout::make_shared( std::move( widgets ) );
    EXPECT_TRUE( nullptr != layout0->find_widget( "WIDGET0" ) );
    EXPECT_TRUE( nullptr != layout0->find_widget( "WIDGET1" ) );
    EXPECT_TRUE( nullptr == layout0->find_widget( "NOT_THERE" ) );
    EXPECT_TRUE( wname0 == layout0->find_widget( "WIDGET0" )->name() );
    EXPECT_TRUE( wname1 == layout0->find_widget( "WIDGET1" )->name() );
}

TEST( layout, widgets_under )
{
    std::list<std::shared_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets( std::move( widgetlist ) );
    auto layout0 = layout::make_shared( std::move( widgets ) );

    std::vector<iwidget*> under;

    layout0->widgets_under( 1, 1, under );
    EXPECT_EQ( 2, under.size() );

    layout0->widgets_under( 128, 128, under );
    EXPECT_EQ( 2, under.size() );

    layout0->widgets_under( 600, 400, under );
    EXPECT_EQ( 1, under.size() );

    layout0->widgets_under( -1, -1, under );
    EXPECT_EQ( 0, under.size() );
}

TEST( layout, widgets_under_and_not )
{
    std::list<std::shared_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets( std::move( widgetlist ) );
    auto layout0 = layout::make_shared( std::move( widgets ) );

    std::vector<iwidget*> under, notunder;

    layout0->widgets_under( 1, 1, under, notunder );
    EXPECT_EQ( 2, under.size() );
    EXPECT_EQ( 1, notunder.size() );

    layout0->widgets_under( 128, 128, under, notunder );
    EXPECT_EQ( 2, under.size() );
    EXPECT_EQ( 1, notunder.size() );

    layout0->widgets_under( 600, 400, under, notunder );
    EXPECT_EQ( 1, under.size() );
    EXPECT_EQ( 2, notunder.size() );

    layout0->widgets_under( -1, -1, under, notunder );
    EXPECT_EQ( 0, under.size() );
    EXPECT_EQ( 3, notunder.size() );
}

TEST( layout, highest_widget_under )
{
    std::list<std::shared_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets( std::move( widgetlist ) );
    auto layout0 = layout::make_shared( std::move( widgets ) );

    iwidget* highest = nullptr;

    highest = layout0->highest_widget_under( 1, 1 );
    EXPECT_TRUE( wname0 == highest->name() );

    highest = layout0->highest_widget_under( 128, 128 );
    EXPECT_TRUE( wname1 == highest->name() );

    highest = layout0->highest_widget_under( 600, 400 );
    EXPECT_TRUE( BackgroundName == highest->name() );

    highest = layout0->highest_widget_under( -1, -1 );
    EXPECT_EQ( nullptr, highest );
}

TEST( layout, widget_moved )
{
    std::list<std::shared_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets( std::move( widgetlist ) );
    auto layout0 = layout::make_shared( std::move( widgets ) );

    iwidget* highest = nullptr;

    highest = layout0->highest_widget_under( 1, 1 );
    EXPECT_TRUE( wname0 == highest->name() );

    highest->set_activearea_rect( rect( 100, 100, dimension2dui( 10, 10 ) ) );

    highest = layout0->highest_widget_under( 1, 1 );
    EXPECT_FALSE( wname0 == highest->name() );

    highest = layout0->highest_widget_under( 101, 101 );
    EXPECT_TRUE( wname0 == highest->name() );
}

TEST( layout, widget_disabled )
{
    std::list<std::shared_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets( std::move( widgetlist ) );
    auto layout0 = layout::make_shared( std::move( widgets ) );

    std::vector<iwidget*> under;

    iwidget* highest = nullptr;

    auto w1 = layout0->highest_widget_under( 1, 1 );
    EXPECT_TRUE( wname0 == w1->name() );

    w1->disable_widget();

    auto w2 = layout0->highest_widget_under( 1, 1 );
    EXPECT_FALSE( w2 == w1 );

    w1->enable_widget();

    w2 = layout0->highest_widget_under( 1, 1 );
    EXPECT_TRUE( w2 == w1 );
}
