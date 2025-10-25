#pragma once

#include <cstddef>
#include <string>
#include <memory>
#include <iostream>

#include "BaseEvent.hpp"      
#include "EventHandlerInterface.hpp" 

class BaseModule : public EventHandlerInterface {
protected:
    size_t id;
    std::string name;
    // идентификатор модуля и название модуля

public:
    BaseModule(size_t _id, std::string _name) : id(_id), name(_name) {}
    // конструктор по умолчанию

    virtual ~BaseModule() = default;
    // деструктор

    virtual void init_subscribes() = 0;
    // реализация подписок изначальных (можно изначально ни на кого не быть подписанным)

    bool send(const EventPtr& event) {
        std::cout << "[" << name << "] Отправляю событие: " 
        << event->to_string() << std::endl;
        return true;
    }
    // отправляет сообщение (менеджеру когда он будет реализован)

    size_t get_id() const { return id; }
    const std::string& get_name() const { return name; }
    // геттеры
};