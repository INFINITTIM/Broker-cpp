#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include "BaseEvent.hpp"

class LimitedThreadSafePipe {
private:
    std::queue<EventPtr> _queue;
    std::mutex _mutex;
    size_t _max_size;

public:
    LimitedThreadSafePipe(size_t max_size) {
        _max_size = max_size;
    }

    bool push(EventPtr event) {
        _mutex.lock();
        if (_queue.size() >= _max_size) {
            _mutex.unlock();
            return false;
        }
        _queue.push(event);
        _mutex.unlock();
        return true;
    }

    EventPtr pop() {
        _mutex.lock();
        if (_queue.empty()) {
            _mutex.unlock();
            return nullptr;
        }
        EventPtr event = _queue.front();
        _queue.pop();
        _mutex.unlock();
        return event;
    }

    size_t size() {
        _mutex.lock();
        size_t s = _queue.size();
        _mutex.unlock();
        return s;
    }
};