cmake_minimum_required(VERSION 3.10)
project(Client_Chat)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Client_Chat 
        main.cpp
        Chat.cpp
)
