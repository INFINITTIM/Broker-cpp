#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "BaseConnector.hpp"
#include "BaseEvent.hpp"

class BaseManager {
private:
    // вектор содержащий указатели на всех коннекторов подключенных к брокеру модулей
    // иначе говоря это соединения со всеми модулями
    std::vector<std::unique_ptr<BaseConnector>> _connectors;
    
    // map подписок то есть кто на что подписан
    // в данном случае есть событие и рядом с ним размещается список коннекторов событий подписанных
    // на данное событие 
    std::unordered_map<BaseEvent::EventType, std::unordered_set<BaseConnector*>> _subscriptions;

public:
    // подключение модуля к брокеру сообщений
    BaseConnector* register_module(size_t queue_size = 10) {
        auto conn = std::make_unique<BaseConnector>(queue_size);
        BaseConnector* raw_ptr = conn.get();
        _connectors.push_back(std::move(conn));
        return raw_ptr;
    }

    // такой-то модуль хочет получать события такого-то типа (подписка на событие чтобы брокер знал о не1)
    void subscribe_module(BaseConnector* connector, BaseEvent::EventType event_type) {
        _subscriptions[event_type].insert(connector);
    }

    // метод - брокер (рассылка сообщений по модулям)
    void process_all_messages() {
        for (auto& connector : _connectors) { // проходится по всем коннекторам
            EventPtr event;
            // у каждого коннектора забираем сообщения если они есть из канала
            while ((event = connector->recv_from_module()) != nullptr) {
                // здесь мы типо находим подписчиков для этого типа события
                auto it = _subscriptions.find(event->get_event_type());
                if (it != _subscriptions.end()) {
                    // а далее рассылаем только подписчикам на это событие
                    for (BaseConnector* target : it->second) {
                        if (target != connector.get()) {
                            target->send_to_module(event);
                        }
                    }
                }
            }
        }
    }
};