#include "LeaderFollowers.hpp" // Include the header file for the LeaderFollowers class
#include <iostream>
#include <sstream>

/**
 * @brief Constructor.
 * Initializes internal variables and creates the thread pool.
 *
 * @param num_threads Number of threads in the pool.
 */
LeaderFollowers::LeaderFollowers(int num_threads): _running(true),_leader_active(false) {
    // Loop to create `num_threads` threads
    for (int i = 0; i < num_threads; ++i) {
        // Create a new thread that will execute the `worker_loop` method of this instance
        _threads.emplace_back(&LeaderFollowers::worker_loop, this);
        // `emplace_back` adds the thread to the `_threads` vector
        // `&LeaderFollowers::worker_loop` is a pointer to the member function `worker_loop`
        // `this` is a pointer to the current instance of LeaderFollowers
    }
}

/**
 * @brief Destructor.
 *
 * Calls the `stop` method to ensure all threads are properly stopped.
 */
LeaderFollowers::~LeaderFollowers() {
    stop(); // Call `stop` to gracefully terminate all threads before destroying the object
}

/**
 * @brief Adds a task to the task queue.
 *
 * This method uses a mutex to ensure thread-safe access to the queue.
 *
 * @param task The task to add.
 */
void LeaderFollowers::add_task(const Task& task) {
    {
        std::lock_guard<std::mutex> lock(_queue_mutex); // Lock access to the task queue `_task_queue`
        _task_queue.push(task); // Add the task to the task queue `_task_queue`
    } // The lock is automatically released here due to the `lock_guard` going out of scope

    _cv.notify_one(); // Notify one waiting thread to process the added task
}

/**
 * @brief Gracefully stops the thread pool.
 *
 * Notifies all threads to exit their main loop and waits for their termination using `join`.
 */
void LeaderFollowers::stop() {
    {
        std::lock_guard<std::mutex> lock(_queue_mutex); // Lock `_queue_mutex` to protect the `_running` variable
        _running = false; // Signal that the thread pool should stop by setting `_running` to false
    } // The lock is released here

    _cv.notify_all(); // Wake up all threads waiting on the condition variable `_cv`

    // Iterate over all threads in the pool to gracefully stop them
    for (auto& thread : _threads) {
        if (thread.joinable()) { // Check if the thread is still active and can be joined
            thread.join(); // Wait for the thread to finish execution
            // The join() function is a member of the std::thread class in the <thread> header.
            // It is used to wait for a thread to complete its execution.
            // When join() is called on a std::thread object, it blocks the calling thread until the thread represented by the std::thread object finishes executing
        }
    }
}

/**
 * @brief Main loop for each thread.
 *
 * Each thread:
 * - Waits for a task or a stop signal.
 * - Attempts to become the leader to execute a task.
 * - Releases the leader role after execution.
 */
void LeaderFollowers::worker_loop() {
    while (_running) { // Loop as long as the pool is active
        Task task; // Variable to store the task to execute

        {
            std::unique_lock<std::mutex> lock(_queue_mutex); // Acquire a unique lock on `_queue_mutex`

            // Wait until a task is available or a stop signal is received
            // `wait` releases the lock and blocks the thread until a notification is received
            // The lambda checks if the pool is stopped or if the task queue is not empty
            _cv.wait(lock, [this]() {
                return !_running || !_task_queue.empty();
            });

            // If the pool is stopped and there are no more tasks, exit the loop and terminate the thread
            if (!_running && _task_queue.empty())
                return;

            // Attempt to become the leader if no other thread is currently the leader
            // `_leader_active` is an atomic variable, `exchange(true)` tries to set it to true and returns the previous value
            // Uses the exchange() method from the std::atomic class to atomically check and update the value of _leader_active.
            if (!_leader_active.exchange(true)) { // If no active leader, this thread becomes the leader
                {
                    std::lock_guard<std::mutex> log_lock(_log_mutex); // Lock `_log_mutex` to protect log access
                    std::ostringstream oss;
                    oss << "[LeaderFollowers] Thread " << std::this_thread::get_id() << " became leader.";
                    std::cout << oss.str() << std::endl; // Log that this thread has become the leader
                }

                if (!_task_queue.empty()) { // Check again if the task queue is not empty
                    task = std::move(_task_queue.front()); // Retrieve the first task from the queue
                    _task_queue.pop(); // Remove the task from the queue
                }

                // IMPORTANT: Do not release leadership here or notify other threads.
                // Remain the leader until the task is fully executed.
            }
            else {
                _cv.notify_one(); // Wake up another thread to retry.
                continue; // Wait for the next opportunity to become the leader.
            }
        } // The `_queue_mutex` lock is released here

        // Execute the task outside of the critical section to avoid blocking access to the queue
        if (task) { // Check if a task was retrieved
            try {
                {
                    std::lock_guard<std::mutex> log_lock(_log_mutex); // Lock `_log_mutex` to protect log access
                    std::ostringstream oss;
                    oss << "[LeaderFollowers] Thread " << std::this_thread::get_id() << " is executing a task.";
                    std::cout << oss.str() << std::endl; // Log that this thread is executing a task
                }
                task(); // Execute the task (call the callable)
            } catch (const std::exception &e) { // Catch any exceptions thrown during task execution
                std::lock_guard<std::mutex> log_lock(_log_mutex); // Lock `_log_mutex` to protect log access
                std::cerr << "[LeaderFollowers] Task exception: " << e.what() << std::endl; // Log the exception
            }
        }

        // After executing the task, release leadership and notify another thread
        {
            std::unique_lock<std::mutex> lock(_queue_mutex); // Acquire a unique lock on `_queue_mutex`
            _leader_active = false; // Indicate that there is no active leader
            _cv.notify_one(); // Notify another waiting thread so it can become the leader
        }
    }
}