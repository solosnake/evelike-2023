#include "solosnake/testing/testing.hpp"
#include "solosnake/layout.hpp"
#include "solosnake/iwidget.hpp"
#include "solosnake/inputs.hpp"
#include "solosnake/ishape.hpp"
#include "solosnake/iscreenstate.hpp"
#include "solosnake/gui.hpp"
#include "solosnake/guishape.hpp"
#include <memory>
#include <string>

using namespace solosnake;

namespace
{
#define WIDGET_Z 0
#define BACK_Z -1

    const std::string wname0( "WIDGET0" );
    const std::string wname1( "WIDGET1" );
    const std::string BackgroundName( "BACK" );

    std::unique_ptr<ishape> make_triangle_shape()
    {
        std::unique_ptr<guishape> t( new guishape() );
        t->add_triangle( point2d( 0.0f, 0.0f ), point2d( 1.0f, 0.0f ), point2d( 1.0f, 1.0f ) );
        return std::unique_ptr<ishape>( t.release() );
    }

    // Implements pure virtuals as no op calls.
    class do_nothing_widget : public iwidget
    {
    protected:

        do_nothing_widget( const widgetparams& params, iwidget::KeyboardFocusing kbf )
            : iwidget( params, kbf, iwidget::SharedPtrOnly() )
        {
        }

        do_nothing_widget( const widgetparams& params,
                           iwidget::KeyboardFocusing kbf,
                           std::unique_ptr<ishape> s )
            : iwidget( params, kbf, std::move( s ), iwidget::SharedPtrOnly() )
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

    // BACKGROUND
    class samplebackgroundwidget : public do_nothing_widget
    {
    public:

        explicit samplebackgroundwidget( int tab )
            : do_nothing_widget(
                widgetparams(BackgroundName, rect( 0, 0, dimension2dui( 640, 480 ) ), BACK_Z, tab ),
                iwidget::YesKeyBoardFocus )
        {
        }

        void on_text_received( wchar_t c ) override
        {
            text_.push_back( c );
        }

        void render( const unsigned long ) const override
        {
        }

        std::wstring text_;
    };

    // BUTTON 0
    class button0 : public do_nothing_widget
    {
    public:

        explicit button0( int tab )
            : do_nothing_widget( 
                widgetparams(wname0, rect( 10, 10, dimension2dui( 10, 10 ) ), WIDGET_Z, tab),
                iwidget::YesKeyBoardFocus )
            , clickedOn_( false )
            , clickedButton_( 0 )
            , clickedXY_( 0, 0 )
        {
        }

        void on_text_received( wchar_t c ) override
        {
            text_.push_back( c );
        }

        virtual void on_clicked( unsigned short button, screenxy xy )
        {
            clickedButton_ = button;
            clickedXY_ = xy;
            clickedOn_ = true;
        }

        void render( const unsigned long ) const override
        {
        }

        bool clickedOn_;
        unsigned short clickedButton_;
        screenxy clickedXY_;
        std::wstring text_;
    };

    // BUTTON 1
    class button1 : public do_nothing_widget
    {
    public:

        explicit button1( int tab )
            : do_nothing_widget( 
                widgetparams( wname1, rect( 10, 30, dimension2dui( 10, 10 ) ), WIDGET_Z, tab ),
                iwidget::YesKeyBoardFocus )
            , clickedOn_( false )
            , clickedButton_( 0 )
            , clickedXY_( 0, 0 )
        {
        }

        void on_clicked( unsigned short button, screenxy xy ) override
        {
            clickedButton_ = button;
            clickedXY_ = xy;
            clickedOn_ = true;
        }

        void render( const unsigned long ) const override
        {
        }

        void on_text_received( wchar_t c ) override
        {
            text_.push_back( c );
        }

        bool clickedOn_;
        unsigned short clickedButton_;
        screenxy clickedXY_;
        std::wstring text_;
    };

    // TRIANGLE
    class trianglebutton : public do_nothing_widget
    {
    public:

        explicit trianglebutton( int tab )
            : do_nothing_widget(
                widgetparams( wname0, rect( 10, 10, dimension2dui( 10, 10 ) ), WIDGET_Z, tab ),
                iwidget::YesKeyBoardFocus,
                make_triangle_shape() )
            , clickedOn_( false )
            , clickedButton_( 0 )
            , clickedXY_( 0, 0 )
        {
        }

        void on_text_received( wchar_t c ) override
        {
            text_.push_back( c );
        }

        void on_clicked( unsigned short button, screenxy xy ) override
        {
            clickedButton_ = button;
            clickedXY_ = xy;
            clickedOn_ = true;
        }

        void render( const unsigned long ) const override
        {
        }

        bool clickedOn_;
        unsigned short clickedButton_;
        screenxy clickedXY_;
        std::wstring text_;
    };

    // Creates two widgets with tab orders 5 and 3 and a background.
    std::list<std::shared_ptr<iwidget>> make_sample_widgets()
    {
        std::list<std::shared_ptr<iwidget>> widgetlist;
        widgetlist.emplace_back( std::shared_ptr<iwidget>( new button0( 1 ) ) );
        widgetlist.emplace_back( std::shared_ptr<iwidget>( new button1( 2 ) ) );
        widgetlist.emplace_back( std::shared_ptr<iwidget>( new samplebackgroundwidget( 0 ) ) );
        return widgetlist;
    }

    class fakescreenstate : public iscreenstate
    {
        fakescreenstate()
        {
        }

    public:

        static std::shared_ptr<fakescreenstate> create()
        {
            return std::shared_ptr<fakescreenstate>( new fakescreenstate() );
        }

        bool is_screen_ended() const override
        {
            return false;
        }

        nextscreen get_next_screen() const override
        {
            return nextscreen( "" );
        }
    };
}

TEST( gui, create )
{
    std::list<std::shared_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets( std::move( widgetlist ) );
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>( std::move( widgets ), dimension2dui( 640, 480 ), scrstate );
}

TEST( gui, tab_order )
{
    std::list<std::shared_ptr<iwidget>> widgetlist;
    auto bk = new samplebackgroundwidget( 0 );
    auto b0 = new button0( 1 );
    auto b1 = new button1( 2 );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b0 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b1 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( bk ) );

    iwidgetcollection widgets( std::move( widgetlist ) );
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>( std::move( widgets ), dimension2dui( 640, 480 ), scrstate );

    EXPECT_TRUE( bk->has_keyboard_focus() );
    EXPECT_FALSE( b0->has_keyboard_focus() );
    EXPECT_FALSE( b1->has_keyboard_focus() );

    g->tab_keyboard_focus_to_next_widget();

    EXPECT_FALSE( bk->has_keyboard_focus() );
    EXPECT_TRUE( b0->has_keyboard_focus() );
    EXPECT_FALSE( b1->has_keyboard_focus() );

    inputs events;

    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_text_entry( 'h' ) );
    events.add_event( input_event::make_text_entry( 'e' ) );
    events.add_event( input_event::make_text_entry( 'l' ) );
    events.add_event( input_event::make_text_entry( 'l' ) );
    events.add_event( input_event::make_text_entry( 'o' ) );

    EXPECT_FALSE( events.empty() );
    g->process_inputs( events );
    events.clear_events();
    EXPECT_TRUE( events.empty() );

    EXPECT_FALSE( bk->has_keyboard_focus() );
    EXPECT_TRUE( b0->has_keyboard_focus() );
    EXPECT_FALSE( b1->has_keyboard_focus() );

    EXPECT_TRUE( b0->text_ == std::wstring( L"hello" ) );

    g->tab_keyboard_focus_to_next_widget();

    EXPECT_FALSE( bk->has_keyboard_focus() );
    EXPECT_FALSE( b0->has_keyboard_focus() );
    EXPECT_TRUE( b1->has_keyboard_focus() );

    events.add_event( input_event::make_text_entry( 'w' ) );
    events.add_event( input_event::make_text_entry( 'o' ) );
    events.add_event( input_event::make_text_entry( 'r' ) );
    events.add_event( input_event::make_text_entry( 'l' ) );
    events.add_event( input_event::make_text_entry( 'd' ) );

    EXPECT_FALSE( events.empty() );
    g->process_inputs( events );
    events.clear_events();
    EXPECT_TRUE( events.empty() );

    EXPECT_TRUE( b1->text_ == std::wstring( L"world" ) );
}

TEST( gui, click_on_button0 )
{
    std::list<std::shared_ptr<iwidget>> widgetlist;
    auto bk = new samplebackgroundwidget( 0 );
    auto b0 = new button0( 1 );
    auto b1 = new button1( 2 );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b0 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b1 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( bk ) );

    iwidgetcollection widgets( std::move( widgetlist ) );
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>( std::move( widgets ), dimension2dui( 640, 480 ), scrstate );

    inputs events;

    EXPECT_FALSE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_cursor_move( 0, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    events.add_event(
        input_event::make_cursor_button_down( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b0->is_mouse_inside_widget() );
    EXPECT_TRUE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    events.add_event(
        input_event::make_cursor_button_up( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_TRUE( b0->clickedOn_ );
}

TEST( gui, click_off_button0 )
{
    std::list<std::shared_ptr<iwidget>> widgetlist;
    auto bk = new samplebackgroundwidget( 0 );
    auto b0 = new button0( 1 );
    auto b1 = new button1( 2 );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b0 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b1 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( bk ) );

    iwidgetcollection widgets( std::move( widgetlist ) );
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>( std::move( widgets ), dimension2dui( 640, 480 ), scrstate );

    inputs events;

    EXPECT_FALSE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_cursor_move( 0, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    // Mouse DOWN inside
    events.add_event(
        input_event::make_cursor_button_down( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b0->is_mouse_inside_widget() );
    EXPECT_TRUE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    // Move mouse off
    events.add_event( input_event::make_cursor_move( 0, screenxy( 200, 200 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_FALSE( b0->is_mouse_inside_widget() );
    EXPECT_TRUE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    // Release off widget - NO CLICK EVENT
    events.add_event(
        input_event::make_cursor_button_up( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 300, 300 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_FALSE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );
}

TEST( gui, click_on_button0_moveoff_and_onagain )
{
    std::list<std::shared_ptr<iwidget>> widgetlist;
    auto bk = new samplebackgroundwidget( 0 );
    auto b0 = new button0( 1 );
    auto b1 = new button1( 2 );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b0 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b1 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( bk ) );

    iwidgetcollection widgets( std::move( widgetlist ) );
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>( std::move( widgets ), dimension2dui( 640, 480 ), scrstate );

    inputs events;

    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_cursor_move( 0, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    events.add_event(
        input_event::make_cursor_button_down( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    // Move cursor off widget.
    events.add_event( input_event::make_cursor_move( 0, screenxy( 200, 200 ) ) );
    g->process_inputs( events );
    events.clear_events();

    // Move cursor back onto widget
    events.add_event( input_event::make_cursor_move( 0, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    // Release
    events.add_event(
        input_event::make_cursor_button_up( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_TRUE( b0->clickedOn_ );
}

TEST( gui, click_off_button0_and_on_button1 )
{
    std::list<std::shared_ptr<iwidget>> widgetlist;
    auto bk = new samplebackgroundwidget( 0 );
    auto b0 = new button0( 1 );
    auto b1 = new button1( 2 );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b0 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( b1 ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( bk ) );

    iwidgetcollection widgets( std::move( widgetlist ) );
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>( std::move( widgets ), dimension2dui( 640, 480 ), scrstate );

    inputs events;

    EXPECT_FALSE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_cursor_move( 0, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    // Mouse DOWN inside
    events.add_event(
        input_event::make_cursor_button_down( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 15, 15 ) ) );
    g->process_inputs( events );
    events.clear_events();

    // Move mouse off
    events.add_event( input_event::make_cursor_move( 0, screenxy( 200, 200 ) ) );
    g->process_inputs( events );
    events.clear_events();

    // Release off widget - NO CLICK EVENT
    events.add_event(
        input_event::make_cursor_button_up( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 300, 300 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_FALSE( b0->is_mouse_inside_widget() );
    EXPECT_FALSE( b0->is_action_pressed() );
    EXPECT_FALSE( b0->clickedOn_ );

    // Move to button1
    events.add_event( input_event::make_cursor_move( 0, screenxy( 15, 35 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b1->is_mouse_inside_widget() );
    EXPECT_FALSE( b1->is_action_pressed() );
    EXPECT_FALSE( b1->clickedOn_ );

    // Mouse down
    events.add_event(
        input_event::make_cursor_button_down( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 15, 35 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( b1->is_mouse_inside_widget() );
    EXPECT_TRUE( b1->is_action_pressed() );
    EXPECT_FALSE( b1->clickedOn_ );

    // Mouse up
    events.add_event(
        input_event::make_cursor_button_up( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 15, 35 ) ) );
    g->process_inputs( events );
    events.clear_events();

    // Expect click:
    EXPECT_TRUE( b1->is_mouse_inside_widget() );
    EXPECT_FALSE( b1->is_action_pressed() );
    EXPECT_TRUE( b1->clickedOn_ );
}

TEST( gui, click_on_trianglebutton )
{
    std::list<std::shared_ptr<iwidget>> widgetlist;
    auto bk = new samplebackgroundwidget( 0 );
    auto t = new trianglebutton( 1 );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( t ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( bk ) );

    iwidgetcollection widgets( std::move( widgetlist ) );
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>( std::move( widgets ), dimension2dui( 640, 480 ), scrstate );

    inputs events;

    EXPECT_FALSE( t->is_mouse_inside_widget() );
    EXPECT_FALSE( t->is_action_pressed() );
    EXPECT_FALSE( t->clickedOn_ );

    // Triangle is shaped kinda like this:
    //
    //             ------
    //             \    |
    //              \   |
    //               \  |
    //                \ |
    //                 \|
    // Middle is 15, 15
    // 16, 14 should be inside.
    // 14, 16 should be outside.
    //
    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_cursor_move( 0, screenxy( 16, 14 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( t->is_mouse_inside_widget() );
    EXPECT_FALSE( t->is_action_pressed() );
    EXPECT_FALSE( t->clickedOn_ );

    events.add_event(
        input_event::make_cursor_button_down( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 16, 14 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( t->is_mouse_inside_widget() );
    EXPECT_TRUE( t->is_action_pressed() );
    EXPECT_FALSE( t->clickedOn_ );

    events.add_event(
        input_event::make_cursor_button_up( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 16, 14 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( t->is_mouse_inside_widget() );
    EXPECT_FALSE( t->is_action_pressed() );
    EXPECT_TRUE( t->clickedOn_ );
}

TEST( gui, wont_click_on_trianglebutton )
{
    std::list<std::shared_ptr<iwidget>> widgetlist;
    auto bk = new samplebackgroundwidget( 0 );
    auto t = new trianglebutton( 1 );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( t ) );
    widgetlist.emplace_back( std::shared_ptr<iwidget>( bk ) );

    iwidgetcollection widgets( std::move( widgetlist ) );
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>( std::move( widgets ), dimension2dui( 640, 480 ), scrstate );

    inputs events;

    EXPECT_FALSE( t->is_mouse_inside_widget() );
    EXPECT_FALSE( t->is_action_pressed() );
    EXPECT_FALSE( t->clickedOn_ );

    // Triangle is shaped kinda like this:
    //
    //             ------
    //             \    |
    //              \   |
    //               \  |
    //                \ |
    //                 \|
    // Middle is 15, 15
    // 16, 14 should be inside.
    // 14, 16 should be outside.
    //
    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_cursor_move( 0, screenxy( 14, 16 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_FALSE( t->is_mouse_inside_widget() );
    EXPECT_FALSE( t->is_action_pressed() );
    EXPECT_FALSE( t->clickedOn_ );

    // Move over triangle part
    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_cursor_move( 0, screenxy( 16, 14 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_TRUE( t->is_mouse_inside_widget() );

    // Move off triangle part again
    events.add_event( input_event::make_focus_gained() );
    events.add_event( input_event::make_cursor_move( 0, screenxy( 14, 16 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_FALSE( t->is_mouse_inside_widget() );

    events.add_event(
        input_event::make_cursor_button_down( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 14, 16 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_FALSE( t->is_mouse_inside_widget() );
    EXPECT_FALSE( t->is_action_pressed() );
    EXPECT_FALSE( t->clickedOn_ );

    events.add_event(
        input_event::make_cursor_button_up( CURSOR_PRIMARY_BUTTON_FLAG, screenxy( 14, 16 ) ) );
    g->process_inputs( events );
    events.clear_events();

    EXPECT_FALSE( t->is_mouse_inside_widget() );
    EXPECT_FALSE( t->is_action_pressed() );
    EXPECT_FALSE( t->clickedOn_ );
}
