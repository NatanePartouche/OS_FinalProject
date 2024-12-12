#ifndef ACTIVEOBJECT_HPP
#define ACTIVEOBJECT_HPP

// Inclusion guard to prevent multiple inclusions of this header file.
// If the symbol `ACTIVEOBJECT_HPP` is not defined, define it now.
// Ensures that this file is included only once in each compilation unit.

#include <queue>                  // Used to manage the task queue (std::queue).
#include <thread>                 // Provides std::thread for thread management.
#include <mutex>                  // Provides std::mutex to protect shared resources.
#include <condition_variable>     // Provides std::condition_variable for thread synchronization.
#include <functional>             // Provides std::function to encapsulate tasks.
#include <atomic>                 // Provides std::atomic for thread-safe variables.

// Declaration of the `ActiveObject` class
class ActiveObject {
private:
    std::queue<std::function<void()>> taskQueue; // Queue to store tasks to be executed.
                                                 // Each task is encapsulated in std::function<void()>,
                                                 // allowing any compatible function or lambda to be managed.

    std::mutex queueMutex;                      // Mutex to protect concurrent access to the task queue.
                                                 // Prevents race conditions when multiple threads
                                                 // add or remove tasks simultaneously.

    std::condition_variable condition;          // Condition variable to pause and wake up the worker thread.
                                                 // Used to avoid busy waiting (active spinning).

    std::thread workerThread;                   // Dedicated thread to execute tasks.
                                                 // Fetches tasks from the queue and executes them.

    std::atomic<bool> running;                  // Indicates whether the active object is running.
                                                 // Used to control the lifecycle of the worker thread
                                                 // in a thread-safe manner.

    std::atomic<bool> processing;               // Indicates whether a task is currently being processed.
                                                 // Helps determine if the thread is busy.

    void run();                                  // Main private method executed by the worker thread.
                                                 // Contains the task processing loop.

public:
    ActiveObject();                              // Constructor: Initializes control variables (running, processing).
    ~ActiveObject();                             // Destructor: Stops the worker thread gracefully if it is still active.

    void enqueue(std::function<void()> task);    // Method to add a task to the queue.
                                                 // A task is encapsulated in std::function<void()>.

    void start();                                // Method to start the active object.
                                                 // Launches the worker thread and begins task processing.

    void stop();                                 // Method to stop the active object.
                                                 // Ensures all queued tasks are completed before
                                                 // shutting down the thread gracefully.
};

#endif // ACTIVEOBJECT_HPP