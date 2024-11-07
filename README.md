Explication des commandes :

	• 	 make    :  Cette commande compile tous les fichiers et crée les exécutables server et tests.
	•  ./server  :  Une fois compilé, cette commande exécute l’exécutable server, correspondant au main du projet.
	•  ./tests   :  Cette commande exécute l’exécutable tests, qui contient tous les tests de ton projet.



My program operates with client connections to a server I have created, 
leveraging socket-based communication to enable interaction between the client and server.
How does a socket work?

	1.	Socket Creation: A program creates a socket, which acts as an endpoint for listening for incoming connections or initiating an outgoing one.
	2.	Connection: A client program uses its socket to connect to a server, which is actively listening on a specific port on a machine. 
        This connection forms a communication pathway between the client and server.
	3.	Data Exchange: Once the connection is established, the programs can exchange data back and forth through their sockets. 
        The data flows between them, enabling real-time communication or data transfer.
	4.	Closure: After the data exchange is complete, each side closes its socket, which frees up resources and ends the communication.

Sockets are commonly used in client-server applications. In such setups, one program (the server) listens for connections on a network port, 
while another program (the client) connects to that server to send or receive data.









1. Threads Pool :
   •	Il y a 4 threads dans le pool de threads qui surveillent la file clientQueue pour traiter les connexions clients.
   •	Ces threads ne traitent pas directement les requêtes des clients.
   Leurs rôles est principalement de :
	- Surveiller la file d’attente pour savoir si de nouveaux clients ont été acceptés par le serveur.
    - Extraire le socket client de la file d’attente.
    - Déléguer le traitement de la requête client à l’ActiveObject.

2. ActiveObject :
   •	Il y a un seul ActiveObject global qui est utilisé pour gérer et exécuter les tâches de manière asynchrone.
   •	L’ActiveObject prend en charge le traitement des requêtes des clients. 
        Chaque tâche qui est envoyée à l’ActiveObject représente une opération de traitement pour un client connecté (par exemple, la fonction handleClient(clientSocket)).
   •	Les threads du pool ajoutent des tâches à l’ActiveObject via activeObject.enqueueTask(...), puis se libèrent pour surveiller à nouveau la file clientQueue.

Fonctionnement général :
	•	Acceptation des connexions : Le thread principal du serveur utilise la fonction accept() pour recevoir les connexions entrantes et les ajoute à la file clientQueue.
	•	Distribution des connexions : Les threads du pool surveillent clientQueue, prennent les connexions et les passent à l’ActiveObject pour traitement.
	•	Traitement des requêtes : L’ActiveObject traite chaque requête de manière asynchrone, libérant ainsi les threads du pool pour gérer d’autres connexions.
Schéma de fonctionnement :
	1.	Le thread principal accepte les nouvelles connexions et les ajoute à clientQueue.
	2.	Les threads du pool prennent les connexions de clientQueue et les ajoutent à l’ActiveObject.
	3.	L’ActiveObject exécute les tâches de traitement des clients.









┌─────────────────/ OS_FinalProject /────────────────────┐
├── src/                                                 │
│   ├── Model/                                           │
│   │   ├── Graph.cpp                                    │
│   │   ├── Graph.hpp                                    │
│   │   ├── MSTFactory.cpp                               │
│   │   └── MSTFactory.hpp                               │
│   │                                                    │
│   ├── Model_Tests/                                     │
│   │   ├── doctest.h                                    │
│   │   ├── Graph with 3 vertices.png                    │
│   │   ├── Graph with 5 vertices.png                    │
│   │   ├── Graph with 6 vertices.png                    │
│   │   └── MST_Tests.cpp                                │
│   │                                                    │
│   ├── Network/                                         │
│   │   ├── ActiveObject.cpp                             │
│   │   ├── ActiveObject.hpp                             │
│   │   ├── Pipeline.cpp                                 │
│   │   ├── Pipeline.hpp                                 │
│   │   ├── Server.cpp                                   │
│   │   └── Server.hpp                                   │
│   └─ main.cpp                                          │
│                                                        │
├── cmake-build-debug/                                   │
├── CMakeLists.txt                                       │
├── Makefile                                             │
├── README.md                                            │
└────────────────────────────────────────────────────────┘
