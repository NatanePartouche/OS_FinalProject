# Compiler
CXX = g++
# Compilation flags
CXXFLAGS = -Wall -std=c++17 -pthread

# Object directories
OBJ_DIR = obj
MODEL_DIR = $(OBJ_DIR)/Model
MODEL_TEST_DIR = $(OBJ_DIR)/Model_Test
NETWORK_DIR = $(OBJ_DIR)/Network

# Source directories
SRC_DIR = src
MODEL_SRC = $(SRC_DIR)/Model
MODEL_TEST_SRC = $(SRC_DIR)/Model_Test
NETWORK_SRC = $(SRC_DIR)/Network

# Object files in each directory
MODEL_OBJ = $(MODEL_DIR)/Graph.o $(MODEL_DIR)/MSTFactory.o
MODEL_TEST_OBJ = $(MODEL_TEST_DIR)/MST_Tests.o
NETWORK_OBJ = $(NETWORK_DIR)/ActiveObject.o $(NETWORK_DIR)/Pipeline.o $(NETWORK_DIR)/Server.o

# Main object file
MAIN_OBJ = $(OBJ_DIR)/main.o

# All object files for the main executable
OBJ_FILES = $(MODEL_OBJ) $(NETWORK_OBJ) $(MAIN_OBJ)

# Build targets
all: create_dirs ./server ./tests

# Create necessary directories
create_dirs:
	mkdir -p $(MODEL_DIR) $(MODEL_TEST_DIR) $(NETWORK_DIR)

# Server executable target
./server: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o ./server $(OBJ_FILES)

# Test executable target
./tests: $(MODEL_TEST_OBJ) $(MODEL_OBJ)
	$(CXX) $(CXXFLAGS) -o ./tests $(MODEL_TEST_OBJ) $(MODEL_OBJ)

# Compilation rules for Model files
$(MODEL_DIR)/Graph.o: $(MODEL_SRC)/Graph.cpp $(MODEL_SRC)/Graph.hpp
	$(CXX) $(CXXFLAGS) -c $(MODEL_SRC)/Graph.cpp -o $(MODEL_DIR)/Graph.o

$(MODEL_DIR)/MSTFactory.o: $(MODEL_SRC)/MSTFactory.cpp $(MODEL_SRC)/MSTFactory.hpp
	$(CXX) $(CXXFLAGS) -c $(MODEL_SRC)/MSTFactory.cpp -o $(MODEL_DIR)/MSTFactory.o

# Compilation rule for Model_Test files
$(MODEL_TEST_DIR)/MST_Tests.o: $(MODEL_TEST_SRC)/MST_Tests.cpp $(MODEL_TEST_SRC)/doctest.h $(MODEL_SRC)/Graph.hpp
	$(CXX) $(CXXFLAGS) -c $(MODEL_TEST_SRC)/MST_Tests.cpp -o $(MODEL_TEST_DIR)/MST_Tests.o

# Compilation rules for Network files
$(NETWORK_DIR)/ActiveObject.o: $(NETWORK_SRC)/ActiveObject.cpp $(NETWORK_SRC)/ActiveObject.hpp
	$(CXX) $(CXXFLAGS) -c $(NETWORK_SRC)/ActiveObject.cpp -o $(NETWORK_DIR)/ActiveObject.o

$(NETWORK_DIR)/Pipeline.o: $(NETWORK_SRC)/Pipeline.cpp $(NETWORK_SRC)/Pipeline.hpp
	$(CXX) $(CXXFLAGS) -c $(NETWORK_SRC)/Pipeline.cpp -o $(NETWORK_DIR)/Pipeline.o

# Add the new rule for Server.o
$(NETWORK_DIR)/Server.o: $(NETWORK_SRC)/Server.cpp $(NETWORK_SRC)/Server.hpp
	$(CXX) $(CXXFLAGS) -c $(NETWORK_SRC)/Server.cpp -o $(NETWORK_DIR)/Server.o

# Compilation rule for main.o
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(NETWORK_SRC)/Pipeline.hpp $(NETWORK_SRC)/ActiveObject.hpp $(NETWORK_SRC)/Server.hpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $(OBJ_DIR)/main.o

# Clean the project
clean:
	rm -rf $(OBJ_DIR) ./server ./tests

.PHONY: all clean create_dirs ./server ./tests