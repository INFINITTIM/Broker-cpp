#pragma once

#include <cstdint>
#include <string>
#include <memory>

class BaseEvent {
public:
    using EventType = uint32_t; 
    // используем псевдоним для более удобного восприятия кода в дальнейшем,
    // чтобы в программе не было просто инт а было написано что это именно число
    // обозначающее тип события 
private:
    EventType event_type;
    // поле класса для хранения типа события
public:
    BaseEvent(EventType _event_type) : event_type(_event_type) {};
    // конструктор с помощью которого наследуясь от базвого типа пользователь будет
    // задавать тип данному классу событий

    virtual ~BaseEvent() = default;
    // виртуальный деструктор 

    virtual std::string to_string() const = 0;
    // единственный виртуальный метод базового класса событий который
    // является описанием данного класса событий

    EventType get_event_type() const { return event_type; }
    // геттер для получения типа данного события
};

using EventPtr = std::shared_ptr<BaseEvent>;
// псевдоним для краткости в дальнейшем для записи указателя на базовый класс события