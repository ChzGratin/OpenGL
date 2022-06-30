# target_link_directories: versionadded:: 3.13
cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME HelloOpenGL_CMake)
set(CMAKE_CXX_STANDARD 17)

# CMake variables for #define macro
set(WINDOW_NAME "HelloOpenGL (with CMake)")
set(WINDOW_WIDTH 800)
set(WINDOW_HEIGHT 600)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} src/main.cpp)

# add #define macro using CMake variables
# add_compile_definitions() and target_compile_definitions() should be called after add_executable()
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
)

include(Dependency.cmake)

# add include/lib option to our project
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

# build dependencies first
add_dependencies(${PROJECT_NAME} ${DEP_LIST})