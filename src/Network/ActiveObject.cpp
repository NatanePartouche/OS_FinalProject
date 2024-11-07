#include "ActiveObject.hpp"

// Constructor: Initializes the ActiveObject and launches the specified number of worker threads.
ActiveObject::ActiveObject(int numThreads) : running(true), cancelingTasks(false) {
    // Launch the specified number of worker threads
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ActiveObject::workerThread, this);
    }
}

// Destructor: Ensures that all threads are properly shut down.
ActiveObject::~ActiveObject() {
    shutdown(); // Ensure that threads are properly shut down before destruction
}

// Adds a new task to the task queue if the ActiveObject is running and not shutting down.
void ActiveObject::enqueueTask(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(mtx);
    // Only enqueue tasks if the object is active and not canceling tasks
    if (running && !cancelingTasks) {
        tasks.push(std::move(task)); // Move the task into the queue
        cv.notify_one(); // Notify one waiting thread that a new task is available
    }
}

// Function executed by each worker thread to process tasks from the queue.
void ActiveObject::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            // Wait for a task to be available, shutdown signal, or task cancellation
            cv.wait(lock, [this]() { return !tasks.empty() || !running || cancelingTasks; });

            // If shutting down or canceling tasks, exit the loop
            if (!running || cancelingTasks) {
                break;
            }
            if (!tasks.empty()) {
                // Retrieve and remove the next task from the queue
                task = std::move(tasks.front());
                tasks.pop();
            }
        }

        // Execute the task outside the locked section
        if (task) { task(); }
    }
}

// Shuts down all worker threads gracefully by stopping new tasks and joining threads.
void ActiveObject::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        running = false; // Signal all threads to stop processing tasks
    }
    cv.notify_all(); // Wake up all waiting threads to allow them to exit
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join(); // Join each thread to ensure graceful shutdown
        }
    }
}