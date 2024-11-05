Explication des commandes :

	• 	 make    :  Cette commande compile tous les fichiers et crée les exécutables server et tests.
	•  ./server  :  Une fois compilé, cette commande exécute l’exécutable server, correspondant au main du projet.
	•  ./tests   :  Cette commande exécute l’exécutable tests, qui contient tous les tests de ton projet.












Dans cette implémentation, le modèle Pipeline est composé de plusieurs Active Objects (AO) qui permettent le traitement séquentiel et parallèle des tâches, tout en optimisant les performances et l’utilisation de la mémoire cache.

Gestion du Pipeline
Le Pipeline est conçu pour enchaîner les opérations de traitement de manière fluide et sans interruption. 
Chaque AO représente une étape du pipeline, et les tâches sont soumises à la première étape via la méthode submitTask. 
Une fois que la tâche est traitée par le premier AO, elle est automatiquement transmise au suivant dans la chaîne, jusqu’à la fin du pipeline. Ainsi, le pipeline fonctionne comme une chaîne de production où chaque étape dépend de la précédente et alimente la suivante.

Fonctionnement de l’Active Object (AO)
Un Active Object est un objet indépendant qui fonctionne dans son propre thread et reste actif tout au long de l’exécution du programme. 
Chaque AO possède une file d’attente de tâches et un mécanisme de synchronisation qui lui permet de recevoir des tâches à exécuter sans être recréé. 
Lorsqu’une tâche est soumise à l’AO (via submit), celle-ci est ajoutée à la file d’attente et exécutée dès que possible par le thread associé.
L’AO fonctionne de manière asynchrone : il surveille en permanence sa file d’attente et exécute les tâches dès qu’elles arrivent. 
Ce modèle permet d’économiser les ressources, car les AO sont créés une seule fois et restent actifs en permanence. 
Lorsqu’une tâche est terminée, l’AO prend automatiquement la suivante, assurant ainsi un flux continu sans coût de création et destruction d’objets.

Avantages de cette architecture
Cette organisation en Pipeline composé d’Active Objects apporte plusieurs bénéfices :

	•	Traitement parallèle : chaque AO exécute des tâches indépendamment, ce qui permet de traiter plusieurs tâches en parallèle.
	•	Optimisation du cache : chaque AO garde les instructions en mémoire locale, réduisant les accès coûteux à la mémoire principale.
	•	Économie de ressources : les AO ne sont pas recréés après chaque tâche, limitant les coûts liés à l’allocation et libération de mémoire.

En résumé, cette gestion de Pipeline par Active Objects permet un traitement efficace et performant, adapté aux applications nécessitant des calculs ou traitements complexes répartis en plusieurs étapes.



┌─────────────────/ OS_FinalProject /────────────────────┐
│                                                        │
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
│   │                                                    │
│   └── main.cpp                                         │
│                                                        │
├── cmake-build-debug/                                   │
├── CMakeLists.txt                                       │
├── Makefile                                             │
├── README.md                                            │
└────────────────────────────────────────────────────────┘
