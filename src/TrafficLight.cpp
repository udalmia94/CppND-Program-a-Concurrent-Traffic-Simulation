#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mtx);
    _cond.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg; 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> uLock(_mtx);
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    TrafficLightPhase msg;

    while(true) {
        msg = _queue.receive();
        if (msg == TrafficLightPhase::green) { return; }
    }
}

void TrafficLight::simulate()
{
    // launch cycleThroughPhases in a thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::random_device rd;
    std::mt19937 e{rd()};
    std::uniform_real_distribution<double> dist{4000, 6000};

    auto start_time = std::chrono::system_clock::now();
    std::chrono::duration<double> cycle_time = std::chrono::milliseconds((int)dist(e));

    // Infinite loop that cycles through red and green lights
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if ((std::chrono::system_clock::now() - start_time) < cycle_time)
            continue;
        if (_currentPhase == TrafficLightPhase::red) {
            _currentPhase = TrafficLightPhase::green;
        } else {
            _currentPhase = TrafficLightPhase::red;
        }
        _queue.send(std::move(_currentPhase));
        cycle_time = std::chrono::milliseconds((int)dist(e));
        start_time = std::chrono::system_clock::now();
    }
}
