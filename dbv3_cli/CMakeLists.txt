cmake_minimum_required(VERSION 3.20)
project(dbv3_cli VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT OpenCV_DIR)
    set(OpenCV_DIR "C:/msys64/mingw64/lib/cmake/opencv4" CACHE PATH "OpenCV build dir" FORCE)
endif()

find_package(OpenCV REQUIRED)

set(SOURCES
    src/main.cpp
    src/ProjectParser.cpp
    src/Project.cpp
    src/Exporters.cpp
    src/DrawingPipeline.cpp
    src/PathOptimizer.cpp
    src/ImageFilter.cpp
    src/PFM_Sketch.cpp
    src/PFM_Streamline.cpp
    src/PFM_Spiral.cpp
    src/PFM_Hatch.cpp
    src/PFM_Adaptive.cpp
    src/PFM_LBG.cpp
    src/PFM_Voronoi.cpp
    src/PFM_Grid.cpp
    src/PFM_Composite.cpp
    src/PFM_Special.cpp
    src/GeometryUtils.cpp
)

add_executable(dbv3_cli ${SOURCES})

target_include_directories(dbv3_cli PRIVATE src include)
if(WIN32)
    target_link_libraries(dbv3_cli PRIVATE ${OpenCV_LIBS} ws2_32 crypt32)
else()
    target_link_libraries(dbv3_cli PRIVATE ${OpenCV_LIBS} pthread)
endif()
