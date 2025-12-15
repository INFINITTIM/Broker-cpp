#pragma once

#include "SkippingConnectorLink.hpp"

/*
класс представляющий собой по факту двустороннюю дорогу которая в свою очередь состоит из
двух однонаправленных каналов которые соединяют менеджер с модулем данного коннектора
*/

class BaseConnector {
private:
    // канал - от модуля к менеджеру
    SkippingConnectorLink _to_manager; 
    // канал - от менеджера к модулю
    SkippingConnectorLink _to_module;

public:
    // конструктор создающий эти два канала одинакогого заданного размера
    BaseConnector(size_t queue_size = 10)
        : _to_manager(queue_size), _to_module(queue_size) {}

    // ложим сообщение (событие) в канал - к менеджеру
    bool send_to_manager(EventPtr event) {
        return _to_manager.send(event);
    }

    // ложим сообщение (событие) в канал - к модулю
    bool send_to_module(EventPtr event) {
        return _to_module.send(event);
    }

    // достаём сообщение (событие) из канала - от модуля
    EventPtr recv_from_module() {
        return _to_manager.try_recv();
    }

    // достаём сообщение (событие) из канала - от менеджера
    EventPtr recv_from_manager() {
        return _to_module.try_recv();
    }
};