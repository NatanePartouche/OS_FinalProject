#include "LeaderFollowers.hpp"
#include <iostream>
#include <sstream>

/**
 * @brief Constructeur.
 * Initialise les variables internes et crée les threads du pool.
 *
 * @param num_threads Nombre de threads dans le pool.
 */
LeaderFollowers::LeaderFollowers(int num_threads) : _running(true), _leader_active(false) {
    // Crée `num_threads` threads qui exécuteront tous la méthode `worker_loop`.
    for (int i = 0; i < num_threads; ++i) {
        _threads.emplace_back(&LeaderFollowers::worker_loop, this); // Ajoute un thread dans le pool.
    }
}

/**
 * @brief Destructeur.
 *
 * Appelle la méthode `stop` pour s'assurer que tous les threads s'arrêtent proprement.
 */
LeaderFollowers::~LeaderFollowers() {
    stop(); // Arrête proprement tous les threads avant la destruction.
}

/**
 * @brief Ajoute une tâche dans la file d'attente.
 *
 * Cette méthode utilise un mutex pour garantir que l'accès à la file est thread-safe.
 *
 * @param task La tâche à ajouter.
 */
void LeaderFollowers::add_task(const Task& task) {
    {
        std::lock_guard<std::mutex> lock(_queue_mutex); // Verrouille l'accès à la file des tâches.
        _task_queue.push(task); // Ajoute la tâche à la file d'attente.
    }
    _cv.notify_one(); // Réveille un thread en attente pour qu'il traite la tâche.
}

/**
 * @brief Arrête proprement le pool de threads.
 *
 * Réveille tous les threads pour qu'ils quittent leur boucle principale
 * et attend leur terminaison avec `join`.
 */
void LeaderFollowers::stop() {
    {
        std::lock_guard<std::mutex> lock(_queue_mutex); // Verrouille pour protéger la variable `_running`.
        _running = false; // Signale que le pool de threads doit s'arrêter.
    }
    _cv.notify_all(); // Réveille tous les threads qui attendent dans `worker_loop`.

    // Parcourt tous les threads du pool pour les arrêter proprement.
    for (auto& thread : _threads) {
        if (thread.joinable()) { // Vérifie que le thread est toujours actif.
            thread.join(); // Attend la fin de l'exécution du thread.
        }
    }
}

/**
 * @brief Boucle principale des threads.
 *
 * Chaque thread :
 * - Attend une tâche ou un signal d'arrêt.
 * - Tente de devenir leader pour exécuter une tâche.
 * - Libère le rôle de leader après exécution.
 */
void LeaderFollowers::worker_loop() {
    while (_running) {
        Task task;

        {
            std::unique_lock<std::mutex> lock(_queue_mutex);

            // Attend qu'une tâche soit disponible ou qu'un signal d'arrêt soit émis.
            _cv.wait(lock, [this]() {
                return !_running || !_task_queue.empty();
            });

            // Si le pool est arrêté et qu'il n'y a plus de tâches, on quitte.
            if (!_running && _task_queue.empty())
                return;

            // Tente de devenir leader si aucun autre ne l'est.
            if (!_leader_active.exchange(true)) {
                {
                    std::lock_guard<std::mutex> log_lock(_log_mutex);
                    std::ostringstream oss;
                    oss << "[LeaderFollowers] Thread " << std::this_thread::get_id() << " became leader.";
                    std::cout << oss.str() << std::endl;
                }

                if (!_task_queue.empty()) {
                    task = std::move(_task_queue.front());
                    _task_queue.pop();
                }

                // IMPORTANT : Ne pas relâcher le leadership ici ni notifier d'autres threads.
                // On reste leader jusqu'à l'exécution complète de la tâche.
            } else {
                // Si on n'est pas devenu leader, continuer la boucle (attendre une autre opportunité).
                // Le thread leader actuel va exécuter la tâche puis notifier un autre thread.
                continue;
            }
        } // Le mutex `_queue_mutex` est libéré ici.

        // Exécution de la tâche en dehors de la section critique
        if (task) {
            try {
                {
                    std::lock_guard<std::mutex> log_lock(_log_mutex);
                    std::ostringstream oss;
                    oss << "[LeaderFollowers] Thread " << std::this_thread::get_id() << " is executing a task.";
                    std::cout << oss.str() << std::endl;
                }
                task();
            } catch (const std::exception &e) {
                std::lock_guard<std::mutex> log_lock(_log_mutex);
                std::cerr << "[LeaderFollowers] Task exception: " << e.what() << std::endl;
            }
        }

        // Une fois la tâche exécutée, rendre le leadership et notifier un autre thread.
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _leader_active = false;
            _cv.notify_one();
        }
    }
}
