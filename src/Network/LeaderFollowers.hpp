#include <vector>               // Pour utiliser std::vector pour gérer les threads
#include <queue>                // Pour std::queue pour la file des tâches
#include <thread>               // Pour std::thread pour gérer les threads
#include <mutex>                // Pour std::mutex pour la synchronisation
#include <condition_variable>   // Pour std::condition_variable pour réveiller les threads
#include <atomic>               // Pour std::atomic pour des opérations atomiques
#include <functional>           // Pour std::function pour représenter les tâches

/**
 * @class LeaderFollowers
 * @brief Implémente le modèle de gestion des threads "Leader/Followers".
 *
 * Ce modèle repose sur une répartition dynamique des rôles entre les threads.
 * - Un thread devient "leader" pour exécuter une tâche, tandis que les autres
 *   threads restent en veille et tentent de devenir leader lorsqu'il se libère.
 */
class LeaderFollowers {
public:

    using Task=std::function<void()>;        // Alias pour représenter une tâche : une fonction sans argument ni retour.
    std::queue<Task>         _task_queue;    // File contenant les tâches en attente d'exécution
    std::vector<std::thread> _threads;       // Liste des threads du pool
    std::mutex               _queue_mutex;   // Mutex pour protéger l'accès à la file des tâches
    std::condition_variable  _cv;            // Condition variable pour synchroniser les threads
    std::atomic<bool>        _running;       // Indique si le pool est actif ou arrêté
    std::atomic<bool>        _leader_active; // Indique si un thread est actuellement leader
    std::mutex               _log_mutex;     // Mutex pour protéger les affichages console/logs


    /**
     * @brief Constructeur.
     * Initialise le pool de threads avec un nombre spécifié de threads.
     *
     * @param num_threads Nombre de threads à créer dans le pool.
     */
    explicit LeaderFollowers(int num_threads);

    /**
     * @brief Destructeur.
     * Arrête proprement le pool de threads et libère les ressources.
     */
    ~LeaderFollowers();

    /**
     * @brief Ajoute une tâche à la file d'attente.
     *
     * Cette méthode est thread-safe grâce à un verrou (`std::mutex`).
     *
     * @param task La tâche à ajouter.
     */
    void add_task(const Task& task);

    /**
     * @brief Arrête le pool de threads.
     *
     * Cette méthode réveille tous les threads pour qu'ils quittent leur boucle principale.
     */
    void stop();

    /**
     * @brief Boucle principale exécutée par chaque thread.
     *
     * Les threads tentent de devenir leader pour exécuter une tâche.
     * Ceux qui ne sont pas leaders restent en veille jusqu'à ce qu'une tâche soit disponible.
     */
    void worker_loop();
};