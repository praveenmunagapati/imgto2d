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

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Network Concurrent)
find_package(Qt6 COMPONENTS SerialPort)

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

# ---- Find Python3 ----
if(NOT Python3_ROOT_DIR)
    set(Python3_ROOT_DIR "C:/Python313" CACHE PATH "Python3 root directory")
endif()
find_package(Python3 COMPONENTS Development REQUIRED)

# ---- Source files ----
set(SOURCES
    main.cpp
    core/color_separation.cpp
    core/coordinates.cpp
    core/mask_manager.cpp
    export/svg_exporter.cpp
    export/path_optimizer.cpp
    export/gcode_exporter.cpp
    export/hpgl_exporter.cpp
    export/pdf_exporter.cpp
    batch/batch_processor.cpp
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
    pfm/adaptive_extras.cpp
    pfm/lbg_pfms.cpp
    pfm/composite_pfms.cpp
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
    filters/base_filter.cpp
    filters/basic_filters.cpp
    filters/color_filters.cpp
    filters/edge_blur_filters.cpp
    filters/noise_filters.cpp
    filters/morph_filters.cpp
    filters/artistic_distort_filters.cpp
    filters/threshold_extra_filters.cpp
    ui/pfm_worker.cpp
    ui/main_window.cpp
)

set(HEADERS
    core/geometry.h
    core/color_separation.h
    core/coordinates.h
    core/mask_manager.h
    export/svg_exporter.h
    export/path_optimizer.h
    export/gcode_exporter.h
    export/hpgl_exporter.h
    export/pdf_exporter.h
    batch/batch_processor.h
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
    pfm/adaptive_extras.h
    pfm/lbg_pfms.h
    pfm/composite_pfms.h
    pfm/sketch_extras.h
    pfm/maze_tsp_pfms.h
    pfm/letters_pfms.h
    pfm/stipple_extras.h
    pfm/voronoi_pfms.h
    pfm/grid_pfms.h
    pfm/mosaic_pfms.h
    pfm/hatch_extras.h
    pfm/streamline_pfms.h
    pfm/special_pfms.h
    pfm/spiral_dbv3.h
    filters/base_filter.h
    filters/basic_filters.h
    filters/color_filters.h
    filters/edge_blur_filters.h
    filters/noise_filters.h
    filters/morph_filters.h
    filters/artistic_distort_filters.h
    filters/threshold_extra_filters.h
    ui/pfm_worker.h
    ui/main_window.h
)


# ---- Executable ----
add_executable(imgto2d_qt WIN32 ${SOURCES} ${HEADERS})

target_include_directories(imgto2d_qt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
)

target_link_libraries(imgto2d_qt PRIVATE
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Svg
    Qt6::Network
    ${OpenCV_LIBS}
    Python3::Python
)

if (Qt6SerialPort_FOUND)
    target_sources(imgto2d_qt PRIVATE
        core/serial_manager.cpp
        ui/serial_dialog.cpp
        core/serial_manager.h
        ui/serial_dialog.h
    )
    target_link_libraries(imgto2d_qt PRIVATE Qt6::SerialPort)
    target_compile_definitions(imgto2d_qt PRIVATE HAS_SERIAL_PORT)
endif()

# ---- Windows: deploy Qt and Python DLLs ----
if(WIN32)
    add_custom_command(TARGET imgto2d_qt POST_BUILD
        COMMAND "${Qt6_DIR}/../../../bin/windeployqt.exe"
                --no-translations
                "$<TARGET_FILE:imgto2d_qt>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/../python/runtime/python/python313.dll"
                "$<TARGET_FILE_DIR:imgto2d_qt>/python313.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/../python/runtime/python/python313._pth"
                "$<TARGET_FILE_DIR:imgto2d_qt>/python313._pth"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/../python/runtime/python/python313.zip"
                "$<TARGET_FILE_DIR:imgto2d_qt>/python313.zip"
        COMMENT "Running windeployqt and copying Python embeddable DLL, _pth and zip files..."
    )
endif()

install(TARGETS imgto2d_qt RUNTIME DESTINATION bin)

add_executable(test_pfms test_pfms.cpp)
target_sources(test_pfms PRIVATE
    filters/base_filter.cpp
    filters/color_filters.cpp
    filters/artistic_distort_filters.cpp
    pfm/pfm_base.cpp
)
target_include_directories(test_pfms PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(test_pfms PRIVATE Qt6::Core Qt6::Widgets ${OpenCV_LIBS})

add_executable(test_vpype_embed test_vpype_embed.cpp)
target_include_directories(test_vpype_embed PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Python3_INCLUDE_DIRS}
)
target_link_libraries(test_vpype_embed PRIVATE Qt6::Core Python3::Python)

if(WIN32)
    add_custom_command(TARGET test_vpype_embed POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/../python/runtime/python/python313.dll"
                "$<TARGET_FILE_DIR:test_vpype_embed>/python313.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/../python/runtime/python/python313._pth"
                "$<TARGET_FILE_DIR:test_vpype_embed>/python313._pth"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/../python/runtime/python/python313.zip"
                "$<TARGET_FILE_DIR:test_vpype_embed>/python313.zip"
        COMMENT "Copying Python embeddable DLL, _pth and zip files to test_vpype_embed directory..."
    )
endif()
