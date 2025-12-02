#pragma once

#include "LimitedThreadSafePipe.hpp"

class SkippingConnectorLink {
private:
    LimitedThreadSafePipe _pipe;

public:
    SkippingConnectorLink(size_t queue_size = 10)
        : _pipe(queue_size) {}

    bool send(EventPtr event) {
        return _pipe.push(event);
    }

    EventPtr try_recv() {
        return _pipe.pop();
    }
};