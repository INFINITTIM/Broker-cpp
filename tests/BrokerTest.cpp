#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "BaseManager.hpp"
#include "BaseModule.hpp"

enum class TestEventType : uint32_t {
    EVENT_A = 1,
    EVENT_B = 2
};

class EventA : public BaseEvent {
public:
    EventA() : BaseEvent(static_cast<EventType>(TestEventType::EVENT_A)) {}
    std::string to_string() const override { return "EventA"; }
};

class EventB : public BaseEvent {
public:
    EventB() : BaseEvent(static_cast<EventType>(TestEventType::EVENT_B)) {}
    std::string to_string() const override { return "EventB"; }
};

class TestModuleA : public BaseModule {
public:
    int received_A = 0;
    int received_B = 0;

    TestModuleA(size_t id) : BaseModule(id, "TestModuleA") {}

    void init_subscribes() override {
        subscribe<EventA>([this](auto) { received_A++; });
    }

    void send_A() {
        send(std::make_shared<EventA>());
    }
};

class TestModuleB : public BaseModule {
public:
    int received_A = 0;
    int received_B = 0;

    TestModuleB(size_t id) : BaseModule(id, "TestModuleB") {}

    void init_subscribes() override {
        subscribe<EventB>([this](auto) { received_B++; });
    }

    void send_B() {
        send(std::make_shared<EventB>());
    }
};

TEST(BrokerTest, BasicDelivery) {
    BaseManager manager;

    TestModuleA modA(1);
    TestModuleB modB(2);

    modA.set_connector(manager.register_module());
    modA.set_manager(&manager);
    modB.set_connector(manager.register_module());
    modB.set_manager(&manager);

    modA.init_subscribes();
    modB.init_subscribes();

    modA.send_A(); 
    modB.send_B(); 

    manager.process_all_messages();

    modA.process_incoming_events();
    modB.process_incoming_events();

    EXPECT_EQ(modA.received_A, 1); 
    EXPECT_EQ(modA.received_B, 0);

    EXPECT_EQ(modB.received_B, 1); 
    EXPECT_EQ(modB.received_A, 0);
}

TEST(BrokerTest, QueueOverflow) {
    BaseManager manager;

    TestModuleA sender(1);
    TestModuleA receiver(2);

    sender.set_connector(manager.register_module(1));
    sender.set_manager(&manager);
    receiver.set_connector(manager.register_module(1));
    receiver.set_manager(&manager);

    receiver.init_subscribes(); 
    sender.init_subscribes();

    sender.send_A();
    sender.send_A();
    sender.send_A();

    manager.process_all_messages();
    receiver.process_incoming_events();

    EXPECT_LE(receiver.received_A, 1);
    EXPECT_GE(receiver.received_A, 1); 
}

TEST(BrokerTest, DynamicSubscription) {
    BaseManager manager;

    TestModuleA mod(1);
    mod.set_connector(manager.register_module());
    mod.set_manager(&manager);

    auto event = std::make_shared<EventA>();
    mod.send(event);

    manager.process_all_messages();
    mod.process_incoming_events();

    EXPECT_EQ(mod.received_A, 0);

    mod.init_subscribes();

    mod.send_A();

    manager.process_all_messages();
    mod.process_incoming_events();

    EXPECT_EQ(mod.received_A, 1);
}