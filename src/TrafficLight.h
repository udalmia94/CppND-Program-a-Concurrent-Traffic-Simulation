#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <chrono>
#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle; 

template <class T>
class MessageQueue
{
public:
    void send(T&& msg);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mtx;
};

enum class TrafficLightPhase {
    red,
    green
};

class TrafficLight : public TrafficObject {
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase() { return _currentPhase; };

    // typical behaviour methods
    void simulate();
    void waitForGreen();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _queue;
    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif
