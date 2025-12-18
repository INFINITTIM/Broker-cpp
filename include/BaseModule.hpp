#pragma once

#include <string>
#include <memory>
#include <iostream>

#include "BaseEvent.hpp"
#include "EventHandlerInterface.hpp"
#include "BaseConnector.hpp"

class BaseManager;

class BaseModule : public EventHandlerInterface {
protected:
    size_t id; // уникальный id модуля 
    std::string name; // имя модуля
    BaseConnector* _connector = nullptr; // коннектор (труба) к менеджеру
    BaseManager* _manager = nullptr; // указатель на менеджера

public:
    // конструктор модуля (id и имя)
    BaseModule(size_t _id, const std::string& _name)
        : id(_id), name(_name) {}

    virtual ~BaseModule() = default;

    virtual void init_subscribes() = 0;

    //сеттер для коннектора
    void set_connector(BaseConnector* conn) {
        _connector = conn;
    }

    //сеттер для менеджера
    void set_manager(BaseManager* mgr) {
        _manager = mgr;
    }

    // отправка сообщения (если модуль подключен к брокеру то у него есть коннектор
    // и тогда происходит отправка - как раз через его канал)
    bool send(const EventPtr& event) {
        if (_connector != nullptr) {
            return _connector->send_to_manager(event);
        } else {
            // если нет коннектора то есть модуль не подключен к менеджеру то просто вывод на экран 
            //(для проверки что все работает (не обязательно))
            std::cout << "[" << name << "] Отправляю: "
                      << event->to_string() << std::endl;
            return true;
        }
    }

    // получение сообщений (пока есть события в очереди коннектора то обрабатываем их)
    void process_incoming_events() {
        if (_connector) {
            EventPtr event;
            while ((event = _connector->recv_from_manager()) != nullptr) {
                process_event(event);
            }
        }
    }

    // подписка модуля на события 
    template<typename EventClass, typename Handler>
    void subscribe(Handler handler) {
        // регистрируем обработчик 
        EventHandlerInterface::subscribe<EventClass>(handler);

        // если есть менеджер то есть модуль подключен к брокеру то тогда регистрируем
        // подписку на событие в менеджере чтобы он знал о нём
        if (_manager && _connector) {
            _manager->subscribe_module(_connector, EventClass{}.get_event_type());
        }
    }

    // геттеры для id и имени
    size_t get_id() const { return id; }
    const std::string& get_name() const { return name; }
};
