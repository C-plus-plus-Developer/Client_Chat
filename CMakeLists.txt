cmake_minimum_required(VERSION 3.10)
project(Check_List)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Check_List 
        main.cpp
        Chat.cpp
)
