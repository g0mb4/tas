cmake_minimum_required(VERSION 3.8)
project(tas)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

file(GLOB_RECURSE SRC
  src/*.c
  src/*.h
)

include_directories(src)

IF(WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(strdup=_strdup)
ENDIF()

add_executable(${PROJECT_NAME} ${SRC})