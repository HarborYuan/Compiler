cmake_minimum_required(VERSION 3.10)
project(task03)

include_directories("${PROJECT_SOURCE_DIR}/include")
add_executable(reg2dfa ast.c dfa.c parser.c set.c main.c)
