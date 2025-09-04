cmake_minimum_required(VERSION 3.16)
project(MyOpenGLApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

find_package(wxWidgets REQUIRED COMPONENTS core base gl)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

if(NOT wxWidgets_FOUND)
    message(FATAL_ERROR "wxWidgets not found! Please install wxWidgets development packages.")
endif()

if(NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found! Please install GLEW development packages.")
endif()

set(SOURCES
    src/main.cpp
    src/MainFrame.cpp
    src/GLCanvas.cpp
    src/Renderer.cpp
)

set(HEADERS
    src/MainFrame.h
    src/GLCanvas.h
    src/Renderer.h
)

add_executable(MyOpenGLApp ${SOURCES} ${HEADERS})

target_link_libraries(MyOpenGLApp PRIVATE
    ${wxWidgets_LIBRARIES}
    OpenGL::GL
    GLEW::GLEW
)

target_include_directories(MyOpenGLApp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${wxWidgets_INCLUDE_DIRS}
)

target_compile_definitions(MyOpenGLApp PRIVATE
    ${wxWidgets_DEFINITIONS}
)

target_compile_options(MyOpenGLApp PRIVATE
    ${wxWidgets_CXX_FLAGS}
)

add_custom_command(TARGET MyOpenGLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/icon
    $<TARGET_FILE_DIR:MyOpenGLApp>/icon
    COMMENT "Copying icon resources to build directory"
)

install(TARGETS MyOpenGLApp
    RUNTIME DESTINATION bin
)

install(DIRECTORY icon/
    DESTINATION bin/icon
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "wxWidgets version: ${wxWidgets_VERSION}")

