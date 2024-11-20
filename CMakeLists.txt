cmake_minimum_required(VERSION 3.0)
project(final)

cmake_policy(SET CMP0072 NEW)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(assimp REQUIRED)

set(TARGET_NAME "${PROJECT_NAME}")
set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(INCLUDE_DIR "${PROJECT_ROOT}/include")
set(SOURCES_DIR "${PROJECT_ROOT}/src")
set(SHADERS_DIR "${SOURCES_DIR}/shaders")

add_definitions(-DSHADERS_DIR="${SHADERS_DIR}")

file(GLOB sources CONFIGURE_DEPENDS "${SOURCES_DIR}/*.c" "${SOURCES_DIR}/*.cc" "${SOURCES_DIR}/*.cpp")
file(GLOB headers CONFIGURE_DEPENDS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp")
list(APPEND headers ${SHADERS_H})

add_executable(${TARGET_NAME} ${sources} ${headers})
target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIR})

target_include_directories(${TARGET_NAME} PUBLIC
	"${SDL2_INCLUDE_DIRS}"
	"${GLEW_INCLUDE_DIRS}"
	"${OPENGL_INCLUDE_DIRS}"
	"${ASSIMP_INCLUDE_DIRS}"
	)
target_link_libraries(${TARGET_NAME} PUBLIC
	"${GLEW_LIBRARIES}"
	"${SDL2_LIBRARIES}"
	"${OPENGL_LIBRARIES}"
	"${ASSIMP_LIBRARIES}"
)
target_compile_definitions(${TARGET_NAME} PUBLIC -DPROJECT_ROOT="${PROJECT_ROOT}")
