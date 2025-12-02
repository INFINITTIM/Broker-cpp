#pragma once

#include "SkippingConnectorLink.hpp"

class BaseConnector {
private:
    SkippingConnectorLink _to_manager; 
    SkippingConnectorLink _to_module;

public:
    BaseConnector(size_t queue_size = 10)
        : _to_manager(queue_size), _to_module(queue_size) {}

    bool send_to_manager(EventPtr event) {
        return _to_manager.send(event);
    }

    bool send_to_module(EventPtr event) {
        return _to_module.send(event);
    }

    EventPtr recv_from_module() {
        return _to_manager.try_recv();
    }

    EventPtr recv_from_manager() {
        return _to_module.try_recv();
    }

    //bool is_module_queue_empty() {
    //    return false;
    //}
};