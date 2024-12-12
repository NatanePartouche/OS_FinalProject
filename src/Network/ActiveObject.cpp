#include "ActiveObject.hpp" // Includes the header file containing the declaration of the ActiveObject class

// Constructor of the ActiveObject class
ActiveObject::ActiveObject() : running(false), processing(false) {
    // Initializes the atomic variables `running` and `processing` to `false`.
    // This indicates that the object is not active yet and no tasks are being processed.
}

// Destructor of the ActiveObject class
ActiveObject::~ActiveObject() {
    stop(); // Ensures that the object is properly stopped before destruction.
}

// Method to add a task to the queue
void ActiveObject::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex); // Locks access to the queue to avoid race conditions.
        taskQueue.push(std::move(task)); // Safely adds the task to the queue.
    }
    condition.notify_one(); // Notifies the worker thread that a new task is available.
}

// Method to start the active object
void ActiveObject::start() {
    running = true; // Marks the object as active and ready to process tasks.
    workerThread = std::thread(&ActiveObject::run, this); // Launches a dedicated thread running the `run` method.
}

// Method to stop the active object
void ActiveObject::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex); // Locks access to the queue.
        running = false; // Marks the object as inactive, preventing new tasks from being accepted.
        condition.notify_all(); // Wakes up the worker thread to allow it to exit the execution loop.

        // Waits until all pending tasks are completed before terminating.
        condition.wait(lock, [this]() {
            return taskQueue.empty() && !processing; // Waits if there are still tasks being processed.
        });
    }

    // Joins the worker thread to ensure it has finished before stopping.
    if (workerThread.joinable()) {
        workerThread.join(); // Properly terminates the thread after processing all tasks.
    }
}

// Main function executed by the worker thread
void ActiveObject::run() {
    // Continues as long as `running` is true or there are tasks in the queue.
    while (running || !taskQueue.empty()) {
        std::function<void()> task; // Variable to store the task to execute.
        {
            std::unique_lock<std::mutex> lock(queueMutex); // Locks access to the queue.
            condition.wait(lock, [this]() {
                // Waits for a task to become available or for the object to stop.
                return !taskQueue.empty() || !running;
            });

            // If the object is no longer active (`running` is false) and the queue is empty, exit the loop.
            if (!running && taskQueue.empty())
                break;

            // Retrieves the task at the front of the queue and removes it from the queue.
            task = std::move(taskQueue.front());
            taskQueue.pop();
            processing = true; // Marks that a task is being processed.
        }

        // Executes the task if it is valid.
        if (task) {
            task(); // Calls the encapsulated task in the std::function.
        }

        // Checks if all tasks are completed.
        {
            std::lock_guard<std::mutex> lock(queueMutex); // Locks access to the queue.
            if (taskQueue.empty()) {    // If the queue is empty.
                processing = false;     // Marks that there are no tasks being processed.
                condition.notify_all(); // Notifies waiting threads that all tasks are finished.
            }
        }
    }
}