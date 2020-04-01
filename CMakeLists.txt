cmake_minimum_required(VERSION 3.15)
project(segway)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE debug)
ENDIF()

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES src/main.cpp src/draw.cpp src/scene.cpp)
SET(SOURCE_HEADERS lib/draw.h lib/scene.h lib/global.h lib/pendulum.h lib/pid.h lib/spring.h)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(3rd_party/glfw)
include_directories(3rd_party/glfw/include)
LIST(APPEND LIBS glfw ${GLFW_LIBRARIES})

if(NOT WIN32)
    LIST(APPEDN LIBS dl)
endif()

include_directories(include)
link_directories(extern)

add_executable(segway ${SOURCE_FILES} ${SOURCE_HEADERS})

target_link_libraries(segway libglad.a libbox2d.a ${LIBS})