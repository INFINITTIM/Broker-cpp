#include <iostream>
#include <memory>

#include "../include/BaseManager.hpp"
#include "../include/BaseModule.hpp"

enum class ChatEventType : uint32_t {
    MESSAGE_SENT = 1001
};

enum class GameEventType : uint32_t {
    PLAYER_MOVE = 2001
};

class ChatMessageEvent : public BaseEvent {
private:
    std::string _username;
    std::string _message;
public:
    ChatMessageEvent() 
        : BaseEvent(static_cast<EventType>(ChatEventType::MESSAGE_SENT)) {}
        
    ChatMessageEvent(const std::string& user, const std::string& msg)
        : BaseEvent(static_cast<EventType>(ChatEventType::MESSAGE_SENT)),
          _username(user), _message(msg) {}
    
    std::string to_string() const override {
        return "ChatMessage: " + _username + " -> " + _message;
    }
    
    std::string get_username() const { return _username; }
    std::string get_message() const { return _message; }
};

class PlayerMoveEvent : public BaseEvent {
private:
    std::string _player_id;
    float _x, _y;
public:
    PlayerMoveEvent()
        : BaseEvent(static_cast<EventType>(GameEventType::PLAYER_MOVE)) {}
        
    PlayerMoveEvent(const std::string& id, float x, float y)
        : BaseEvent(static_cast<EventType>(GameEventType::PLAYER_MOVE)),
          _player_id(id), _x(x), _y(y) {}
    
    std::string to_string() const override {
        return "PlayerMove: " + _player_id + " to (" + 
               std::to_string(_x) + ", " + std::to_string(_y) + ")";
    }
    
    std::string get_player_id() const { return _player_id; }
    float get_x() const { return _x; }
    float get_y() const { return _y; }
};

class ChatModule : public BaseModule {
public:
    ChatModule(size_t id) : BaseModule(id, "ChatModule") {}

    void init_subscribes() override {
        subscribe<ChatMessageEvent>([](auto event) {
            std::cout << "[ChatModule] Получено сообщение от "
                      << event->get_username() << ": " 
                      << event->get_message() << std::endl;
        });
    }

    void send_message(const std::string& user, const std::string& msg) {
        auto event = std::make_shared<ChatMessageEvent>(user, msg);
        send(event);
    }
};

class GameModule : public BaseModule {
public:
    GameModule(size_t id) : BaseModule(id, "GameModule") {}

    void init_subscribes() override {
        subscribe<PlayerMoveEvent>([](auto event) {
            std::cout << "[GameModule] Игрок " << event->get_player_id()
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
    std::cout << "=== ТЕСТ БРОКЕРА СОБЫТИЙ ===" << std::endl;

    BaseManager manager;

    ChatModule chat(1);
    GameModule game(2);

    chat.set_connector(manager.register_module());
    chat.set_manager(&manager);

    game.set_connector(manager.register_module());
    game.set_manager(&manager);

    chat.init_subscribes();
    game.init_subscribes();

    chat.send_message("Пользователь", "Привет!");

    game.player_moved("Игрок", 10.5f, 20.3f);

    std::cout << "\n--- Обработка событий менеджером ---" << std::endl;
    manager.process_all_messages();

    std::cout << "\n--- Получение событий модулями ---" << std::endl;
    chat.process_incoming_events();
    game.process_incoming_events(); 

    return 0;
}