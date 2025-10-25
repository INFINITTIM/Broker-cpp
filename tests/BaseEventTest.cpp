#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "../include/BaseEvent.hpp"

class TestEvent1 : public BaseEvent {
public:
    TestEvent1() : BaseEvent(1001) {}
    std::string to_string() const override { return "TestEvent1"; }
};

class TestEvent2 : public BaseEvent {
public:
    TestEvent2() : BaseEvent(1002) {}
    std::string to_string() const override { return "TestEvent2"; }
};

class TestEventWithString : public BaseEvent {
private:
    std::string str;
public:
    TestEventWithString(const std::string& _str) : BaseEvent(1003), str(_str) {}
    std::string to_string() const override { return "TestEventWithString: " + str; }
};

TEST(BaseEventTest, EventHasUniqueType) {
    TestEvent1 event1;
    TestEvent2 event2;
    
    EXPECT_NE(event1.get_event_type(), event2.get_event_type());
}

TEST(BaseEventTest, EventTypeIsConsistent) {
    TestEvent1 event1;
    TestEvent1 event2;
    
    EXPECT_EQ(event1.get_event_type(), event2.get_event_type());
}

TEST(BaseEventTest, ToStringMethodWorks) {
    TestEvent1 event1;
    TestEventWithString event2("Hello World");
    
    EXPECT_EQ(event1.to_string(), "TestEvent1");
    EXPECT_EQ(event2.to_string(), "TestEventWithString: Hello World");
}

TEST(BaseEventTest, EventPtrUse) {
    EventPtr event1 = std::make_shared<TestEvent1>();
    EventPtr event2 = std::make_shared<TestEvent2>();
    EventPtr event3 = std::make_shared<TestEvent1>();

    EXPECT_EQ(event1->get_event_type(), event3->get_event_type());
    EXPECT_NE(event1->get_event_type(), event2->get_event_type());
    EXPECT_EQ(event1->to_string(), "TestEvent1");
    EXPECT_EQ(event2->to_string(), "TestEvent2");
    EXPECT_EQ(event3->to_string(), "TestEvent1");
}

TEST(BaseEventTest, MultipleType) {
    TestEvent1 event1, event2, event3;
    
    EXPECT_EQ(event1.get_event_type(), event2.get_event_type());  
    EXPECT_EQ(event2.get_event_type(), event3.get_event_type()); 
    EXPECT_EQ(event1.get_event_type(), event3.get_event_type());
}

TEST(BaseEventTest, EventWithConstructorParameters) {
    TestEventWithString event1("First");
    TestEventWithString event2("Second");
    
    EXPECT_EQ(event1.get_event_type(), event2.get_event_type());
    EXPECT_EQ(event1.to_string(), "TestEventWithString: First");
    EXPECT_EQ(event2.to_string(), "TestEventWithString: Second");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}