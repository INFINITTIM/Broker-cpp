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
    size_t id;
    std::string name;
    BaseConnector* _connector = nullptr;
    BaseManager* _manager = nullptr;

public:
    BaseModule(size_t _id, const std::string& _name)
        : id(_id), name(_name) {}

    virtual ~BaseModule() = default;

    virtual void init_subscribes() = 0;

    void set_connector(BaseConnector* conn) {
        _connector = conn;
    }

    void set_manager(BaseManager* mgr) {
        _manager = mgr;
    }

    bool send(const EventPtr& event) {
        if (_connector != nullptr) {
            return _connector->send_to_manager(event);
        } else {
            std::cout << "[" << name << "] Отправляю: "
                      << event->to_string() << std::endl;
            return true;
        }
    }

    void process_incoming_events() {
        if (_connector) {
            EventPtr event;
            while ((event = _connector->recv_from_manager()) != nullptr) {
                process_event(event);
            }
        }
    }

    template<typename EventClass, typename Handler>
    void subscribe(Handler handler) {
        EventHandlerInterface::subscribe<EventClass>(handler);

        if (_manager && _connector) {
            _manager->subscribe_module(_connector, EventClass{}.get_event_type());
        }
    }

    size_t get_id() const { return id; }
    const std::string& get_name() const { return name; }
};
