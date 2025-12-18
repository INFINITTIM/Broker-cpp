#pragma once

#include "LimitedThreadSafePipe.hpp"

/*
Данный класс является классом-обёрткой для потокобезопасной очереди
*/

class SkippingConnectorLink {
private:
    // потокобезопасная очередь
    LimitedThreadSafePipe _pipe;

public:
    // конструктор с параметром по умолчанию 
    SkippingConnectorLink(size_t queue_size = 10)
        : _pipe(queue_size) {}

    // отправка сообщения (события)
    bool send(EventPtr event) {
        return _pipe.push(event);
    }

    // получение сообщения то есть просто смотрит в очередь и если там что-то есть то достаёт
    EventPtr try_recv() {
        return _pipe.pop();
    }
};