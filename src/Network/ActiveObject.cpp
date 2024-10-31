#include "ActiveObject.hpp"
#include <iostream>
#include <chrono>

ActiveObject::ActiveObject() : running(true), worker(&ActiveObject::processTasks, this) {}

ActiveObject::~ActiveObject() {
    stop();
    if (worker.joinable()) {worker.join();}
}

void ActiveObject::enqueueTask(std::function<void()> task) {
    auto start = std::chrono::high_resolution_clock::now(); // Horodatage du début

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.push([task, start]() {  // Emballage de la tâche pour mesurer le délai
            auto end = std::chrono::high_resolution_clock::now();
            auto delay = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "[processTasks] Tâche exécutée après un délai de " << delay << " µs." << std::endl;
            task();
        });
    }
    std::cout << "[enqueueTask] Tâche ajoutée à la file." << std::endl;
    cv.notify_one();
}

void ActiveObject::stop() {
    running = false;
    cv.notify_all();
}

void ActiveObject::processTasks() {
    while (running) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this] { return !tasks.empty() || !running; });
            if (!running && tasks.empty()) return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        if (task) task();
    }
}