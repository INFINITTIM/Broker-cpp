#include <iostream>
#include <memory>
#include <vector>

#include "../include/BaseEvent.hpp"
#include "../include/BaseModule.hpp"

enum class ChatEventType : uint32_t {
    MESSAGE_SENT = 1001,
    USER_JOINED = 1002,
    USER_LEFT = 1003,
    PRIVATE_MESSAGE = 1004
};

enum class GameEventType : uint32_t {
    PLAYER_MOVE = 2001,
    PLAYER_ATTACK = 2002,
    ITEM_PICKED = 2003,
    LEVEL_COMPLETE = 2004
};

class ChatMessageEvent : public BaseEvent {
private:
    std::string username;
    std::string message;
    
public:

    ChatMessageEvent() 
        : BaseEvent(static_cast<EventType>(ChatEventType::MESSAGE_SENT)),
          username(""), message("") {}

    ChatMessageEvent(const std::string& user, const std::string& msg)
        : BaseEvent(static_cast<EventType>(ChatEventType::MESSAGE_SENT)),
          username(user), message(msg) {}
    
    std::string to_string() const override {
        return "ChatMessage: " + username + " -> " + message;
    }
    
    std::string get_username() const { return username; }
    std::string get_message() const { return message; }
};

class PlayerMoveEvent : public BaseEvent {
private:
    std::string player_id;
    float x, y;
    
public:
    PlayerMoveEvent() 
        : BaseEvent(static_cast<EventType>(GameEventType::PLAYER_MOVE)),
          player_id(""), x(0), y(0) {}

    PlayerMoveEvent(const std::string& id, float x_pos, float y_pos)
        : BaseEvent(static_cast<EventType>(GameEventType::PLAYER_MOVE)),
          player_id(id), x(x_pos), y(y_pos) {}
    
    std::string to_string() const override {
        return "PlayerMove: " + player_id + " to (" + 
               std::to_string(x) + ", " + std::to_string(y) + ")";
    }
    
    std::string get_player_id() const { return player_id; }
    float get_x() const { return x; }
    float get_y() const { return y; }
};

class ChatModule : public BaseModule {
public:
    ChatModule() : BaseModule(1, "ChatModule") {}
    
    void init_subscribes() override {
        subscribe<ChatMessageEvent>([](auto event) {
            std::cout << "ЧАТ: Сообщение от " << event->get_username()
                      << ": " << event->get_message() << std::endl;
        });
    }
    
    void send_message(const std::string& user, const std::string& msg) {
        auto event = std::make_shared<ChatMessageEvent>(user, msg);
        send(event);
    }
};

class GameModule : public BaseModule {
public:
    GameModule() : BaseModule(2, "GameModule") {}
    
    void init_subscribes() override {
        subscribe<PlayerMoveEvent>([](auto event) {
            std::cout << "ИГРА: Игрок " << event->get_player_id()
                      << " переместился в (" << event->get_x()
                      << ", " << event->get_y() << ")" << std::endl;
        });
    }
    
    void player_moved(const std::string& id, float x, float y) {
        auto event = std::make_shared<PlayerMoveEvent>(id, x, y);
        send(event);
    }
};

int main() {
    std::cout << "=== БИБЛИОТЕКА СОБЫТИЙ ===" << std::endl;
    std::cout << "Пользователь сам создает enum'ы для типов событий\n" << std::endl;
    
    ChatModule chat;
    GameModule game;
    
    chat.init_subscribes();
    game.init_subscribes();

    chat.send_message("Пользователь1", "Привет1");
    game.player_moved("p1", 10.5f, 20.3f);

    auto msg_event = std::make_shared<ChatMessageEvent>("Пользователь2", "Привет2");
    chat.process_event(msg_event);
    
    auto move_event = std::make_shared<PlayerMoveEvent>("p2", 5.0f, 15.0f);
    game.process_event(move_event);
    
    return 0;
}