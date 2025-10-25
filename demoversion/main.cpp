#include <iostream>
#include <memory>
#include <vector>

#include "../include/BaseEvent.hpp"
#include "../include/BaseModule.hpp"

class ChatMessageEvent : public BaseEvent {
private:
    std::string username;
    std::string message;
public:
    ChatMessageEvent() : BaseEvent(100), username(""), message("") {}

    ChatMessageEvent(const std::string& user, const std::string& msg) 
        : BaseEvent(100), username(user), message(msg) {}
    
    std::string to_string() const override {
        return "Пользователь " + username + ": " + message;
    }
    
    const std::string& get_username() const { return username; }
    const std::string& get_message() const { return message; }
};

class UserJoinedEvent : public BaseEvent {
private:
    std::string username;
public:
    UserJoinedEvent() : BaseEvent(101), username("") {}

    UserJoinedEvent(const std::string& user) : BaseEvent(101), username(user) {}
    
    std::string to_string() const override {
        return "Пользователь " + username + " присоединился к чату";
    }
    
    const std::string& get_username() const { return username; }
};

class ChatModule : public BaseModule {
public:
    ChatModule(size_t id) : BaseModule(id, "ChatModule") {}

    void init_subscribes() override {
        subscribe<ChatMessageEvent>([this](auto msg) {
            std::cout << "ЧАТ: " << msg->get_username() << " написал: " 
                      << msg->get_message() << std::endl; 
                    });

        subscribe<UserJoinedEvent>([this](auto user) {
            std::cout << "ЧАТ: " << user->get_username() 
                      << " присоединился!" << std::endl; 
                    });
    }

    void send_message(const std::string& username, const std::string& message) {
        auto msg_event = std::make_shared<ChatMessageEvent>(username, message);
        send(msg_event);
    }

    void user_join(const std::string& username) {
        auto join_event = std::make_shared<UserJoinedEvent>(username);
        send(join_event);
    }
};

int main() {
    ChatModule chat(1);
    
    chat.init_subscribes();
    
    chat.user_join("Алексей");
    chat.user_join("Владимир");
    
    chat.send_message("Алексей", "Ку!");
    chat.send_message("Владимир", "Йоу привет, Алексей! Как дела?");
    chat.send_message("Алексей", "Да нормально, спасибо! А у тебя?");
    
    return 0;
}