# quadratic_solver

## Description
quadratic_solver is a project for solving quadratic equations with multithreaded processing, implemented using the Producer-Consumer pattern.  

The structure is implemented as follows:

```
                  +----------------------+  
                  |  Single Input Prod.  |  
                  +----------------------+  
                /            |            \  
               /             |             \  
              /              |              \-----  
             /               |                    \  
            v                v                     v  
+----------------+   +----------------+        +----------------+  
|  Solver #1     |   |  Solver #2     |   ...  |  Solver #N     |  
+----------------+   +----------------+        +----------------+  
           \                 |                    /  
            \                |                   /  
             \               |                  /  
              \              |                 /  
               v             v                v  
               +-----------------------------+  
               |      Single Output Print    |  
               +-----------------------------+  
```

## Build Requirements
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
make run_tests
```
or
```sh
cd build
python3 tests.py
```

## Output format

```
> ./quadratic_solver 1 -2 -3
(1,-2,-3) => (3.000000, -1.000000) Xmin=-1.000000

> ./quadratic_solver 0 0 0
(0,0,0) => All roots No Xmin

> ./quadratic_solver 0 0 1
(0,0,1) => No roots No Xmin

> ./quadratic_solver 0 1 1
(0,1,1) => (-1.000000) No Xmin

> ./quadratic_solver qwerty 1 1
(,1,1) => invalid input
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