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
        "C:/Qt/6.4.2/mingw_64"
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
find_package(Qt6SerialPort QUIET)

# ---- Find OpenCV ----
# Try common choco / vcpkg paths if OpenCV_DIR not set
if(NOT OpenCV_DIR)
    set(_OCV_SEARCH_PATHS
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

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc photo videoio)

# ---- Find Python3 ----
if(NOT Python3_ROOT_DIR)
    set(Python3_ROOT_DIR "C:/Python313" CACHE PATH "Python3 root directory")
endif()
find_package(Python3 COMPONENTS Development REQUIRED)

# ---- Source files ----
set(SOURCES
    
    
    
    batch/batch_processor.cpp
    core/color_separation.cpp
    core/coordinates.cpp
    core/mask_manager.cpp
    export/gcode_exporter.cpp
    export/hpgl_exporter.cpp
    export/path_optimizer.cpp
    export/pdf_exporter.cpp
    export/svg_exporter.cpp
    filters/artistic_distort_filters.cpp
    filters/base_filter.cpp
    filters/basic_filters.cpp
    filters/color_filters.cpp
    filters/edge_blur_filters.cpp
    filters/morph_filters.cpp
    filters/noise_filters.cpp
    filters/threshold_extra_filters.cpp
    main.cpp
    pfm/_letters_base_pfm.cpp
    pfm/adaptive_circular_scribbles_pfm.cpp
    pfm/adaptive_dashes_pfm.cpp
    pfm/adaptive_diagram_pfm.cpp
    pfm/adaptive_letters_pfm.cpp
    pfm/adaptive_pfm.cpp
    pfm/adaptive_shapes_pfm.cpp
    pfm/adaptive_tree_pfm.cpp
    pfm/adaptive_triangulation_pfm.cpp
    pfm/adaptive_tsppfm.cpp
    pfm/ambient_flow_pfm.cpp
    pfm/base_adaptive_pfm.cpp
    pfm/base_grid_pfm.cpp
    pfm/base_maze_pfm.cpp
    pfm/base_mosaic_pfm.cpp
    pfm/base_multi_hatch_pfm.cpp
    pfm/base_stipple_pfm.cpp
    pfm/base_streamline_pfm.cpp
    pfm/base_tsppfm.cpp
    pfm/base_voronoi_extra_pfm.cpp
    pfm/contour_paths_pfm.cpp
    pfm/edge_shading_pfm.cpp
    pfm/grid_dashes_pfm.cpp
    pfm/grid_letters_pfm.cpp
    pfm/grid_shapes_pfm.cpp
    pfm/hatch3_way_pfm.cpp
    pfm/hatch4_way_pfm.cpp
    pfm/hatch5_way_pfm.cpp
    pfm/hatch6_way_pfm.cpp
    pfm/hatch_circular_scribbles_pfm.cpp
    pfm/hatch_cross_pfm.cpp
    pfm/hatch_diagonal1_pfm.cpp
    pfm/hatch_diagonal2_pfm.cpp
    pfm/hatch_grid_pfm.cpp
    pfm/hatch_horizontal_pfm.cpp
    pfm/hatch_lines.cpp
    pfm/hatch_sawtooth_pfm.cpp
    pfm/hatch_vertical_pfm.cpp
    pfm/labyrinth_classic_pfm.cpp
    pfm/labyrinth_roman_pfm.cpp
    pfm/layers_pfm.cpp
    pfm/lbg_circular_scribbles_pfm.cpp
    pfm/lbg_dashes_pfm.cpp
    pfm/lbg_diagram_pfm.cpp
    pfm/lbg_letters_pfm.cpp
    pfm/lbg_pfms.cpp
    pfm/lbg_shapes_pfm.cpp
    pfm/lbg_stippling_pfm.cpp
    pfm/lbg_tree_pfm.cpp
    pfm/lbg_triangulation_pfm.cpp
    pfm/lbgtsppfm.cpp
    pfm/maze_circle_pfm.cpp
    pfm/maze_hex_pfm.cpp
    pfm/maze_hilbert_pfm.cpp
    pfm/maze_labyrinth_pfm.cpp
    pfm/maze_peano_pfm.cpp
    pfm/maze_rect_pfm.cpp
    pfm/maze_tri_pfm.cpp
    pfm/maze_voronoi_pfm.cpp
    pfm/mosaic_custom_pfm.cpp
    pfm/mosaic_rectangles_pfm.cpp
    pfm/mosaic_voronoi_pfm.cpp
    pfm/pfm_base.cpp
    pfm/sketch_abstract_pfm.cpp
    pfm/sketch_catmull_roms_pfm.cpp
    pfm/sketch_cubic_beziers2_pfm.cpp
    pfm/sketch_cubic_beziers_pfm.cpp
    pfm/sketch_curves.cpp
    pfm/sketch_curves_pfm.cpp
    pfm/sketch_delaunay_pfm.cpp
    pfm/sketch_flow_fields_pfm.cpp
    pfm/sketch_lines.cpp
    pfm/sketch_quad_beziers2_pfm.cpp
    pfm/sketch_quad_beziers_pfm.cpp
    pfm/sketch_radial_pfm.cpp
    pfm/sketch_scribble_pfm.cpp
    pfm/sketch_shapes.cpp
    pfm/sketch_sobel_edges_pfm.cpp
    pfm/sketch_spirals.cpp
    pfm/sketch_squares.cpp
    pfm/sketch_superformula_pfm.cpp
    pfm/sketch_sweeping_curves_pfm.cpp
    pfm/sketch_voronoi_pfm.cpp
    pfm/sketch_waves.cpp
    pfm/spiral_circular_scribbles_pfm.cpp
    pfm/spiral_sawtooth_pfm.cpp
    pfm/stipple_chaos_pfm.cpp
    pfm/stipple_circles_pfm.cpp
    pfm/stipple_crosses_pfm.cpp
    pfm/stipple_dots.cpp
    pfm/stipple_hexagons_pfm.cpp
    pfm/stipple_layers_pfm.cpp
    pfm/stipple_lines_pfm.cpp
    pfm/stipple_squares_pfm.cpp
    pfm/stipple_stars_pfm.cpp
    pfm/stipple_triangles_pfm.cpp
    pfm/stipple_variable_circles_pfm.cpp
    pfm/stipple_variable_squares_pfm.cpp
    pfm/streamlines_edge_field_pfm.cpp
    pfm/streamlines_flow_field_pfm.cpp
    pfm/streamlines_superformula_pfm.cpp
    pfm/tsp_classic_pfm.cpp
    pfm/tsp_outline_pfm.cpp
    pfm/tsp_shading_pfm.cpp
    pfm/tsp_stipple_pfm.cpp
    pfm/tsp_voronoi_pfm.cpp
    pfm/tspmstpfm.cpp
    pfm/voronoi_circles_pfm.cpp
    pfm/voronoi_dashes_pfm.cpp
    pfm/voronoi_diagram_pfm.cpp
    pfm/voronoi_letters_pfm.cpp
    pfm/voronoi_shapes_pfm.cpp
    pfm/voronoi_stippling_pfm.cpp
    pfm/voronoi_tree_pfm.cpp
    pfm/voronoi_triangulation_pfm.cpp
    pfm/voronoi_tsppfm.cpp
    ui/main_window.cpp
    ui/pfm_worker.cpp



)

set(HEADERS
    
    
    
    batch/batch_processor.h
    core/color_separation.h
    core/coordinates.h
    core/geometry.h
    core/mask_manager.h
    export/gcode_exporter.h
    export/hpgl_exporter.h
    export/path_optimizer.h
    export/pdf_exporter.h
    export/svg_exporter.h
    filters/artistic_distort_filters.h
    filters/base_filter.h
    filters/basic_filters.h
    filters/color_filters.h
    filters/edge_blur_filters.h
    filters/morph_filters.h
    filters/noise_filters.h
    filters/threshold_extra_filters.h
    pfm/_letters_base_pfm.h
    pfm/adaptive_circular_scribbles_pfm.h
    pfm/adaptive_dashes_pfm.h
    pfm/adaptive_diagram_pfm.h
    pfm/adaptive_letters_pfm.h
    pfm/adaptive_pfm.h
    pfm/adaptive_shapes_pfm.h
    pfm/adaptive_tree_pfm.h
    pfm/adaptive_triangulation_pfm.h
    pfm/adaptive_tsppfm.h
    pfm/ambient_flow_pfm.h
    pfm/base_adaptive_pfm.h
    pfm/base_grid_pfm.h
    pfm/base_maze_pfm.h
    pfm/base_mosaic_pfm.h
    pfm/base_multi_hatch_pfm.h
    pfm/base_stipple_pfm.h
    pfm/base_streamline_pfm.h
    pfm/base_tsppfm.h
    pfm/base_voronoi_extra_pfm.h
    pfm/contour_paths_pfm.h
    pfm/edge_shading_pfm.h
    pfm/grid_dashes_pfm.h
    pfm/grid_letters_pfm.h
    pfm/grid_shapes_pfm.h
    pfm/hatch3_way_pfm.h
    pfm/hatch4_way_pfm.h
    pfm/hatch5_way_pfm.h
    pfm/hatch6_way_pfm.h
    pfm/hatch_circular_scribbles_pfm.h
    pfm/hatch_cross_pfm.h
    pfm/hatch_diagonal1_pfm.h
    pfm/hatch_diagonal2_pfm.h
    pfm/hatch_grid_pfm.h
    pfm/hatch_horizontal_pfm.h
    pfm/hatch_lines.h
    pfm/hatch_sawtooth_pfm.h
    pfm/hatch_vertical_pfm.h
    pfm/labyrinth_classic_pfm.h
    pfm/labyrinth_roman_pfm.h
    pfm/layers_pfm.h
    pfm/lbg_circular_scribbles_pfm.h
    pfm/lbg_dashes_pfm.h
    pfm/lbg_diagram_pfm.h
    pfm/lbg_letters_pfm.h
    pfm/lbg_pfms.h
    pfm/lbg_shapes_pfm.h
    pfm/lbg_stippling_pfm.h
    pfm/lbg_tree_pfm.h
    pfm/lbg_triangulation_pfm.h
    pfm/lbgtsppfm.h
    pfm/maze_circle_pfm.h
    pfm/maze_hex_pfm.h
    pfm/maze_hilbert_pfm.h
    pfm/maze_labyrinth_pfm.h
    pfm/maze_peano_pfm.h
    pfm/maze_rect_pfm.h
    pfm/maze_tri_pfm.h
    pfm/maze_voronoi_pfm.h
    pfm/mosaic_custom_pfm.h
    pfm/mosaic_rectangles_pfm.h
    pfm/mosaic_voronoi_pfm.h
    pfm/pfm_base.h
    pfm/pfm_sketch_utils.h
    pfm/sketch_abstract_pfm.h
    pfm/sketch_catmull_roms_pfm.h
    pfm/sketch_cubic_beziers2_pfm.h
    pfm/sketch_cubic_beziers_pfm.h
    pfm/sketch_curves.h
    pfm/sketch_curves_pfm.h
    pfm/sketch_delaunay_pfm.h
    pfm/sketch_flow_fields_pfm.h
    pfm/sketch_lines.h
    pfm/sketch_quad_beziers2_pfm.h
    pfm/sketch_quad_beziers_pfm.h
    pfm/sketch_radial_pfm.h
    pfm/sketch_scribble_pfm.h
    pfm/sketch_shapes.h
    pfm/sketch_sobel_edges_pfm.h
    pfm/sketch_spirals.h
    pfm/sketch_squares.h
    pfm/sketch_superformula_pfm.h
    pfm/sketch_sweeping_curves_pfm.h
    pfm/sketch_voronoi_pfm.h
    pfm/sketch_waves.h
    pfm/spiral_circular_scribbles_pfm.h
    pfm/spiral_sawtooth_pfm.h
    pfm/stipple_chaos_pfm.h
    pfm/stipple_circles_pfm.h
    pfm/stipple_crosses_pfm.h
    pfm/stipple_dots.h
    pfm/stipple_hexagons_pfm.h
    pfm/stipple_layers_pfm.h
    pfm/stipple_lines_pfm.h
    pfm/stipple_squares_pfm.h
    pfm/stipple_stars_pfm.h
    pfm/stipple_triangles_pfm.h
    pfm/stipple_variable_circles_pfm.h
    pfm/stipple_variable_squares_pfm.h
    pfm/streamlines_edge_field_pfm.h
    pfm/streamlines_flow_field_pfm.h
    pfm/streamlines_superformula_pfm.h
    pfm/tsp_classic_pfm.h
    pfm/tsp_outline_pfm.h
    pfm/tsp_shading_pfm.h
    pfm/tsp_stipple_pfm.h
    pfm/tsp_voronoi_pfm.h
    pfm/tspmstpfm.h
    pfm/voronoi_circles_pfm.h
    pfm/voronoi_dashes_pfm.h
    pfm/voronoi_diagram_pfm.h
    pfm/voronoi_letters_pfm.h
    pfm/voronoi_shapes_pfm.h
    pfm/voronoi_stippling_pfm.h
    pfm/voronoi_tree_pfm.h
    pfm/voronoi_triangulation_pfm.h
    pfm/voronoi_tsppfm.h
    ui/main_window.h
    ui/pfm_worker.h



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
