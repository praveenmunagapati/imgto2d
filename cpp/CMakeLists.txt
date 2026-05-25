cmake_minimum_required(VERSION 3.20)
project(imgto2d_qt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt auto-tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---- Find Qt6 ----
# Try common install locations if CMAKE_PREFIX_PATH not set
if(NOT CMAKE_PREFIX_PATH)
    set(_QT_SEARCH_PATHS
        "C:/Qt/6.9.0/msvc2022_64"
        "C:/Qt/6.8.0/msvc2022_64"
        "C:/Qt/6.7.0/msvc2022_64"
        "C:/Qt/6.4.2/msvc2022_64"
        "C:/tools/Qt/6.9.0/msvc2022_64"
    )
    foreach(_p ${_QT_SEARCH_PATHS})
        if(EXISTS "${_p}/lib/cmake/Qt6")
            set(CMAKE_PREFIX_PATH "${_p}" CACHE PATH "Qt6 install prefix" FORCE)
            message(STATUS "Found Qt6 at: ${_p}")
            break()
        endif()
    endforeach()
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

# ---- Find OpenCV ----
# Try common choco / vcpkg paths if OpenCV_DIR not set
if(NOT OpenCV_DIR)
    set(_OCV_SEARCH_PATHS
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

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc)

# ---- Source files ----
set(SOURCES
    main.cpp
    pfm/pfm_base.cpp
    pfm/sketch_lines.cpp
    pfm/sketch_squares.cpp
    pfm/sketch_curves.cpp
    pfm/sketch_shapes.cpp
    pfm/sketch_waves.cpp
    pfm/sketch_spirals.cpp
    pfm/hatch_lines.cpp
    pfm/stipple_dots.cpp
    pfm/adaptive_pfm.cpp
    pfm/sketch_extras.cpp
    pfm/maze_tsp_pfms.cpp
    pfm/letters_pfms.cpp
    pfm/stipple_extras.cpp
    pfm/voronoi_pfms.cpp
    pfm/grid_pfms.cpp
    pfm/mosaic_pfms.cpp
    pfm/hatch_extras.cpp
    pfm/streamline_pfms.cpp
    pfm/special_pfms.cpp
    pfm/spiral_dbv3.cpp
    ui/pfm_worker.cpp
    ui/main_window.cpp
)

set(HEADERS
    core/geometry.h
    pfm/pfm_base.h
    pfm/pfm_sketch_utils.h
    pfm/sketch_lines.h
    pfm/sketch_squares.h
    pfm/sketch_curves.h
    pfm/sketch_beziers.h
    pfm/sketch_shapes.h
    pfm/sketch_waves.h
    pfm/sketch_spirals.h
    pfm/hatch_lines.h
    pfm/stipple_dots.h
    pfm/adaptive_pfm.h
    pfm/sketch_extras.h
    pfm/maze_tsp_pfms.h
    pfm/letters_pfms.h
    pfm/stipple_extras.h
    pfm/voronoi_pfms.h
    pfm/grid_pfms.h
    pfm/mosaic_pfms.h
    ui/pfm_worker.h
    ui/main_window.h
)

# ---- Executable ----
add_executable(imgto2d_qt WIN32 ${SOURCES} ${HEADERS})

target_include_directories(imgto2d_qt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(imgto2d_qt PRIVATE
    Qt6::Widgets
    Qt6::Concurrent
    ${OpenCV_LIBS}
)

# ---- Windows: deploy Qt DLLs ----
if(WIN32)
    add_custom_command(TARGET imgto2d_qt POST_BUILD
        COMMAND "${Qt6_DIR}/../../../bin/windeployqt.exe"
                --no-translations
                "$<TARGET_FILE:imgto2d_qt>"
        COMMENT "Running windeployqt..."
    )
endif()

install(TARGETS imgto2d_qt RUNTIME DESTINATION bin)

add_executable(test_pfms test_pfms.cpp)
target_link_libraries(test_pfms PRIVATE Qt6::Core Qt6::Widgets )
