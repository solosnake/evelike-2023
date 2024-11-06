#include "solosnake/testing/testing.hpp"
#include "solosnake/layout.hpp"
#include "solosnake/iwidget.hpp"
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

class samplewidget0 : public iwidget
{
public:
    explicit samplewidget0(int tab)
        : iwidget(wname0, rect(0, 0, dimension2dui(32, 32)), true, WIDGET_Z, tab)
    {
    }

    virtual void render() const
    {
    }
};

class samplewidget1 : public iwidget
{
public:
    explicit samplewidget1(int tab)
        : iwidget(wname1, rect(64, 64, dimension2dui(128, 128)), true, WIDGET_Z, tab)
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
    widgetlist.emplace_back(std::unique_ptr<iwidget>(new samplewidget1(5)));
    widgetlist.emplace_back(std::unique_ptr<iwidget>(new samplebackgroundwidget(42)));
    widgetlist.emplace_back(std::unique_ptr<iwidget>(new samplewidget0(3)));
    return widgetlist;
}
}

TEST(layout, create)
{
    std::list<std::unique_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets(std::move(widgetlist));
    auto layout0 = layout::create(std::move(widgets));
    EXPECT_EQ(3, layout0->widgets().size());
}

TEST(layout, no_duplicate_widgetnames)
{
    std::list<std::unique_ptr<iwidget>> widgetlist;
    widgetlist.emplace_back(std::unique_ptr<iwidget>(new samplewidget1(5)));
    widgetlist.emplace_back(std::unique_ptr<iwidget>(new samplewidget1(6)));
    EXPECT_ANY_THROW(iwidgetcollection widgets(std::move(widgetlist)));
}

TEST(layout, find_widget)
{
    std::list<std::unique_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets(std::move(widgetlist));
    auto layout0 = layout::create(std::move(widgets));
    EXPECT_TRUE(nullptr != layout0->find_widget("WIDGET0"));
    EXPECT_TRUE(nullptr != layout0->find_widget("WIDGET1"));
    EXPECT_TRUE(nullptr == layout0->find_widget("NOT_THERE"));
    EXPECT_TRUE(wname0 == layout0->find_widget("WIDGET0")->name());
    EXPECT_TRUE(wname1 == layout0->find_widget("WIDGET1")->name());
}

TEST(layout, widgets_under)
{
    std::list<std::unique_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets(std::move(widgetlist));
    auto layout0 = layout::create(std::move(widgets));

    std::vector<iwidget*> under;

    layout0->widgets_under(1, 1, under);
    EXPECT_EQ(2, under.size());

    layout0->widgets_under(128, 128, under);
    EXPECT_EQ(2, under.size());

    layout0->widgets_under(600, 400, under);
    EXPECT_EQ(1, under.size());

    layout0->widgets_under(-1, -1, under);
    EXPECT_EQ(0, under.size());
}

TEST(layout, widgets_under_and_not)
{
    std::list<std::unique_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets(std::move(widgetlist));
    auto layout0 = layout::create(std::move(widgets));

    std::vector<iwidget*> under, notunder;

    layout0->widgets_under(1, 1, under, notunder);
    EXPECT_EQ(2, under.size());
    EXPECT_EQ(1, notunder.size());

    layout0->widgets_under(128, 128, under, notunder);
    EXPECT_EQ(2, under.size());
    EXPECT_EQ(1, notunder.size());

    layout0->widgets_under(600, 400, under, notunder);
    EXPECT_EQ(1, under.size());
    EXPECT_EQ(2, notunder.size());

    layout0->widgets_under(-1, -1, under, notunder);
    EXPECT_EQ(0, under.size());
    EXPECT_EQ(3, notunder.size());
}

TEST(layout, highest_widget_under)
{
    std::list<std::unique_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets(std::move(widgetlist));
    auto layout0 = layout::create(std::move(widgets));

    iwidget* highest = nullptr;

    highest = layout0->highest_widget_under(1, 1);
    EXPECT_TRUE(wname0 == highest->name());

    highest = layout0->highest_widget_under(128, 128);
    EXPECT_TRUE(wname1 == highest->name());

    highest = layout0->highest_widget_under(600, 400);
    EXPECT_TRUE(BackgroundName == highest->name());

    highest = layout0->highest_widget_under(-1, -1);
    EXPECT_EQ(nullptr, highest);
}

TEST(layout, widget_moved)
{
    std::list<std::unique_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets(std::move(widgetlist));
    auto layout0 = layout::create(std::move(widgets));

    iwidget* highest = nullptr;

    highest = layout0->highest_widget_under(1, 1);
    EXPECT_TRUE(wname0 == highest->name());

    highest->set_activearea_rect(rect(100, 100, dimension2dui(10, 10)));

    highest = layout0->highest_widget_under(1, 1);
    EXPECT_FALSE(wname0 == highest->name());

    highest = layout0->highest_widget_under(101, 101);
    EXPECT_TRUE(wname0 == highest->name());
}

TEST(layout, widget_disabled)
{
    std::list<std::unique_ptr<iwidget>> widgetlist = make_sample_widgets();
    iwidgetcollection widgets(std::move(widgetlist));
    auto layout0 = layout::create(std::move(widgets));

    auto w1 = layout0->highest_widget_under(1, 1);
    EXPECT_TRUE(wname0 == w1->name());

    w1->disable_widget();

    auto w2 = layout0->highest_widget_under(1, 1);
    EXPECT_FALSE(w2 == w1);

    w1->enable_widget();

    w2 = layout0->highest_widget_under(1, 1);
    EXPECT_TRUE(w2 == w1);
}
