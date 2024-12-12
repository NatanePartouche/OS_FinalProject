#include <vector>               // For std::vector to manage threads
#include <queue>                // For std::queue to handle the task queue
#include <thread>               // For std::thread to create and manage threads
#include <mutex>                // For std::mutex to ensure thread synchronization
#include <condition_variable>   // For std::condition_variable to notify threads
#include <atomic>               // For std::atomic to handle atomic operations
#include <functional>           // For std::function to represent tasks

/**
 * @class LeaderFollowers
 * @brief Implements the "Leader/Followers" thread management pattern.
 *
 * This pattern dynamically assigns roles to threads:
 * - One thread becomes the "leader" to execute a task, while others
 *   remain idle ("followers"). When the leader is done, another thread
 *   takes over as the leader.
 */
class LeaderFollowers {
public:
    using Task = std::function<void()>;      // Alias for a task: a function with no arguments or return value.

    std::queue<Task>         _task_queue;    // Queue to hold tasks waiting to be executed.
    std::vector<std::thread> _threads;       // Vector holding all the threads in the pool.
    std::mutex               _queue_mutex;   // Mutex to protect access to the task queue.
    std::condition_variable  _cv;            // Condition variable to synchronize threads and signal when tasks are available.
    std::atomic<bool>        _running;       // Flag indicating whether the thread pool is running or stopped.
    std::atomic<bool>        _leader_active; // Flag indicating if a thread is currently acting as the leader.
    std::mutex               _log_mutex;     // Mutex to protect console/log outputs from concurrent access.

    /**
     * @brief Constructor.
     * Initializes the thread pool with the specified number of threads.
     *
     * @param num_threads Number of threads to create in the pool.
     */
    explicit LeaderFollowers(int num_threads);

    /**
     * @brief Destructor.
     * Properly stops the thread pool and releases resources.
     */
    ~LeaderFollowers();

    /**
     * @brief Adds a task to the task queue.
     *
     * This method is thread-safe due to the use of a mutex (`std::mutex`).
     *
     * @param task The task to add to the queue.
     */
    void add_task(const Task& task);

    /**
     * @brief Stops the thread pool.
     *
     * This method signals all threads to exit their main loop, effectively shutting down the pool.
     */
    void stop();

    /**
     * @brief Main loop executed by each thread.
     *
     * Each thread attempts to become the "leader" to execute tasks.
     * Threads that are not leaders remain idle until a task becomes available.
     */
    void worker_loop();
};