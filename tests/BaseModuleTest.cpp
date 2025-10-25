#include <gtest/gtest.h>
#include <memory>
#include "../include/BaseModule.hpp"

class TestEvent1 : public BaseEvent {
public:
    TestEvent1() : BaseEvent(1) {}
    std::string to_string() const override { return "TestEvent1"; }
};

class TestEvent2 : public BaseEvent {
public:
    TestEvent2() : BaseEvent(2) {}
    std::string to_string() const override { return "TestEvent2"; }
};

class TestModule : public BaseModule {
private:
    int event1_count = 0;
    int event2_count = 0;
public:
    TestModule(size_t id) : BaseModule(id, "TestModule") {}

    void init_subscribes() override {
        subscribe<TestEvent1>([this](auto event) {
            event1_count++;
        });
        
        subscribe<TestEvent2>([this](auto event) {
            event2_count++;
        });
    }

    int get_event1_count() const { return event1_count; }
    int get_event2_count() const { return event2_count; }
};

TEST(BaseModuleTest, ModuleInitialization) {
    TestModule module(42);
    
    EXPECT_EQ(module.get_id(), 42);
    EXPECT_EQ(module.get_name(), "TestModule");
}

TEST(BaseModuleTest, SendMethodWorks) {
    TestModule module(1);
    auto event = std::make_shared<TestEvent1>();

    EXPECT_NO_THROW(module.send(event));
}

TEST(BaseModuleTest, EventProcessing) {
    TestModule module(1);
    module.init_subscribes();
    
    auto event1 = std::make_shared<TestEvent1>();
    auto event2 = std::make_shared<TestEvent2>();
    
    module.process_event(event1);
    module.process_event(event2);
    
    EXPECT_EQ(module.get_event1_count(), 1);
    EXPECT_EQ(module.get_event2_count(), 1);
}

TEST(BaseModuleTest, UnsubscribeWorks) {
    TestModule module(1);
    module.init_subscribes();
    
    auto event1 = std::make_shared<TestEvent1>();
    module.process_event(event1);
    EXPECT_EQ(module.get_event1_count(), 1);
    
    module.unsubscribe(1);
    
    auto event2 = std::make_shared<TestEvent1>();
    bool processed = module.process_event(event2);
    EXPECT_FALSE(processed);
    EXPECT_EQ(module.get_event1_count(), 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}