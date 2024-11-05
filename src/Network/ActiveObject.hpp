#ifndef ACTIVEOBJECT_HPP
#define ACTIVEOBJECT_HPP

#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

// The ActiveObject class provides a thread pool to execute tasks asynchronously.
// Tasks can be enqueued and are handled by multiple worker threads.
class ActiveObject {
public:
    // Constructor: Initializes the ActiveObject with a specified number of threads.
    // Each thread will process tasks from the task queue.
    ActiveObject(int numThreads);

    // Destructor: Ensures that all threads are properly stopped and cleaned up.
    ~ActiveObject();

    // Adds a new task to the queue. Each task is a function that will be executed
    // by one of the worker threads when available.
    void enqueueTask(std::function<void()> task);

    // Shuts down the ActiveObject, stopping all threads gracefully and clearing
    // remaining tasks if cancelingTasks is set.
    void shutdown();

private:
    // The function executed by each worker thread. It continuously checks the task
    // queue for new tasks and executes them as long as the ActiveObject is running.
    void workerThread();
    // Vector of worker threads in the pool.
    std::vector<std::thread> workers;
    // Queue of tasks to be executed by the worker threads.
    std::queue<std::function<void()>> tasks;
    // Mutex to synchronize access to the task queue.
    std::mutex mtx;
    // Condition variable to notify worker threads when tasks are available or
    // when shutdown is requested.
    std::condition_variable cv;
    // Flag to indicate if the ActiveObject is currently running.
    bool running;
    // Flag to determine if remaining tasks should be canceled during shutdown.
    bool cancelingTasks;
};

#endif // ACTIVEOBJECT_HPP