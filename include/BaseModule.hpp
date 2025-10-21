#pragma once

#include <cstddef>
#include <string>
#include <memory>
#include <iostream>

#include "BaseEvent.hpp"      
#include "EventHandlerInterface.hpp" 

class BaseModule : public EventHandlerInterface {
protected:
    size_t _id;
    std::string _name;

public:
    BaseModule(size_t id, const std::string& name) : _id(id), _name(name) {}
    virtual ~BaseModule() = default;

    // Абстрактный метод - КАЖДЫЙ модуль должен определить свои подписки
    virtual void init_subscribes() = 0;

    // Упрощенная отправка (просто вызов менеджера)
    bool send(const EventPtr& event) {
        std::cout << "[" << _name << "] 📤 Отправляю событие: " << event->to_string() << std::endl;
        // В реальности здесь был бы вызов менеджера
        return true;
    }

    // Геттеры
    size_t get_id() const { return _id; }
    const std::string& get_name() const { return _name; }
};