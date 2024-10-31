#ifndef ACTIVEOBJECT_HPP
#define ACTIVEOBJECT_HPP

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ActiveObject {
public:
    ActiveObject();
    ~ActiveObject();

    void enqueueTask(std::function<void()> task);
    void stop();

private:
    void processTasks();

    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::thread worker;
};

#endif // ACTIVEOBJECT_HPP