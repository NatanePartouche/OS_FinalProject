#include "ActiveObject.hpp" // Inclut le fichier d'en-tête contenant la déclaration de la classe ActiveObject

// Constructeur de la classe ActiveObject
ActiveObject::ActiveObject() : running(false), processing(false) {
    // Initialise les variables atomiques `running` et `processing` à `false`.
    // Cela indique que l'objet n'est pas encore actif et qu'aucune tâche n'est en cours.
}

// Destructeur de la classe ActiveObject
ActiveObject::~ActiveObject() {
    stop(); // Assure que l'objet est proprement arrêté avant sa destruction
}

// Méthode pour ajouter une tâche à la file
void ActiveObject::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex); // Verrouille l'accès à la file pour éviter les conditions de course
        taskQueue.push(std::move(task)); // Ajoute la tâche à la file de manière sécurisée
    }
    condition.notify_one(); // Réveille le thread de travail si nécessaire, car une nouvelle tâche est disponible
}

// Méthode pour démarrer l'objet actif
void ActiveObject::start() {
    running = true; // Indique que l'objet est actif et prêt à traiter des tâches
    workerThread = std::thread(&ActiveObject::run, this); // Lance un thread dédié exécutant la méthode `run`
}

// Méthode pour arrêter l'objet actif
void ActiveObject::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex); // Verrouille l'accès à la file
        running = false;                                  // Indique que l'objet ne doit plus accepter de nouvelles tâches
        condition.notify_all();                           // Réveille le thread de travail pour terminer sa boucle d'exécution

        // Attend que toutes les tâches en attente soient exécutées avant de terminer
        condition.wait(lock, [this]() {
            return taskQueue.empty() && !processing; // Continue d'attendre si des tâches sont encore en cours
        });
    }

    // Joindre le thread de travail pour attendre sa fin
    if (workerThread.joinable()) {
        workerThread.join(); // Termine proprement le thread une fois que toutes les tâches sont traitées
    }
}

// Fonction principale exécutée par le thread de travail
void ActiveObject::run() {
    // Continue tant que `running` est actif ou que la file de tâches n'est pas vide
    while (running || !taskQueue.empty()) {
        std::function<void()> task; // Variable pour stocker la tâche à exécuter
        {
            std::unique_lock<std::mutex> lock(queueMutex); // Verrouille l'accès à la file
            condition.wait(lock, [this]() {
                // Attend qu'une tâche soit disponible ou que l'objet ne soit plus actif
                return !taskQueue.empty() || !running;
            });

            // Si l'objet n'est plus actif (`running` est faux) et que la file est vide, sortir de la boucle
            if (!running && taskQueue.empty())
                break;

            // Récupère la tâche en tête de la file et la supprime de la file
            task = std::move(taskQueue.front());
            taskQueue.pop();
            processing = true; // Indique qu'une tâche est en cours d'exécution
        }

        // Exécute la tâche si elle est valide
        if (task) {
            task(); // Appelle la tâche encapsulée dans le std::function
        }

        // Vérifie si toutes les tâches sont terminées
        {
            std::lock_guard<std::mutex> lock(queueMutex); // Verrouille l'accès à la file
            if (taskQueue.empty()) {    // Si la file est vide
                processing = false;     // Indique qu'il n'y a plus de tâche en cours
                condition.notify_all(); // Notifie les threads en attente que tout est terminé
            }
        }
    }
}