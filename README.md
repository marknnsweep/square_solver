# quadratic_solver

## Description
quadratic_solver is a project for solving quadratic equations with multithreaded processing, implemented using the Producer-Consumer pattern.  

The structure is implemented as follows:

```
                  +----------------------+  
                  |  Single Input Prod. |  
                  +----------------------+  
                    /        |           \  
                   /         |            \  
                  /          |             \-----  
                 /           |                   \  
                v            v                    v  
   +----------------+     +----------------+     +----------------+  
   |  Solver #1     |     |  Solver #2     |     |  Solver #N     |  
   +----------------+     +----------------+     +----------------+  
           \                 |                    /  
            \                |                   /  
             \               |                  /  
              \              |                 /  
               v             v                v  
               +-----------------------------+  
               |      Single Output Print   |  
               +-----------------------------+  
```

## Build Instructions
- CMake 3.10+
- g++12+
- python3.7+

## Build Instructions
```sh
mkdir -p build
cd build
cmake ..
make
```

## Running the Program
```sh
./build/quadratic_solver N1 N2 N3 ... Nm
```

## Running Tests
```sh
cd build
python3 tests.py
```

## Project Structure
```
quadratic_solver/
│── src/                 # Source files
│   │── Application.cpp
│   │── Application.hpp
│   │── Printer.hpp
│   │── Producer.hpp
│   │── QuadraticEquationSolver.cpp
│   │── QuadraticEquationSolver.hpp
│   │── Solver.hpp
│   │── SortOfLockFreeQueue.hpp
│   │── TupleProducer.hpp
│   │── main.cpp
│── build/               # Build directory
│── tests/               # Tests
│   │── tests.py
│── CMakeLists.txt       # CMake configuration
│── README.md            # Documentation