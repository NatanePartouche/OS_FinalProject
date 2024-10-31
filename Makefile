# Nom du compilateur
CXX = g++
# Options de compilation
CXXFLAGS = -Wall -std=c++17 -pthread

# Répertoires d'objets
OBJ_DIR = obj
MODEL_DIR = $(OBJ_DIR)/Model
MODEL_TEST_DIR = $(OBJ_DIR)/Model_Test
NETWORK_DIR = $(OBJ_DIR)/Network

# Répertoires sources
SRC_DIR = src
MODEL_SRC = $(SRC_DIR)/Model
MODEL_TEST_SRC = $(SRC_DIR)/Model_Test
NETWORK_SRC = $(SRC_DIR)/Network

# Fichiers objets dans chaque répertoire
MODEL_OBJ = $(MODEL_DIR)/Graph.o $(MODEL_DIR)/MST.o $(MODEL_DIR)/MSTFactory.o
MODEL_TEST_OBJ = $(MODEL_TEST_DIR)/MST_Tests.o
NETWORK_OBJ = $(NETWORK_DIR)/ActiveObject.o $(NETWORK_DIR)/Server.o

# Fichier d'objet principal
MAIN_OBJ = $(OBJ_DIR)/main.o

# Tous les fichiers objets pour l'exécutable principal
OBJ_FILES = $(MODEL_OBJ) $(NETWORK_OBJ) $(MAIN_OBJ)

# Cibles de compilation
all: create_dirs server test

# Création des répertoires nécessaires
create_dirs:
	mkdir -p $(MODEL_DIR) $(MODEL_TEST_DIR) $(NETWORK_DIR)

# Cible de l'exécutable server
server: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o server $(OBJ_FILES)

# Cible de l'exécutable pour les tests
test: $(MODEL_TEST_OBJ) $(MODEL_OBJ)
	$(CXX) $(CXXFLAGS) -o test $(MODEL_TEST_OBJ) $(MODEL_OBJ)

# Règles de compilation pour les fichiers Model
$(MODEL_DIR)/Graph.o: $(MODEL_SRC)/Graph.cpp $(MODEL_SRC)/Graph.hpp
	$(CXX) $(CXXFLAGS) -c $(MODEL_SRC)/Graph.cpp -o $(MODEL_DIR)/Graph.o

$(MODEL_DIR)/MST.o: $(MODEL_SRC)/MST.cpp $(MODEL_SRC)/MST.hpp
	$(CXX) $(CXXFLAGS) -c $(MODEL_SRC)/MST.cpp -o $(MODEL_DIR)/MST.o

$(MODEL_DIR)/MSTFactory.o: $(MODEL_SRC)/MSTFactory.cpp $(MODEL_SRC)/MSTFactory.hpp
	$(CXX) $(CXXFLAGS) -c $(MODEL_SRC)/MSTFactory.cpp -o $(MODEL_DIR)/MSTFactory.o

# Règle de compilation pour les fichiers Model_Test
$(MODEL_TEST_DIR)/MST_Tests.o: $(MODEL_TEST_SRC)/MST_Tests.cpp $(MODEL_TEST_SRC)/doctest.h $(MODEL_SRC)/Graph.hpp $(MODEL_SRC)/MST.hpp
	$(CXX) $(CXXFLAGS) -c $(MODEL_TEST_SRC)/MST_Tests.cpp -o $(MODEL_TEST_DIR)/MST_Tests.o

# Règles de compilation pour les fichiers Network
$(NETWORK_DIR)/ActiveObject.o: $(NETWORK_SRC)/ActiveObject.cpp $(NETWORK_SRC)/ActiveObject.hpp
	$(CXX) $(CXXFLAGS) -c $(NETWORK_SRC)/ActiveObject.cpp -o $(NETWORK_DIR)/ActiveObject.o

$(NETWORK_DIR)/Server.o: $(NETWORK_SRC)/Server.cpp $(NETWORK_SRC)/Server.hpp $(NETWORK_SRC)/ActiveObject.hpp
	$(CXX) $(CXXFLAGS) -c $(NETWORK_SRC)/Server.cpp -o $(NETWORK_DIR)/Server.o

# Règle de compilation pour main.o
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $(OBJ_DIR)/main.o

# Commande pour nettoyer le projet
clean:
	rm -rf $(OBJ_DIR) server test

.PHONY: all clean create_dirs server test