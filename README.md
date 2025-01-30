# quadratic_solver

## Description
quadratic_solver is a project for solving quadratic equations with multithreaded processing, implemented using the Producer-Consumer pattern.  

The structure is implemented as follows:

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


## Build Instructions
- CMake 3.10+
- g++12
- python3

## Build Instructions
```sh
mkdir -p build
cd build
cmake ..
make
```

## Running the Program
```sh
./build/quadratic_solver
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
│── build/               # Build directory
│── tests/               # Tests
│── CMakeLists.txt       # CMake configuration
│── README.md            # Documentation