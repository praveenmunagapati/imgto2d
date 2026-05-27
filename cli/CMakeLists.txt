cmake_minimum_required(VERSION 3.20)
project(imgto2d_standalone_cli VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Find OpenCV ----
# Try common choco / vcpkg / mingw paths if OpenCV_DIR not set
if(NOT OpenCV_DIR)
    set(_OCV_SEARCH_PATHS
        "C:/msys64/mingw64/lib/cmake/opencv4"
        "C:/tools/OpenCV-MinGW-Build"
        "C:/tools/opencv/build"
        "C:/opencv/build"
        "C:/ProgramData/chocolatey/lib/opencv/tools/opencv/build"
    )
    foreach(_p ${_OCV_SEARCH_PATHS})
        if(EXISTS "${_p}/OpenCVConfig.cmake")
            set(OpenCV_DIR "${_p}" CACHE PATH "OpenCV build dir" FORCE)
            message(STATUS "Found OpenCV at: ${_p}")
            break()
        endif()
    endforeach()
endif()

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc photo)
find_package(OpenMP REQUIRED)

# ---- Executable ----
add_executable(imgto2d_standalone_cli
    src/app/main_cli.cpp
    src/algorithms/pfms.cpp
    src/algorithms/filters_raw.cpp
    src/pipeline/project_processor.cpp
    src/exporters/export_svg.cpp
    src/pipeline/mask_manager.cpp
    src/pipeline/color_separation.cpp
    src/core/coordinates.cpp
    src/pipeline/path_optimizer.cpp
    src/exporters/gcode_exporter.cpp
    src/exporters/hpgl_exporter.cpp
)

target_include_directories(imgto2d_standalone_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exporters
    ${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(imgto2d_standalone_cli PRIVATE
    ${OpenCV_LIBS}
    OpenMP::OpenMP_CXX
)

install(TARGETS imgto2d_standalone_cli RUNTIME DESTINATION bin)

# ---- Presets Generator ----
add_executable(dump_presets
    src/app/dump_presets.cpp
    src/algorithms/pfms.cpp
    src/pipeline/color_separation.cpp
)
target_include_directories(dump_presets PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline
    ${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(dump_presets PRIVATE
    ${OpenCV_LIBS}
    OpenMP::OpenMP_CXX
)



