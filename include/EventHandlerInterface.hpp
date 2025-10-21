#pragma once

#include <unordered_map>
#include <functional>
#include <memory>

#include "BaseEvent.hpp"

class EventHandlerInterface {
private:
    std::unordered_map<BaseEvent::EventType, std::function<void(EventPtr)>> _handlers;
    // у нас есть мапа которая содержит тип события и функцию как на неё реагиоровать

public:
    virtual ~EventHandlerInterface() = default;

    template<typename EventClass, typename Handler>
    void subscribe(Handler&& handler) {
        BaseEvent::EventType event_type = EventClass{}.type();
        _handlers[event_type] = [handler = std::forward<Handler>(handler)](EventPtr event) {
            if (auto casted_event = std::dynamic_pointer_cast<EventClass>(event)) {
                handler(casted_event);
            }
        };
    }
    /*
    у нас есть метод подписки на событие поэтому мы берем и подписываемся используя
    кавычки записывая в них класс события и в скобках в хэндлере записываем лямбду к примеру
    как на данное событие нужно реагировать и как обрабатывать
    */

    void unsubscribe(BaseEvent::EventType event_type) {
        _handlers.erase(event_type);
    }

    bool process_event(const EventPtr& event) {
        auto it = _handlers.find(event->type());
        if (it != _handlers.end()) {
            it->second(event);
            return true;
        }
        return false;
    }

    void clear_subscriptions() {
        _handlers.clear();
    }
};