#include <gtest/gtest.h>
#include <memory>
#include <string>

// Подключаем твои заголовки
#include "BaseManager.hpp"
#include "BaseModule.hpp"

// ========== Тестовые события ==========
enum class TestEventType : uint32_t {
    EVENT_A = 1,
    EVENT_B = 2
};

class EventA : public BaseEvent {
public:
    EventA() : BaseEvent(static_cast<EventType>(TestEventType::EVENT_A)) {}
    std::string to_string() const override { return "EventA"; }
};

class EventB : public BaseEvent {
public:
    EventB() : BaseEvent(static_cast<EventType>(TestEventType::EVENT_B)) {}
    std::string to_string() const override { return "EventB"; }
};

// ========== Тестовые модули ==========
class TestModuleA : public BaseModule {
public:
    int received_A = 0;
    int received_B = 0;

    TestModuleA(size_t id) : BaseModule(id, "TestModuleA") {}

    void init_subscribes() override {
        subscribe<EventA>([this](auto) { received_A++; });
        // НЕ подписываемся на EventB
    }

    void send_A() {
        send(std::make_shared<EventA>());
    }
};

class TestModuleB : public BaseModule {
public:
    int received_A = 0;
    int received_B = 0;

    TestModuleB(size_t id) : BaseModule(id, "TestModuleB") {}

    void init_subscribes() override {
        subscribe<EventB>([this](auto) { received_B++; });
        // НЕ подписываемся на EventA
    }

    void send_B() {
        send(std::make_shared<EventB>());
    }
};

// ========== ТЕСТ 1: Базовая доставка ==========
TEST(BrokerTest, BasicDelivery) {
    BaseManager manager;

    TestModuleA modA(1);
    TestModuleB modB(2);

    // Регистрация
    modA.set_connector(manager.register_module());
    modA.set_manager(&manager);
    modB.set_connector(manager.register_module());
    modB.set_manager(&manager);

    modA.init_subscribes();
    modB.init_subscribes();

    // Отправка
    modA.send_A(); // A отправляет EventA
    modB.send_B(); // B отправляет EventB

    // Обработка
    manager.process_all_messages();

    // Получение
    modA.process_incoming_events();
    modB.process_incoming_events();

    // Проверки
    EXPECT_EQ(modA.received_A, 1); // A получил свой EventA
    EXPECT_EQ(modA.received_B, 0); // A НЕ получил EventB

    EXPECT_EQ(modB.received_B, 1); // B получил свой EventB
    EXPECT_EQ(modB.received_A, 0); // B НЕ получил EventA
}

// ========== ТЕСТ 2: Переполнение очереди ==========
TEST(BrokerTest, QueueOverflow) {
    BaseManager manager;

    TestModuleA sender(1);
    TestModuleA receiver(2);

    // Регистрация с ОЧЕНЬ МАЛЕНЬКОЙ очередью (размер 1)
    sender.set_connector(manager.register_module(1));
    sender.set_manager(&manager);
    receiver.set_connector(manager.register_module(1));
    receiver.set_manager(&manager);

    receiver.init_subscribes(); // подписывается на EventA
    sender.init_subscribes();

    // Отправим 3 события, но очередь — на 1
    sender.send_A();
    sender.send_A();
    sender.send_A();

    manager.process_all_messages();
    receiver.process_incoming_events();

    // Должно прийти ТОЛЬКО 1 событие (остальные пропущены)
    EXPECT_LE(receiver.received_A, 1);
    EXPECT_GE(receiver.received_A, 1); // и хотя бы 1 должно дойти
}

// ========== ТЕСТ 3: Динамическая подписка ==========
TEST(BrokerTest, DynamicSubscription) {
    BaseManager manager;

    TestModuleA mod(1);
    mod.set_connector(manager.register_module());
    mod.set_manager(&manager);

    // Изначально НЕ вызываем init_subscribes() → нет подписки

    // Отправим событие
    auto event = std::make_shared<EventA>();
    mod.send(event);

    manager.process_all_messages();
    mod.process_incoming_events();

    EXPECT_EQ(mod.received_A, 0); // не получил, потому что не подписан

    // Теперь подписываемся
    mod.init_subscribes();

    // Отправим ещё раз
    mod.send_A();

    manager.process_all_messages();
    mod.process_incoming_events();

    EXPECT_EQ(mod.received_A, 1); // теперь получил!
}