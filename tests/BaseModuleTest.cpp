#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "../include/BaseEvent.hpp"
#include "../include/EventHandlerInterface.hpp" 
#include "../include/BaseModule.hpp"

class TestMessageEvent : public BaseEvent {
private:
    std::string text;
public:
    TestMessageEvent() : text("default") {}
    TestMessageEvent(const std::string& _text) : text(_text) {}
    
    BaseEvent::EventType type() const override { 
        return 1;
    }
    
    std::string to_string() const override { 
        return "TestMessageEvent: " + text; 
    }
    
    const std::string& get_text() const { return text; }
};

class TestErrorEvent : public BaseEvent {
private:
    std::string message;
    int code;
public:
    TestErrorEvent() : message("default"), code(0) {}
    TestErrorEvent(const std::string& _message, int _code = 0) 
        : message(_message), code(_code) {}
    
    BaseEvent::EventType type() const override { 
        return 2;
    }
    
    std::string to_string() const override { 
        return "TestErrorEvent: " + message + " (code: " + std::to_string(code) + ")"; 
    }
    
    const std::string& get_message() const { return message; }
    int get_code() const { return code; }
};

class TestChatModule : public BaseModule {
private:
    std::vector<std::string> received_messages;
    std::vector<std::string> received_errors;

public:
    TestChatModule(size_t id) : BaseModule(id, "TestChatModule") {}

    void init_subscribes() override {
        subscribe<TestMessageEvent>([this](auto msg) {
            received_messages.push_back(msg->get_text());
        });

        subscribe<TestErrorEvent>([this](auto err) {
            received_errors.push_back(err->get_message());
        });
    }

    const std::vector<std::string>& get_received_messages() const { return received_messages; }
    const std::vector<std::string>& get_received_errors() const { return received_errors; }
    
    void simulate_send_message(const std::string& text) {
        auto message = std::make_shared<TestMessageEvent>(text);
        send(message);
    }
};

class EmptyTestModule : public BaseModule {
public:
    EmptyTestModule(size_t id) : BaseModule(id, "EmptyTestModule") {}
    void init_subscribes() override {}
};

TEST(BaseModuleTest, ModuleInitialization) {
    TestChatModule module(1);
    
    EXPECT_EQ(module.get_id(), 1);
    EXPECT_EQ(module.get_name(), "TestChatModule");
}

TEST(BaseModuleTest, ModuleCanSendEvents) {
    TestChatModule module(1);
    auto message = std::make_shared<TestMessageEvent>("Test");
    
    bool send_result = module.send(message);
    EXPECT_TRUE(send_result);
}

TEST(BaseModuleTest, EventHandlerSubscribeAndProcess) {
    TestChatModule module(1);
    module.init_subscribes();
    
    auto message = std::make_shared<TestMessageEvent>("Hello");
    bool processed = module.process_event(message);
    
    EXPECT_TRUE(processed);
    EXPECT_EQ(module.get_received_messages().size(), 1);
    EXPECT_EQ(module.get_received_messages()[0], "Hello");
}

TEST(BaseModuleTest, MultipleEventTypes) {
    TestChatModule module(1);
    module.init_subscribes();
    
    auto message = std::make_shared<TestMessageEvent>("Hello");
    auto error = std::make_shared<TestErrorEvent>("Database error", 500);
    
    module.process_event(message);
    module.process_event(error);
    
    EXPECT_EQ(module.get_received_messages().size(), 1);
    EXPECT_EQ(module.get_received_errors().size(), 1);
    EXPECT_EQ(module.get_received_messages()[0], "Hello");
    EXPECT_EQ(module.get_received_errors()[0], "Database error");
}

TEST(BaseModuleTest, UnsubscribeWorks) {
    TestChatModule module(1);
    module.init_subscribes();
    
    auto message1 = std::make_shared<TestMessageEvent>("First");
    module.process_event(message1);
    EXPECT_EQ(module.get_received_messages().size(), 1);
    
    module.unsubscribe(1);
    
    auto message2 = std::make_shared<TestMessageEvent>("Second");
    bool processed = module.process_event(message2);
    EXPECT_FALSE(processed);
    EXPECT_EQ(module.get_received_messages().size(), 1);
}

TEST(BaseModuleTest, ClearSubscriptions) {
    TestChatModule module(1);
    module.init_subscribes();
    
    auto message = std::make_shared<TestMessageEvent>("Test");
    bool processed1 = module.process_event(message);
    EXPECT_TRUE(processed1);
    
    module.clear_subscriptions();
    
    auto message2 = std::make_shared<TestMessageEvent>("Test2");
    bool processed2 = module.process_event(message2);
    EXPECT_FALSE(processed2);
}

TEST(BaseModuleTest, NoSubscriptions) {
    EmptyTestModule module(1);
    module.init_subscribes();
    
    auto event = std::make_shared<TestMessageEvent>("Test");
    bool processed = module.process_event(event);
    
    EXPECT_FALSE(processed);
}

TEST(BaseModuleTest, DynamicSubscription) {
    TestChatModule module(1);
    module.init_subscribes();
    
    bool dynamic_handler_called = false;
    module.subscribe<TestErrorEvent>([&dynamic_handler_called](auto err) {
        dynamic_handler_called = true;
    });
    
    auto error = std::make_shared<TestErrorEvent>("Dynamic error");
    module.process_event(error);
    
    EXPECT_TRUE(dynamic_handler_called);
}

TEST(BaseModuleTest, MultipleModulesIndependent) {
    TestChatModule module1(1);
    TestChatModule module2(2);
    module1.init_subscribes();
    module2.init_subscribes();
    
    auto message1 = std::make_shared<TestMessageEvent>("For module1");
    auto message2 = std::make_shared<TestMessageEvent>("For module2");
    
    module1.process_event(message1);
    module2.process_event(message2);
    
    EXPECT_EQ(module1.get_received_messages().size(), 1);
    EXPECT_EQ(module2.get_received_messages().size(), 1);
    EXPECT_EQ(module1.get_received_messages()[0], "For module1");
    EXPECT_EQ(module2.get_received_messages()[0], "For module2");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}