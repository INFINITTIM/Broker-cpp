#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include "../include/Thread.hpp"
#include "../include/BaseEvent.hpp"
#include "../include/BaseManager.hpp"
#include "../include/BaseModule.hpp"

enum class ChatEventType : uint32_t {
    MESSAGE = 1001
};

class ChatMessageEvent : public BaseEvent {
private:
    std::string username;
    std::string message;
    
public:
    ChatMessageEvent() 
        : BaseEvent(static_cast<EventType>(ChatEventType::MESSAGE)),
          username(""), message("") {}

    ChatMessageEvent(const std::string& user, const std::string& msg)
        : BaseEvent(static_cast<EventType>(ChatEventType::MESSAGE)),
          username(user), message(msg) {}
    
    std::string to_string() const override {
        return username + ": " + message;
    }
    
    std::string get_username() const { return username; }
    std::string get_message() const { return message; }
};

class ChatModule : public BaseModule {
private:
    int received_count = 0;
    
public:
    ChatModule() : BaseModule(1, "МодульЧат") {}
    
    void init_subscribes() override {
        subscribe<ChatMessageEvent>([this](auto event) {
            std::cout << "[чат] Получено: " << event->to_string() << std::endl;
            received_count++;
        });
    }
    
    void send_message(const std::string& user, const std::string& msg) {
        auto event = std::make_shared<ChatMessageEvent>(user, msg);
        std::cout << "[чат] Отправляю: " << event->to_string() << std::endl;
        send(event);
    }
    
    int get_received_count() const { return received_count; }
};

class LoggerModule : public BaseModule {
private:
    int logged_count = 0;
    
public:
    LoggerModule() : BaseModule(2, "МодульЛог") {}
    
    void init_subscribes() override {
        subscribe<ChatMessageEvent>([this](auto event) {
            std::cout << "[лог] Записано в лог: " 
                      << event->get_username() << " - " 
                      << event->get_message() << std::endl;
            logged_count++;
        });
    }
    
    int get_logged_count() const { return logged_count; }
};

int main() {
    std::cout << "[1] Создание менеджера (брокера)" << std::endl;
    BaseManager manager;

    std::cout << "[2] Создание модулей" << std::endl;
    ChatModule chat;
    LoggerModule logger;

    std::cout << "[3] Регистрация модулей в менеджере" << std::endl;
    auto chat_connector = manager.register_module(3);
    auto logger_connector = manager.register_module(3);
    
    chat.set_connector(chat_connector);
    chat.set_manager(&manager);
    
    logger.set_connector(logger_connector);
    logger.set_manager(&manager);

    std::cout << "[4] Инициализация подпискок модулей" << std::endl;
    chat.init_subscribes();
    logger.init_subscribes();

    std::cout << "[5] Происходит запуск потоков" << std::endl;
    Thread manager_thread;
    Thread chat_thread;
    Thread logger_thread;
    
    manager_thread.start_manager(&manager);
    chat_thread.start_module(&chat);
    logger_thread.start_module(&logger);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << std::endl << "окно отладки (смотрим на сообщения и принимаются ли они двумя классами - логгером и чатом)" << std::endl;

    std::cout << std::endl << "Отправляется 5 сообщений" << std::endl;
    
    for (int i = 1; i <= 5; i++) {
        chat.send_message("Пользователь" + std::to_string(i), 
                         "Сообщение " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << std::endl << "[6] Происходит остановка потоков" << std::endl;
    chat_thread.stop();
    logger_thread.stop();
    manager_thread.stop();
    
    return 0;
}