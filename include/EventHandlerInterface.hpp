#pragma once

#include <unordered_map>
#include <functional>
#include <memory>

#include "BaseEvent.hpp"

class EventHandlerInterface {
private:
    std::unordered_map<BaseEvent::EventType, std::function<void(EventPtr)>> _handlers;

public:
    virtual ~EventHandlerInterface() = default;

    template<typename EventClass, typename Handler>
    void subscribe(Handler handler) {
        BaseEvent::EventType event_type = EventClass{}.get_event_type();
        _handlers[event_type] = [handler](EventPtr event) {
            if (auto casted = std::dynamic_pointer_cast<EventClass>(event)) {
                handler(casted);
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
    // отписываемся от события (убираем его из мапы)

    bool process_event(const EventPtr& event) {
        auto it = _handlers.find(event->get_event_type());
        if (it != _handlers.end()) {
            it->second(event);
            return true;
        }
        return false;
    }
    //мы берём событие если оно нашлось в мапе по типу его то тогда 

    void clear_subscriptions() {
        _handlers.clear();
    }
    // отписываемся от всех событий
};