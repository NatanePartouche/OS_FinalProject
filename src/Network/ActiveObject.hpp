#ifndef ACTIVEOBJECT_HPP
#define ACTIVEOBJECT_HPP

// Les directives d'inclusion conditionnelle empêchent les inclusions multiples du fichier d'en-tête.
// Si le symbole `ACTIVEOBJECT_HPP` n'est pas défini, il est défini ici.
// Cela garantit que ce fichier n'est inclus qu'une seule fois dans chaque unité de compilation.

#include <queue>                  // Utilisé pour gérer la file de tâches (std::queue)
#include <thread>                 // Fournit la classe std::thread pour la gestion des threads
#include <mutex>                  // Fournit std::mutex pour protéger les ressources partagées
#include <condition_variable>     // Fournit std::condition_variable pour synchroniser les threads
#include <functional>             // Fournit std::function pour encapsuler les tâches
#include <atomic>                 // Fournit std::atomic pour des variables thread-safe

// Déclaration de la classe `ActiveObject`
class ActiveObject {
private:
    std::queue<std::function<void()>> taskQueue; // File pour stocker les tâches à exécuter.
                                                 // Chaque tâche est encapsulée dans un std::function<void()>,
                                                 // ce qui permet de gérer n'importe quelle fonction ou lambda compatible.

    std::mutex queueMutex;                      // Mutex pour protéger l'accès concurrent à la file de tâches.
                                                 // Empêche les conditions de course lorsque plusieurs threads
                                                 // ajoutent ou suppriment des tâches simultanément.

    std::condition_variable condition;          // Condition pour mettre en pause et réveiller le thread de travail.
                                                 // Utilisé pour éviter le busy waiting (attente active).

    std::thread workerThread;                   // Le thread dédié qui exécute les tâches.
                                                 // Il récupère les tâches de la file et les exécute.

    std::atomic<bool> running;                  // Indique si l'objet actif est en cours d'exécution.
                                                 // Utilisé pour contrôler le cycle de vie du thread de travail
                                                 // de manière thread-safe.

    std::atomic<bool> processing;               // Indique si une tâche est actuellement en cours de traitement.
                                                 // Cela permet de savoir si le thread est occupé.

    void run();                                  // Méthode privée principale exécutée par le thread.
                                                 // Elle contient la boucle de traitement des tâches.

public:
    ActiveObject();  // Constructeur : initialise les variables de contrôle (running, processing).
    ~ActiveObject(); // Destructeur : arrête proprement le thread de travail s'il est encore actif.

    void enqueue(std::function<void()> task); // Méthode pour ajouter une tâche à la file.
                                              // Une tâche est une fonction encapsulée dans std::function<void()>.

    void start();                             // Méthode pour démarrer l'objet actif.
                                              // Lance le thread de travail et commence à traiter les tâches.

    void stop();                              // Méthode pour arrêter l'objet actif.
                                              // Attend que toutes les tâches en file soient terminées
                                              // avant de terminer le thread proprement.
};

#endif // ACTIVEOBJECT_HPP