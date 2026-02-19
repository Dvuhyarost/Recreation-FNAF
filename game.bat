@echo off
echo Build started...

if not exist "build" mkdir build

cmake -S . -B build

cmake --build build

build\ConsoleGame  

pause

g++ module1.cpp module2.cpp module3.cpp main.cpp -o program