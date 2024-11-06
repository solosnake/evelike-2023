
#include "solosnake/notification.hpp"
#include <string>
#include <cassert>
#include "solosnake/testing/testing.hpp"

using namespace solosnake;

namespace
{

unsigned int g_obsDtorCallCount = 0;

class MyModel
{
public:
    notifier<MyModel>& ModelChanged() const
    {
        return m_modelNotifier;
    }
    void NotifyModelChanged()
    {
        m_modelNotifier.notify(*this);
    }

private:
    mutable notifier<MyModel> m_modelNotifier;
};

class MyObserver
{
public:
    MyObserver()
    {
        m_notification_counter = 0;
    }

    ~MyObserver()
    {
        g_obsDtorCallCount++;
    }

    void ModelHasChangedHandler(const MyModel&)
    {
        ++m_notification_counter;
    }

    int NotificationCount() const
    {
        return m_notification_counter;
    }

private:
    int m_notification_counter;
};
}

TEST(notification, notifications)
{
    MyModel model;
    std::shared_ptr<MyObserver> observer(new MyObserver);

    EXPECT_TRUE(0 == observer->NotificationCount());

    // Make the connection, specifying the handler function to call.
    model.ModelChanged() += handler
        <MyModel>(&MyObserver::ModelHasChangedHandler, std::weak_ptr<MyObserver>(observer));

    // The model can notify the observers when it has changed:
    model.NotifyModelChanged();

    EXPECT_TRUE(1 == observer->NotificationCount());

    model.NotifyModelChanged();

    EXPECT_TRUE(2 == observer->NotificationCount());

    // Remove the connection when no longer needed:
    model.ModelChanged() -= handler
        <MyModel>(&MyObserver::ModelHasChangedHandler, std::weak_ptr<MyObserver>(observer));

    // No further notifications:
    model.NotifyModelChanged();

    EXPECT_TRUE(2 == observer->NotificationCount());
}

namespace
{
unsigned int g_obsWithModelDtorCallCount = 0;
unsigned int g_ModelInObserverDtorCallCount = 0;

class ModelInObserver
{
public:
    ~ModelInObserver()
    {
        g_ModelInObserverDtorCallCount++;
    }
    notifier<ModelInObserver>& ModelChanged() const
    {
        return m_modelNotifier;
    }
    void NotifyModelChanged()
    {
        m_modelNotifier.notify(*this);
    }

private:
    mutable notifier<ModelInObserver> m_modelNotifier;
};

class ObserverWithModel : public std::enable_shared_from_this<ObserverWithModel>
{
public:
    static std::shared_ptr<ObserverWithModel> create()
    {
        std::shared_ptr<ObserverWithModel> obs = std::make_shared<ObserverWithModel>();
        obs->m_model->ModelChanged() += handler<ModelInObserver>(
            &ObserverWithModel::ModelHasChangedHandler, std::weak_ptr<ObserverWithModel>(obs));
        return obs;
    }

    ObserverWithModel()
    {
        m_model = std::make_shared<ModelInObserver>();
        m_notification_counter = 0;
    }

    ~ObserverWithModel()
    {
        g_obsWithModelDtorCallCount++;
    }

    void ModelHasChangedHandler(const ModelInObserver&)
    {
        ++m_notification_counter;
    }

    int NotificationCount() const
    {
        return m_notification_counter;
    }

    ModelInObserver& Model()
    {
        return *m_model;
    }

private:
    std::shared_ptr<ModelInObserver> m_model;
    int m_notification_counter;
};
}

TEST(notification, lifetimes)
{
    // Pre-condition.
    g_obsDtorCallCount = 0;
    EXPECT_TRUE(g_obsDtorCallCount == 0);

    {
        // Create model in scope #1.
        MyModel model;

        {
            // Create and attach observer in scope #2:
            std::shared_ptr<MyObserver> observer(new MyObserver);

            // Make the connection, specifying the handler function to call.
            model.ModelChanged() += handler
                <MyModel>(&MyObserver::ModelHasChangedHandler, std::weak_ptr<MyObserver>(observer));

            // Leave scope. We expect that the observer to be destroyed now.
        }

        EXPECT_TRUE(g_obsDtorCallCount == 1);

        // The model can notify the observers when it has changed:
        model.NotifyModelChanged();
    }

    // Leaving scope should destroy everything:
    EXPECT_TRUE(g_obsDtorCallCount == 1);
}

TEST(notification, lifetimes_shared_ptr)
{
    // Pre-condition.
    g_obsWithModelDtorCallCount = 0;
    g_ModelInObserverDtorCallCount = 0;

    EXPECT_TRUE(g_ModelInObserverDtorCallCount == 0);
    EXPECT_TRUE(g_obsWithModelDtorCallCount == 0);

    // Create the observer containing its own model
    auto observer = ObserverWithModel::create();

    // Check its notifications are working.
    EXPECT_TRUE(observer->NotificationCount() == 0);
    observer->Model().NotifyModelChanged();
    EXPECT_TRUE(observer->NotificationCount() == 1);

    // Destroy model. Does it die or is it kept alive?
    observer.reset();

    // Leaving scope should destroy everything:
    EXPECT_TRUE(g_ModelInObserverDtorCallCount == 1);
    EXPECT_TRUE(g_obsWithModelDtorCallCount == 1);
}