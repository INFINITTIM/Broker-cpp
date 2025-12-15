#pragma once

#include <thread>
#include <functional>
#include <iostream>
#include "BaseManager.hpp"

class Thread {
private:
    std::thread _thread;
    bool _running = false;

public:
    Thread() = default;
    
    ~Thread() {
        stop();
    }
    
    // Запуск модуля
    template<typename ModuleType>
    ModuleType* start_module(ModuleType* module) {
        if (_running) return module;
        
        _running = true;
        
        _thread = std::thread([this, module]() {
            module->init_subscribes();
            
            while (_running) {
                module->process_incoming_events();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
        
        return module;
    }
    
    // Запуск менеджера
    BaseManager* start_manager(BaseManager* manager) {
        if (_running) return manager;
        
        _running = true;
        
        _thread = std::thread([this, manager]() {
            while (_running) {
                manager->process_all_messages();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
        
        return manager;
    }
    
    void stop() {
        if (!_running) return;
        
        _running = false;
        
        if (_thread.joinable()) {
            _thread.join();
        }
    }
    
    bool is_running() const { return _running; }
};