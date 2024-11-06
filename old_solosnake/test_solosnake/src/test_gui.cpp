#include "solosnake/testing/testing.hpp"
#include "solosnake/layout.hpp"
#include "solosnake/iwidget.hpp"
#include "solosnake/iscreenstate.hpp"
#include "solosnake/gui.hpp"
#include <memory>
#include <string>

using namespace solosnake;

namespace
{
#define WIDGET_Z 0
#define BACK_Z -1

const std::string wname0("WIDGET0");
const std::string wname1("WIDGET1");
const std::string BackgroundName("BACK");

class samplebackgroundwidget : public iwidget
{
public:
    explicit samplebackgroundwidget(int tab)
        : iwidget(BackgroundName, rect(0, 0, dimension2dui(640, 480)), true, BACK_Z, tab)
    {
    }

    virtual void render() const
    {
    }
};

class button0 : public iwidget
{
public:
    explicit button0(int tab)
        : iwidget(wname0, rect(10, 10, dimension2dui(10, 10)), true, WIDGET_Z, tab)
    {
    }

    virtual void render() const
    {
    }
};

class button1 : public iwidget
{
public:
    explicit button1(int tab)
        : iwidget(wname1, rect(10, 30, dimension2dui(10, 10)), true, WIDGET_Z, tab)
    {
    }

    virtual void render() const
    {
    }
};

// Creates two widgets with tab orders 5 and 3 and a background.
std::list<std::unique_ptr<iwidget>> make_sample_widgets()
{
    std::list<std::unique_ptr<iwidget>> widgetlist;
    widgetlist.emplace_back(std::unique_ptr<iwidget>(new button0(1)));
    widgetlist.emplace_back(std::unique_ptr<iwidget>(new button1(2)));
    widgetlist.emplace_back(std::unique_ptr<iwidget>(new samplebackgroundwidget(0)));
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
        return std::shared_ptr<fakescreenstate>(new fakescreenstate());
    }

    virtual bool is_screen_ended() const
    {
        return false;
    }

    virtual nextscreen get_next_screen() const
    {
        return "";
    }
};
}

TEST(gui, create)
{
    std::list<std::unique_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets(std::move(widgetlist));
    auto layout0 = layout::create(std::move(widgets));
    std::shared_ptr<iscreenstate> scrstate = fakescreenstate::create();
    auto g = std::make_shared<gui>(layout0, dimension2dui(640, 480), scrstate);
}
