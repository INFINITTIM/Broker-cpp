#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include "BaseEvent.hpp"

class LimitedThreadSafePipe {
private:
    /*
    В данном классе присутствуют: обычная очередь состоящая из указателей на события, её размера мьютекса для 
    защиты от одновременного доступа к ресурсам за которые могут конкурировать потоки одновременно.
    */
    std::queue<EventPtr> _queue;
    std::mutex _mutex;
    size_t _max_size;

public:
    // конструктор определяющий максимальный размер очереди, очередь же создаётся автоматически а mutex изначально разблокирован
    LimitedThreadSafePipe(size_t max_size) {
        _max_size = max_size;
    }

    // функция добавления в очередь
    bool push(EventPtr event) {
        // если мьютекс свободен то поток захватывает его а если нет то тогда текущий поток ждёт
        _mutex.lock();
        // проверка не переполнена ли очередь
        if (_queue.size() >= _max_size) {
            // разблокируем перед возвратом мьютекс
            _mutex.unlock();
            return false; // событие не добавленно так как очередь переполнена
        }
        _queue.push(event); // добавляем событие в очередь
        _mutex.unlock(); // разблокируем мьютекс
        return true; // событие было добавлено 
    }

    // функция извлечения из очереди
    EventPtr pop() {
        _mutex.lock(); // аналогично происходит блокировка мьютекса
        if (_queue.empty()) { // проверка не пуста ли очередь
            _mutex.unlock(); // разбллокируем мьютекс
            return nullptr; // очередь пуста и поэтому возвращаем nullptr
        }
        EventPtr event = _queue.front(); // извлекаем из очереди элемент - событие
        _queue.pop(); // удаляем его из очереди
        _mutex.unlock(); // разблокировка мьютекса
        return event; // возвращает событие
    }

    // функция получения размера очереди 
    size_t size() {
        _mutex.lock(); // блокируем мьютекс
        size_t s = _queue.size(); // получаем размер очереди 
        _mutex.unlock(); // разблокируем мьютекс
        return s; // возвращаем размер
    }
};