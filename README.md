
Explication des commandes :

	• 	 make    :  Cette commande compile tous les fichiers et crée les exécutables server et tests.
	•  ./server  :  Une fois compilé, cette commande exécute l’exécutable server, correspondant au main du projet.
	•  ./tests   :  Cette commande exécute l’exécutable tests, qui contient tous les tests de ton projet.



┌─────────────────/ OS_FinalProject /───────────────────┐
│                                                        │
├── cmake-build-debug/                                   │
│                                                        │
├── src/                                                 │
│   ├── Model/                                           │
│   │   ├── Graph.cpp                                    │
│   │   ├── Graph.hpp                                    │
│   │   ├── MST.cpp                                      │
│   │   ├── MST.hpp                                      │
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
│   │   ├── Server.cpp                                   │
│   │   ├── Server.hpp                                   │
│   │   ├── ActiveObject.cpp                             │
│   │   └── ActiveObject.hpp                             │
│   │                                                    │
│   └── main.cpp                                         │
│                                                        │
├── CMakeLists.txt                                       │
├── Makefile                                             │
├── README.md                                            │
└────────────────────────────────────────────────────────┘