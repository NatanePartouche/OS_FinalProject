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
    while (_running) { // Continue tant que `_running` est `true`.
        Task task; // Conteneur pour stocker la tâche à exécuter.

        {
            std::unique_lock<std::mutex> lock(_queue_mutex); // Verrouille l'accès à la file des tâches.

            // Attend qu'une tâche soit disponible ou qu'un signal d'arrêt soit émis.
            _cv.wait(lock, [this]() {
                return !_running || !_task_queue.empty(); // Condition de réveil : arrêt ou tâche disponible.
            });

            // Si le pool est arrêté et qu'il n'y a plus de tâches, quitter la boucle.
            if (!_running && _task_queue.empty()) return;

            // Début de la définition du thread leader
            if (!_leader_active.exchange(true)) { // Si aucun autre thread n'est leader, ce thread devient leader.
                {
                    std::lock_guard<std::mutex> log_lock(_log_mutex); // Verrouille l'accès aux logs.
                    std::ostringstream oss;
                    oss << "[LeaderFollowers] Thread " << std::this_thread::get_id() << " became leader.";
                    std::cout << oss.str() << std::endl; // Affiche que ce thread est devenu leader.
                }

                if (!_task_queue.empty()) { // Vérifie si une tâche est disponible.
                    task = std::move(_task_queue.front()); // Récupère la tâche en tête de file.
                    _task_queue.pop(); // Supprime la tâche de la file.
                }

                _leader_active = false; // Libère le rôle de leader pour permettre à d'autres threads de devenir leader.
            }
        }

        // Début de l'exécution de la tâche par le leader
        if (task) { // Si une tâche a été récupérée.
            try {
                {
                    std::lock_guard<std::mutex> log_lock(_log_mutex); // Verrouille l'accès aux logs.
                    std::ostringstream oss;
                    oss << "[LeaderFollowers] Thread " << std::this_thread::get_id() << " is executing a task.";
                    std::cout << oss.str() << std::endl; // Affiche que ce thread exécute une tâche.
                }
                task(); // Exécute la tâche.
            } catch (const std::exception& e) { // Capture les exceptions levées par la tâche.
                std::lock_guard<std::mutex> log_lock(_log_mutex); // Verrouille les logs pour signaler une erreur.
                std::cerr << "[LeaderFollowers] Task exception: " << e.what() << std::endl; // Affiche l'erreur.
            }
        }

        // Fin de l'exécution de la tâche par le leader
        _cv.notify_one(); // Réveille un autre thread en attente pour qu'il devienne leader.
    }
}