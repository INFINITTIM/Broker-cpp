#pragma once

#include <cstdint>
#include <string>
#include <memory>

class BaseEvent {
    public:
        using EventType = uint16_t;
        virtual ~BaseEvent() = default;
        virtual EventType type() const = 0;
        virtual std::string to_string() const = 0;
};

using EventPtr = std::shared_ptr<BaseEvent>;