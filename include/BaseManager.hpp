#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "BaseConnector.hpp"
#include "BaseEvent.hpp"

class BaseManager {
private:
    std::vector<std::unique_ptr<BaseConnector>> _connectors;
    
    std::unordered_map<BaseEvent::EventType, std::unordered_set<BaseConnector*>> _subscriptions;

public:
    BaseConnector* register_module(size_t queue_size = 10) {
        auto conn = std::make_unique<BaseConnector>(queue_size);
        BaseConnector* raw_ptr = conn.get();
        _connectors.push_back(std::move(conn));
        return raw_ptr;
    }

    void subscribe_module(BaseConnector* connector, BaseEvent::EventType event_type) {
        _subscriptions[event_type].insert(connector);
    }

    void process_all_messages() {
        for (auto& connector : _connectors) {
            EventPtr event;
            while ((event = connector->recv_from_module()) != nullptr) {
                // здесь мы типо находим подписчиков для этого типа события
                auto it = _subscriptions.find(event->get_event_type());
                if (it != _subscriptions.end()) {
                    // а далее рассылаем только подписчикам на это событие
                    for (BaseConnector* target : it->second) {
                        target->send_to_module(event);
                    }
                }
            }
        }
    }
};