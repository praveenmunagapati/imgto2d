cmake_minimum_required(VERSION 3.20)
project(ArchitectureTest VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

set(SOURCES
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

add_library(DrawingBotCore STATIC ${SOURCES})
target_include_directories(DrawingBotCore PUBLIC ${OpenCV_INCLUDE_DIRS} include)
target_link_libraries(DrawingBotCore PUBLIC ${OpenCV_LIBS})
