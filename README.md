
# OS_FinalProject: Graph Manipulation and Server Execution

## Graph Manipulation

The server provides a command-based interface to dynamically manipulate graphs. Below are the key commands:

### Commands

1. **Create a Graph**
    - **Syntax:** `create <number_of_vertices>`
    - Initializes a graph with the specified number of vertices.
    - **Example:** `create 5`

2. **Add an Edge**
    - **Syntax:** `add <u> <v> <w>`
    - Adds an edge between vertices `u` and `v` with weight `w`.
    - **Example:** `add 1 2 10`

3. **Remove an Edge**
    - **Syntax:** `remove <u> <v>`
    - Removes the edge between vertices `u` and `v`.
    - **Example:** `remove 1 2`

4. **Select MST Algorithm**
    - **Syntax:** `algo <algorithm_name>`
    - Sets the Minimum Spanning Tree algorithm.  
      **Options:** `prim`, `kruskal`, `tarjan`, `boruvka`, `integer_mst`
    - **Example:** `algo prim`

5. **Analyze MST**
    - Once the graph is manipulated, the server calculates:
        - Total MST weight
        - Average distance
        - Longest and heaviest paths
        - Heaviest and lightest edges

6. **Shutdown**
    - **Syntax:** `shutdown`
    - Disconnects the client.

---

## Server Execution

### Compilation

To build the project:

```bash
make
```

This generates:
- `./server`: The main server executable.
- `./tests`: Test suite executable.

---

### Running the Server

#### Pipeline Mode (`-PL`)

```bash
./server -PL [<port>]
```

- **Description:** Starts the server in Pipeline mode.
- **Default Port:** `8080`.

**Example:**

```bash
./server -PL 9090
```

#### Leader-Followers Mode (`-LF`)

```bash
./server -LF [<num_threads>] [<port>]
```

- **Description:** Starts the server in Leader-Followers mode.
- **Defaults:**
    - `num_threads = 4`
    - `port = 8080`

**Example:**

```bash
./server -LF 8 9090
```

---

### Stopping the Server

Press **Enter** in the terminal running the server to stop it gracefully.

---

### Clean Up

To remove build artifacts and executables:

```bash
make clean
```

---






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
│   │   ├── LeaderFollowers.cpp                          │
│   │   ├── LeaderFollowers.hpp                          │
│   │   ├── Server.hpp                                   │
│   │   ├── Server_LF.hpp                                │
│   │   └── Server_PL.hpp                                │
│   └─ main.cpp                                          │
│                                                        │
├── cmake-build-debug/                                   │
├── CMakeLists.txt                                       │
├── Makefile                                             │
├── README.md                                            │
└────────────────────────────────────────────────────────┘





make memcheck_server ARGS="-PL 4 8080"
make memcheck_server ARGS="-LF 4 8080"