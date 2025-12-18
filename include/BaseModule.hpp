#pragma once

#include <string>
#include <iostream>
#include "BaseEvent.hpp"
#include "EventHandlerInterface.hpp"
#include "Connector.hpp"

class Module : public EventHandlerInterface {
protected:
    size_t id;
    std::string name;
    Connector* connector = nullptr;

public:
    Module(size_t _id, const std::string& _name)
        : id(_id), name(_name) {}

    virtual ~Module() = default;

    virtual void setup() = 0;

    void set_connector(Connector* conn) {
        connector = conn;
    }

    bool send(EventPtr event) {
        if (connector != nullptr) {
            return connector->send_to_manager(event);
        } else {
            std::cout << "[" << name << "] Sending: "
                      << event->to_string() << std::endl;
            return true;
        }
    }

    void process_messages() {
        if (connector) {
            EventPtr event;
            while ((event = connector->recv_from_manager()) != nullptr) {
                process_event(event);
            }
        }
    }

    // ВАЖНО: сохраняем ваш интерфейс подписки!
    template<typename EventClass, typename Handler>
    void subscribe(Handler handler) {
        BaseEvent::EventType event_type = EventClass{}.get_event_type();
        
        EventHandlerInterface::subscribe(event_type, [handler](EventPtr event) {
            if (auto casted = std::dynamic_pointer_cast<EventClass>(event)) {
                handler(casted);
            }
        });
    }

    size_t get_id() const { return id; }
    const std::string& get_name() const { return name; }
};

