#include <gtest/gtest.h>
#include <memory>
#include "../include/BaseEvent.hpp"

class TestMessageEvent : public BaseEvent {
private:
    std::string text;
public:
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
    TestErrorEvent(const std::string& message, int _code = 0) 
        : message(message), code(_code) {}
    
    BaseEvent::EventType type() const override { 
        return 2;
    }
    
    std::string to_string() const override { 
        return "TestErrorEvent: " + message + " (code: " + std::to_string(code) + ")"; 
    }
    
    const std::string& get_message() const { return message; }
    int get_code() const { return code; }
};

TEST(BaseEventTest, ReturnCorrectTypeNumbers) {
    auto message = std::make_shared<TestMessageEvent>("Hello");
    auto error = std::make_shared<TestErrorEvent>("Error", 500);

    EXPECT_EQ(message->type(), 1);
    EXPECT_EQ(error->type(), 2); 
}

TEST(BaseEventTest, HaveCorrectStringRepresentation) {
    auto message = std::make_shared<TestMessageEvent>("Test Message");
    auto error = std::make_shared<TestErrorEvent>("File Not Found", 404);
    
    EXPECT_EQ(message->to_string(), "TestMessageEvent: Test Message");
    EXPECT_EQ(error->to_string(), "TestErrorEvent: File Not Found (code: 404)");
}

TEST(BaseEventTest, WorkWithPolymorphism) {
    BaseEvent* event1 = new TestMessageEvent("Poly Test");
    BaseEvent* event2 = new TestErrorEvent("Poly Error", 300);
    
    EXPECT_EQ(event1->type(), 1);
    EXPECT_EQ(event2->type(), 2);
    
    delete event1;
    delete event2;
}

TEST(BaseEventTest, WorkWithEventPtr) {
    EventPtr message = std::make_shared<TestMessageEvent>("Shared");
    EventPtr error = std::make_shared<TestErrorEvent>("Shared Error", 200);
    
    EXPECT_EQ(message->type(), 1);
    EXPECT_EQ(error->type(), 2);
}

TEST(BaseEventTest, CastCorrectly) {
    EventPtr event = std::make_shared<TestMessageEvent>("Cast Test");
    
    auto message = std::dynamic_pointer_cast<TestMessageEvent>(event);
    EXPECT_NE(message, nullptr);
    EXPECT_EQ(message->get_text(), "Cast Test");
    
    auto error = std::dynamic_pointer_cast<TestErrorEvent>(event);
    EXPECT_EQ(error, nullptr);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}