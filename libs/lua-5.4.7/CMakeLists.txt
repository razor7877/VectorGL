cmake_minimum_required(VERSION 3.24)
project(lua)

file(GLOB LUA_SRC_INCLUDES "src/*.c" "src/*.h")

add_library(lua ${LUA_SRC_INCLUDES})