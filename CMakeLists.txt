cmake_minimum_required(VERSION 3.25)
project(PasswordManager)

set(CMAKE_CXX_STANDARD 23)

add_executable(PasswordManager main.cpp password.cpp encryptor.cpp menu.cpp)
