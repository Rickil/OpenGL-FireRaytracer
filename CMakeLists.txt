cmake_minimum_required(VERSION 3.25)
project(POGL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLUT_LIBRARIES})

include_directories(.)

add_executable(POGL
        matrix4.hh Main.cpp Transformations.hh program.hh Camera.hh ObjLoader.hh Vector3.hh Vector2.hh Particle.hh Fire.hh TGA.hh KDTree.cpp KDTree.hh Scene.hh)

target_link_libraries(POGL ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLUT_LIBRARY})
