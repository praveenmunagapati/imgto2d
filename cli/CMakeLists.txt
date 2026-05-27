cmake_minimum_required(VERSION 3.20)
project(imgto2d_standalone_cli VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Find OpenCV ----
# Set default OpenCV path if OpenCV_DIR is not set
if(NOT OpenCV_DIR)
    set(OpenCV_DIR "C:/msys64/mingw64/lib/cmake/opencv4" CACHE PATH "OpenCV build dir" FORCE)
endif()

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc photo)
find_package(OpenMP REQUIRED)

# ---- Common Includes ----
set(COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exporters
    ${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# ---- Executable ----
add_executable(imgto2d_standalone_cli
    src/algorithms/filters_raw.cpp
    src/algorithms/pfms.cpp
    src/app/main_cli.cpp
    src/core/coordinates.cpp
    src/exporters/export_svg.cpp
    src/exporters/gcode_exporter.cpp
    src/exporters/hpgl_exporter.cpp
    src/pipeline/color_separation.cpp
    src/pipeline/mask_manager.cpp
    src/pipeline/path_optimizer.cpp
    src/pipeline/project_processor.cpp
)

target_include_directories(imgto2d_standalone_cli PRIVATE
    ${COMMON_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(imgto2d_standalone_cli PRIVATE
    ${OpenCV_LIBS}
    OpenMP::OpenMP_CXX
)

install(TARGETS imgto2d_standalone_cli RUNTIME DESTINATION bin)


