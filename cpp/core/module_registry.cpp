#include "core/module_registry.h"
#include "pfm/_letters_base_pfm.h"
#include "pfm/adaptive_circular_scribbles_pfm.h"
#include "pfm/adaptive_dashes_pfm.h"
#include "pfm/adaptive_diagram_pfm.h"
#include "pfm/adaptive_letters_pfm.h"
#include "pfm/adaptive_pfm.h"
#include "pfm/adaptive_shapes_pfm.h"
#include "pfm/adaptive_tree_pfm.h"
#include "pfm/adaptive_triangulation_pfm.h"
#include "pfm/adaptive_tsppfm.h"
#include "pfm/ambient_flow_pfm.h"
#include "pfm/base_adaptive_pfm.h"
#include "pfm/base_grid_pfm.h"
#include "pfm/base_maze_pfm.h"
#include "pfm/base_mosaic_pfm.h"
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/base_stipple_pfm.h"
#include "pfm/base_streamline_pfm.h"
#include "pfm/base_tsppfm.h"
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/contour_paths_pfm.h"
#include "pfm/edge_shading_pfm.h"
#include "pfm/grid_dashes_pfm.h"
#include "pfm/grid_letters_pfm.h"
#include "pfm/grid_shapes_pfm.h"
#include "pfm/hatch3_way_pfm.h"
#include "pfm/hatch4_way_pfm.h"
#include "pfm/hatch5_way_pfm.h"
#include "pfm/hatch6_way_pfm.h"
#include "pfm/hatch_circular_scribbles_pfm.h"
#include "pfm/hatch_cross_pfm.h"
#include "pfm/hatch_diagonal1_pfm.h"
#include "pfm/hatch_diagonal2_pfm.h"
#include "pfm/hatch_grid_pfm.h"
#include "pfm/hatch_horizontal_pfm.h"
#include "pfm/hatch_lines.h"
#include "pfm/hatch_sawtooth_pfm.h"
#include "pfm/hatch_vertical_pfm.h"
#include "pfm/labyrinth_classic_pfm.h"
#include "pfm/labyrinth_roman_pfm.h"
#include "pfm/layers_pfm.h"
#include "pfm/lbg_circular_scribbles_pfm.h"
#include "pfm/lbg_dashes_pfm.h"
#include "pfm/lbg_diagram_pfm.h"
#include "pfm/lbg_letters_pfm.h"
#include "pfm/lbg_pfms.h"
#include "pfm/lbg_shapes_pfm.h"
#include "pfm/lbg_stippling_pfm.h"
#include "pfm/lbg_tree_pfm.h"
#include "pfm/lbg_triangulation_pfm.h"
#include "pfm/lbgtsppfm.h"
#include "pfm/maze_circle_pfm.h"
#include "pfm/maze_hex_pfm.h"
#include "pfm/maze_hilbert_pfm.h"
#include "pfm/maze_labyrinth_pfm.h"
#include "pfm/maze_peano_pfm.h"
#include "pfm/maze_rect_pfm.h"
#include "pfm/maze_tri_pfm.h"
#include "pfm/maze_voronoi_pfm.h"
#include "pfm/mosaic_custom_pfm.h"
#include "pfm/mosaic_rectangles_pfm.h"
#include "pfm/mosaic_voronoi_pfm.h"
#include "pfm/sketch_abstract_pfm.h"
#include "pfm/sketch_catmull_roms_pfm.h"
#include "pfm/sketch_cubic_beziers2_pfm.h"
#include "pfm/sketch_cubic_beziers_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_curves_pfm.h"
#include "pfm/sketch_delaunay_pfm.h"
#include "pfm/sketch_flow_fields_pfm.h"
#include "pfm/sketch_lines.h"
#include "pfm/sketch_quad_beziers2_pfm.h"
#include "pfm/sketch_quad_beziers_pfm.h"
#include "pfm/sketch_radial_pfm.h"
#include "pfm/sketch_scribble_pfm.h"
#include "pfm/sketch_shapes.h"
#include "pfm/sketch_sobel_edges_pfm.h"
#include "pfm/sketch_spirals.h"
#include "pfm/sketch_squares.h"
#include "pfm/sketch_superformula_pfm.h"
#include "pfm/sketch_sweeping_curves_pfm.h"
#include "pfm/sketch_voronoi_pfm.h"
#include "pfm/sketch_waves.h"
#include "pfm/spiral_circular_scribbles_pfm.h"
#include "pfm/spiral_sawtooth_pfm.h"
#include "pfm/stipple_chaos_pfm.h"
#include "pfm/stipple_circles_pfm.h"
#include "pfm/stipple_crosses_pfm.h"
#include "pfm/stipple_dots.h"
#include "pfm/stipple_hexagons_pfm.h"
#include "pfm/stipple_layers_pfm.h"
#include "pfm/stipple_lines_pfm.h"
#include "pfm/stipple_squares_pfm.h"
#include "pfm/stipple_stars_pfm.h"
#include "pfm/stipple_triangles_pfm.h"
#include "pfm/stipple_variable_circles_pfm.h"
#include "pfm/stipple_variable_squares_pfm.h"
#include "pfm/streamlines_edge_field_pfm.h"
#include "pfm/streamlines_flow_field_pfm.h"
#include "pfm/streamlines_superformula_pfm.h"
#include "pfm/tsp_classic_pfm.h"
#include "pfm/tsp_outline_pfm.h"
#include "pfm/tsp_shading_pfm.h"
#include "pfm/tsp_stipple_pfm.h"
#include "pfm/tsp_voronoi_pfm.h"
#include "pfm/tspmstpfm.h"
#include "pfm/voronoi_circles_pfm.h"
#include "pfm/voronoi_dashes_pfm.h"
#include "pfm/voronoi_diagram_pfm.h"
#include "pfm/voronoi_letters_pfm.h"
#include "pfm/voronoi_shapes_pfm.h"
#include "pfm/voronoi_stippling_pfm.h"
#include "pfm/voronoi_tree_pfm.h"
#include "pfm/voronoi_triangulation_pfm.h"
#include "pfm/voronoi_tsppfm.h"
#include "filters/basic_filters.h"
#include "filters/color_filters.h"
#include "filters/edge_blur_filters.h"
#include "filters/noise_filters.h"
#include "filters/morph_filters.h"
#include "filters/artistic_distort_filters.h"
#include "filters/threshold_extra_filters.h"

QVector<std::shared_ptr<PathFindingModule>> ModuleRegistry::createAllPFMs() {
    QVector<std::shared_ptr<PathFindingModule>> pfms;
    pfms.append(std::make_shared<SketchLinesPFM>());
    pfms.append(std::make_shared<SketchSquaresPFM>());
    pfms.append(std::make_shared<SketchCurvesPFM>());
    pfms.append(std::make_shared<SketchShapesPFM>());
    pfms.append(std::make_shared<SketchQuadBeziersPFM>());
    pfms.append(std::make_shared<SketchCubicBeziersPFM>());
    pfms.append(std::make_shared<SketchWavesPFM>());
    pfms.append(std::make_shared<SketchSpiralsPFM>());
    pfms.append(std::make_shared<SketchSuperformulaPFM>());
    pfms.append(std::make_shared<SketchCubicBeziers2PFM>());
    pfms.append(std::make_shared<SketchQuadBeziers2PFM>());
    pfms.append(std::make_shared<SketchSweepingCurvesPFM>());
    pfms.append(std::make_shared<SketchFlowFieldsPFM>());
    pfms.append(std::make_shared<SketchVoronoiPFM>());
    pfms.append(std::make_shared<SketchDelaunayPFM>());
    pfms.append(std::make_shared<SketchRadialPFM>());
    pfms.append(std::make_shared<SketchScribblePFM>());
    pfms.append(std::make_shared<SketchAbstractPFM>());
    pfms.append(std::make_shared<SketchCatmullRomsPFM>());
    pfms.append(std::make_shared<SketchSobelEdgesPFM>());
    pfms.append(std::make_shared<HatchLinesPFM>());
    pfms.append(std::make_shared<HatchHorizontalPFM>());
    pfms.append(std::make_shared<HatchVerticalPFM>());
    pfms.append(std::make_shared<HatchDiagonal1PFM>());
    pfms.append(std::make_shared<HatchDiagonal2PFM>());
    pfms.append(std::make_shared<HatchGridPFM>());
    pfms.append(std::make_shared<HatchCrossPFM>());
    pfms.append(std::make_shared<Hatch3WayPFM>());
    pfms.append(std::make_shared<Hatch4WayPFM>());
    pfms.append(std::make_shared<Hatch5WayPFM>());
    pfms.append(std::make_shared<Hatch6WayPFM>());
    pfms.append(std::make_shared<HatchSawtoothPFM>());
    pfms.append(std::make_shared<HatchCircularScribblesPFM>());
    pfms.append(std::make_shared<StippleDotsPFM>());
    pfms.append(std::make_shared<StippleCirclesPFM>());
    pfms.append(std::make_shared<StippleSquaresPFM>());
    pfms.append(std::make_shared<StippleTrianglesPFM>());
    pfms.append(std::make_shared<StippleHexagonsPFM>());
    pfms.append(std::make_shared<StippleStarsPFM>());
    pfms.append(std::make_shared<StippleCrossesPFM>());
    pfms.append(std::make_shared<StippleLinesPFM>());
    pfms.append(std::make_shared<StippleVariableCirclesPFM>());
    pfms.append(std::make_shared<StippleVariableSquaresPFM>());
    pfms.append(std::make_shared<StippleChaosPFM>());
    pfms.append(std::make_shared<AdaptiveStipplingPFM>());
    pfms.append(std::make_shared<AdaptiveCircularScribblesPFM>());
    pfms.append(std::make_shared<AdaptiveShapesPFM>());
    pfms.append(std::make_shared<AdaptiveDashesPFM>());
    pfms.append(std::make_shared<AdaptiveTSPPFM>());
    pfms.append(std::make_shared<AdaptiveTriangulationPFM>());
    pfms.append(std::make_shared<AdaptiveTreePFM>());
    pfms.append(std::make_shared<AdaptiveDiagramPFM>());
    pfms.append(std::make_shared<AdaptiveLettersPFM>());
    pfms.append(std::make_shared<LBGCircularScribblesPFM>());
    pfms.append(std::make_shared<LBGShapesPFM>());
    pfms.append(std::make_shared<LBGTriangulationPFM>());
    pfms.append(std::make_shared<LBGTreePFM>());
    pfms.append(std::make_shared<LBGStipplingPFM>());
    pfms.append(std::make_shared<LBGDashesPFM>());
    pfms.append(std::make_shared<LBGDiagramPFM>());
    pfms.append(std::make_shared<LBGTSPPFM>());
    pfms.append(std::make_shared<LBGLettersPFM>());
    pfms.append(std::make_shared<VoronoiStipplingPFM>());
    pfms.append(std::make_shared<VoronoiCirclesPFM>());
    pfms.append(std::make_shared<VoronoiTriangulationPFM>());
    pfms.append(std::make_shared<VoronoiTreePFM>());
    pfms.append(std::make_shared<VoronoiDashesPFM>());
    pfms.append(std::make_shared<VoronoiDiagramPFM>());
    pfms.append(std::make_shared<VoronoiShapesPFM>());
    pfms.append(std::make_shared<VoronoiTSPPFM>());
    pfms.append(std::make_shared<VoronoiLettersPFM>());
    pfms.append(std::make_shared<TSPClassicPFM>());
    pfms.append(std::make_shared<TSPOutlinePFM>());
    pfms.append(std::make_shared<TSPShadingPFM>());
    pfms.append(std::make_shared<TSPStipplePFM>());
    pfms.append(std::make_shared<TSPVoronoiPFM>());
    pfms.append(std::make_shared<TSPMSTPFM>());
    pfms.append(std::make_shared<MazeRectPFM>());
    pfms.append(std::make_shared<MazeHexPFM>());
    pfms.append(std::make_shared<MazeTriPFM>());
    pfms.append(std::make_shared<MazeVoronoiPFM>());
    pfms.append(std::make_shared<MazeCirclePFM>());
    pfms.append(std::make_shared<MazeLabyrinthPFM>());
    pfms.append(std::make_shared<MazeHilbertPFM>());
    pfms.append(std::make_shared<MazePeanoPFM>());
    pfms.append(std::make_shared<LabyrinthClassicPFM>());
    pfms.append(std::make_shared<LabyrinthRomanPFM>());
    pfms.append(std::make_shared<GridShapesPFM>());
    pfms.append(std::make_shared<GridDashesPFM>());
    pfms.append(std::make_shared<GridLettersPFM>());
    pfms.append(std::make_shared<MosaicRectanglesPFM>());
    pfms.append(std::make_shared<MosaicVoronoiPFM>());
    pfms.append(std::make_shared<MosaicCustomPFM>());
    pfms.append(std::make_shared<SpiralCircularScribblesPFM>());
    pfms.append(std::make_shared<SpiralSawtoothPFM>());
    pfms.append(std::make_shared<StreamlinesFlowFieldPFM>());
    pfms.append(std::make_shared<StreamlinesEdgeFieldPFM>());
    pfms.append(std::make_shared<StreamlinesSuperformulaPFM>());
    pfms.append(std::make_shared<ContourPathsPFM>());
    pfms.append(std::make_shared<AmbientFlowPFM>());
    pfms.append(std::make_shared<EdgeShadingPFM>());
    pfms.append(std::make_shared<LayersPFM>());
    pfms.append(std::make_shared<StippleLayersPFM>());
    return pfms;
}

QVector<std::shared_ptr<ImageFilter>> ModuleRegistry::createAllFilters() {
    QVector<std::shared_ptr<ImageFilter>> filters;
    filters.append(std::make_shared<BrightnessFilter>());
    filters.append(std::make_shared<ContrastFilter>());
    filters.append(std::make_shared<InvertFilter>());
    filters.append(std::make_shared<ThresholdFilter>());
    filters.append(std::make_shared<UnsharpMaskFilter>());
    filters.append(std::make_shared<GrayscaleFilter>());
    filters.append(std::make_shared<DesaturateFilter>());
    filters.append(std::make_shared<SaturationFilter>());
    filters.append(std::make_shared<HueFilter>());
    filters.append(std::make_shared<GammaFilter>());
    filters.append(std::make_shared<ExposureFilter>());
    filters.append(std::make_shared<SepiaFilter>());
    filters.append(std::make_shared<CLAHEFilter>());
    filters.append(std::make_shared<TemperatureFilter>());
    filters.append(std::make_shared<CannyFilter>());
    filters.append(std::make_shared<SobelFilter>());
    filters.append(std::make_shared<LaplacianFilter>());
    filters.append(std::make_shared<PrewittFilter>());
    filters.append(std::make_shared<ScharrFilter>());
    filters.append(std::make_shared<DoGFilter>());
    filters.append(std::make_shared<RidgeDetectionFilter>());
    filters.append(std::make_shared<HighPassFilter>());
    filters.append(std::make_shared<GaussianBlurFilter>());
    filters.append(std::make_shared<MedianBlurFilter>());
    filters.append(std::make_shared<BoxBlurFilter>());
    filters.append(std::make_shared<MotionBlurFilter>());
    filters.append(std::make_shared<BilateralFilter>());
    filters.append(std::make_shared<LowPassFilter>());
    filters.append(std::make_shared<SharpenMoreFilter>());
    filters.append(std::make_shared<GaussianNoiseFilter>());
    filters.append(std::make_shared<SaltAndPepperFilter>());
    filters.append(std::make_shared<DenoiseFilter>());
    filters.append(std::make_shared<GaussianNoise2Filter>());
    filters.append(std::make_shared<SpeckleNoiseFilter>());
    filters.append(std::make_shared<DilationFilter>());
    filters.append(std::make_shared<ErosionFilter>());
    filters.append(std::make_shared<OpeningFilter>());
    filters.append(std::make_shared<ClosingFilter>());
    filters.append(std::make_shared<MorphGradientFilter>());
    filters.append(std::make_shared<TopHatFilter>());
    filters.append(std::make_shared<BlackHatFilter>());
    filters.append(std::make_shared<DilateCrossFilter>());
    filters.append(std::make_shared<ErodeCrossFilter>());
    filters.append(std::make_shared<DilateEllipseFilter>());
    filters.append(std::make_shared<ErodeEllipseFilter>());
    filters.append(std::make_shared<PosterizeFilter>());
    filters.append(std::make_shared<EdgePreserveFilter>());
    filters.append(std::make_shared<StylizationFilter>());
    filters.append(std::make_shared<OilPaintingFilter>());
    filters.append(std::make_shared<DetailEnhanceFilter>());
    filters.append(std::make_shared<PencilSketchFilter>());
    filters.append(std::make_shared<EmbossFilter>());
    filters.append(std::make_shared<QuantizeFilter>());
    filters.append(std::make_shared<VignetteFilter>());
    filters.append(std::make_shared<PixelateFilter>());
    filters.append(std::make_shared<WaveFilter>());
    filters.append(std::make_shared<OtsuThresholdFilter>());
    filters.append(std::make_shared<AdaptiveThresholdFilter>());
    filters.append(std::make_shared<TruncateThresholdFilter>());
    filters.append(std::make_shared<ToZeroThresholdFilter>());
    filters.append(std::make_shared<EqualizeHistFilter>());
    filters.append(std::make_shared<AutoContrastFilter>());
    filters.append(std::make_shared<AutoColorFilter>());
    filters.append(std::make_shared<ColorizeFilter>());
    filters.append(std::make_shared<InvertHueFilter>());
    return filters;
}
