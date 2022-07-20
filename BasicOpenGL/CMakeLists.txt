cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME basic_OpenGL)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})
include_directories("${CMAKE_SOURCE_DIR}/include")
add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    include/Shader.hpp
    include/Image.hpp
    include/Mesh.hpp
    include/Model.hpp
    )

include(Dependency.cmake)

target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

add_dependencies(${PROJECT_NAME} ${DEP_LIST})