# circle_rect_collision_detection
Simple implementation of circle-rectangle collision detection in C++ using SFML

Requirements: 1) C++11, 2) SFML 2.6.1

The program was tested on Ubuntu 22.04 LTS Jammy, gcc 11.4.0.

Instructions for an out-or-place build:
1) Go to the directory containing the CMakeLists.txt file.
2) Run `cmake -S . -B build`. This will create a `build` directory and put CMake files there.
3) Go to the `build` directory.
4) Build the project (e.g. for Unix makefiles it is achieved by running `make` command in the terminal).
5) Launch the executable file "circle_rect_collision".
6) Keyboard controls. Use direction (Up, Down, Left, Right) to move the circle.
7) Mouse controls. Left click on the circle and drag it across the screen.
8) When circle and rectangle collide circle's color is switched to red. 
