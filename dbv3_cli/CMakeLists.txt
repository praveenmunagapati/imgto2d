cmake_minimum_required(VERSION 3.20)
project(dbv3_cli VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT OpenCV_DIR)
    set(OpenCV_DIR "C:/msys64/mingw64/lib/cmake/opencv4" CACHE PATH "OpenCV build dir" FORCE)
endif()

find_package(OpenCV REQUIRED)

# Include the core backend engine
add_subdirectory(../architecture architecture_build)

set(SOURCES
    src/main.cpp
    src/ProjectParser.cpp
)

add_executable(dbv3_cli ${SOURCES})

target_include_directories(dbv3_cli PRIVATE src ../architecture/include)
target_link_libraries(dbv3_cli PRIVATE DrawingBotCore ${OpenCV_LIBS})
