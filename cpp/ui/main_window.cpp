/**
 * main_window.cpp — Qt GUI for imgto2d (C++ replacement of tk_gui_fast.py).
 *
 * Layout mirrors TkApp:
 *   Left panel:  Load | PFM select | Start | Fast Preview | Resolution | Reset
 *                Dynamic settings panel (scrollable) — new vs. Tkinter version
 *   Right panel: Preview canvas (QLabel with QPixmap)
 */

#include "ui/main_window.h"

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QDoubleSpinBox>
#include <QDir>
#include <QSpinBox>
#include <QScrollArea>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QResizeEvent>
#include <QFont>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QProcess>
#include <QTemporaryFile>

#ifdef _DEBUG
  #undef _DEBUG
  #pragma push_macro("slots")
  #undef slots
  #include <Python.h>
  #pragma pop_macro("slots")
  #define _DEBUG
#else
  #pragma push_macro("slots")
  #undef slots
  #include <Python.h>
  #pragma pop_macro("slots")
#endif

static bool runVpypeEmbedded(const QString& tempPath, const QString& pipeline, const QString& filepath, QString& outErrorMsg) {
    PyGILState_STATE gstate = PyGILState_Ensure();

    bool success = false;
    PyObject* pModule = PyImport_ImportModule("vpype_cli");
    if (!pModule) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        if (pvalue) {
            PyObject* pstr = PyObject_Str(pvalue);
            if (pstr) {
                outErrorMsg = QString("Could not import vpype_cli: %1").arg(PyUnicode_AsUTF8(pstr));
                Py_DECREF(pstr);
            } else {
                outErrorMsg = "Could not import vpype_cli module (unknown error).";
            }
            Py_DECREF(pvalue);
        } else {
            outErrorMsg = "Could not import vpype_cli module.";
        }
        Py_XDECREF(ptype);
        Py_XDECREF(ptraceback);
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pFunc = PyObject_GetAttrString(pModule, "execute");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        outErrorMsg = "vpype_cli.execute is not callable.";
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
        PyGILState_Release(gstate);
        return false;
    }

    QString fullPipeline = QString("read \"%1\" %2 write \"%3\"")
                               .arg(tempPath)
                               .arg(pipeline)
                               .arg(filepath);

    PyObject* pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(fullPipeline.toUtf8().constData()));

    PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);

    if (pResult) {
        success = true;
        Py_DECREF(pResult);
    } else {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        if (pvalue) {
            PyObject* pstr = PyObject_Str(pvalue);
            if (pstr) {
                outErrorMsg = QString("vpype error: %1").arg(PyUnicode_AsUTF8(pstr));
                Py_DECREF(pstr);
            } else {
                outErrorMsg = "vpype execution failed (unknown error).";
            }
            Py_DECREF(pvalue);
        } else {
            outErrorMsg = "vpype execution failed.";
        }
        Py_XDECREF(ptype);
        Py_XDECREF(ptraceback);
    }

    Py_DECREF(pFunc);
    Py_DECREF(pModule);

    PyGILState_Release(gstate);
    return success;
}

#include "export/svg_exporter.h"
#include "export/gcode_exporter.h"
#include "export/hpgl_exporter.h"
#include "export/pdf_exporter.h"
#include "batch/batch_processor.h"
#ifdef HAS_SERIAL_PORT
#include "ui/serial_dialog.h"
#endif
#include <QStyleFactory>
#include <QFrame>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <algorithm>

// PFM includes — all families
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

// Filter includes
#include "filters/basic_filters.h"
#include "filters/color_filters.h"
#include "filters/edge_blur_filters.h"
#include "filters/noise_filters.h"
#include "filters/morph_filters.h"
#include "filters/artistic_distort_filters.h"
#include "filters/threshold_extra_filters.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("imgto2d — Qt Fast Preview");
    resize(1200, 800);

    // -----------------------------------------------------------------------
    // Register all PFMs — 1:1 with DrawingBot V3
    // -----------------------------------------------------------------------

    // --- Sketch family ---
    m_pfms.append(std::make_shared<SketchLinesPFM>());
    m_pfms.append(std::make_shared<SketchSquaresPFM>());
    m_pfms.append(std::make_shared<SketchCurvesPFM>());
    m_pfms.append(std::make_shared<SketchShapesPFM>());
    m_pfms.append(std::make_shared<SketchQuadBeziersPFM>());
    m_pfms.append(std::make_shared<SketchCubicBeziersPFM>());
    m_pfms.append(std::make_shared<SketchWavesPFM>());
    m_pfms.append(std::make_shared<SketchSpiralsPFM>());
    m_pfms.append(std::make_shared<SketchSuperformulaPFM>());
    m_pfms.append(std::make_shared<SketchCubicBeziers2PFM>());
    m_pfms.append(std::make_shared<SketchQuadBeziers2PFM>());
    m_pfms.append(std::make_shared<SketchSweepingCurvesPFM>());
    m_pfms.append(std::make_shared<SketchFlowFieldsPFM>());
    m_pfms.append(std::make_shared<SketchVoronoiPFM>());
    m_pfms.append(std::make_shared<SketchDelaunayPFM>());
    m_pfms.append(std::make_shared<SketchRadialPFM>());
    m_pfms.append(std::make_shared<SketchScribblePFM>());
    m_pfms.append(std::make_shared<SketchAbstractPFM>());
    m_pfms.append(std::make_shared<SketchCatmullRomsPFM>());
    m_pfms.append(std::make_shared<SketchSobelEdgesPFM>());

    // --- Hatch family ---
    m_pfms.append(std::make_shared<HatchLinesPFM>());
    m_pfms.append(std::make_shared<HatchHorizontalPFM>());
    m_pfms.append(std::make_shared<HatchVerticalPFM>());
    m_pfms.append(std::make_shared<HatchDiagonal1PFM>());
    m_pfms.append(std::make_shared<HatchDiagonal2PFM>());
    m_pfms.append(std::make_shared<HatchGridPFM>());
    m_pfms.append(std::make_shared<HatchCrossPFM>());
    m_pfms.append(std::make_shared<Hatch3WayPFM>());
    m_pfms.append(std::make_shared<Hatch4WayPFM>());
    m_pfms.append(std::make_shared<Hatch5WayPFM>());
    m_pfms.append(std::make_shared<Hatch6WayPFM>());
    m_pfms.append(std::make_shared<HatchSawtoothPFM>());
    m_pfms.append(std::make_shared<HatchCircularScribblesPFM>());

    // --- Stipple family ---
    m_pfms.append(std::make_shared<StippleDotsPFM>());
    m_pfms.append(std::make_shared<StippleCirclesPFM>());
    m_pfms.append(std::make_shared<StippleSquaresPFM>());
    m_pfms.append(std::make_shared<StippleTrianglesPFM>());
    m_pfms.append(std::make_shared<StippleHexagonsPFM>());
    m_pfms.append(std::make_shared<StippleStarsPFM>());
    m_pfms.append(std::make_shared<StippleCrossesPFM>());
    m_pfms.append(std::make_shared<StippleLinesPFM>());
    m_pfms.append(std::make_shared<StippleVariableCirclesPFM>());
    m_pfms.append(std::make_shared<StippleVariableSquaresPFM>());
    m_pfms.append(std::make_shared<StippleChaosPFM>());

    // --- Adaptive family ---
    m_pfms.append(std::make_shared<AdaptiveStipplingPFM>());
    m_pfms.append(std::make_shared<AdaptiveCircularScribblesPFM>());
    m_pfms.append(std::make_shared<AdaptiveShapesPFM>());
    m_pfms.append(std::make_shared<AdaptiveDashesPFM>());
    m_pfms.append(std::make_shared<AdaptiveTSPPFM>());
    m_pfms.append(std::make_shared<AdaptiveTriangulationPFM>());
    m_pfms.append(std::make_shared<AdaptiveTreePFM>());
    m_pfms.append(std::make_shared<AdaptiveDiagramPFM>());
    m_pfms.append(std::make_shared<AdaptiveLettersPFM>());

    // --- LBG family ---
    m_pfms.append(std::make_shared<LBGCircularScribblesPFM>());
    m_pfms.append(std::make_shared<LBGShapesPFM>());
    m_pfms.append(std::make_shared<LBGTriangulationPFM>());
    m_pfms.append(std::make_shared<LBGTreePFM>());
    m_pfms.append(std::make_shared<LBGStipplingPFM>());
    m_pfms.append(std::make_shared<LBGDashesPFM>());
    m_pfms.append(std::make_shared<LBGDiagramPFM>());
    m_pfms.append(std::make_shared<LBGTSPPFM>());
    m_pfms.append(std::make_shared<LBGLettersPFM>());

    // --- Voronoi family ---
    m_pfms.append(std::make_shared<VoronoiStipplingPFM>());
    m_pfms.append(std::make_shared<VoronoiCirclesPFM>());
    m_pfms.append(std::make_shared<VoronoiTriangulationPFM>());
    m_pfms.append(std::make_shared<VoronoiTreePFM>());
    m_pfms.append(std::make_shared<VoronoiDashesPFM>());
    m_pfms.append(std::make_shared<VoronoiDiagramPFM>());
    m_pfms.append(std::make_shared<VoronoiShapesPFM>());
    m_pfms.append(std::make_shared<VoronoiTSPPFM>());
    m_pfms.append(std::make_shared<VoronoiLettersPFM>());

    // --- TSP family ---
    m_pfms.append(std::make_shared<TSPClassicPFM>());
    m_pfms.append(std::make_shared<TSPOutlinePFM>());
    m_pfms.append(std::make_shared<TSPShadingPFM>());
    m_pfms.append(std::make_shared<TSPStipplePFM>());
    m_pfms.append(std::make_shared<TSPVoronoiPFM>());
    m_pfms.append(std::make_shared<TSPMSTPFM>());

    // --- Maze / Labyrinth family ---
    m_pfms.append(std::make_shared<MazeRectPFM>());
    m_pfms.append(std::make_shared<MazeHexPFM>());
    m_pfms.append(std::make_shared<MazeTriPFM>());
    m_pfms.append(std::make_shared<MazeVoronoiPFM>());
    m_pfms.append(std::make_shared<MazeCirclePFM>());
    m_pfms.append(std::make_shared<MazeLabyrinthPFM>());
    m_pfms.append(std::make_shared<MazeHilbertPFM>());
    m_pfms.append(std::make_shared<MazePeanoPFM>());
    m_pfms.append(std::make_shared<LabyrinthClassicPFM>());
    m_pfms.append(std::make_shared<LabyrinthRomanPFM>());

    // --- Grid family ---
    m_pfms.append(std::make_shared<GridShapesPFM>());
    m_pfms.append(std::make_shared<GridDashesPFM>());
    m_pfms.append(std::make_shared<GridLettersPFM>());

    // --- Mosaic family ---
    m_pfms.append(std::make_shared<MosaicRectanglesPFM>());
    m_pfms.append(std::make_shared<MosaicVoronoiPFM>());
    m_pfms.append(std::make_shared<MosaicCustomPFM>());

    // --- Spiral family ---
    m_pfms.append(std::make_shared<SpiralCircularScribblesPFM>());
    m_pfms.append(std::make_shared<SpiralSawtoothPFM>());

    // --- Streamlines family ---
    m_pfms.append(std::make_shared<StreamlinesFlowFieldPFM>());
    m_pfms.append(std::make_shared<StreamlinesEdgeFieldPFM>());
    m_pfms.append(std::make_shared<StreamlinesSuperformulaPFM>());

    // --- Special family ---
    m_pfms.append(std::make_shared<ContourPathsPFM>());
    m_pfms.append(std::make_shared<AmbientFlowPFM>());
    m_pfms.append(std::make_shared<EdgeShadingPFM>());

    // --- Composite family ---
    m_pfms.append(std::make_shared<LayersPFM>());
    m_pfms.append(std::make_shared<StippleLayersPFM>());

    // -----------------------------------------------------------------------
    // Register Filters
    // -----------------------------------------------------------------------
    m_availableFilters.append(std::make_shared<BrightnessFilter>());
    m_availableFilters.append(std::make_shared<ContrastFilter>());
    m_availableFilters.append(std::make_shared<InvertFilter>());
    m_availableFilters.append(std::make_shared<ThresholdFilter>());
    m_availableFilters.append(std::make_shared<UnsharpMaskFilter>());
    
    // Color filters
    m_availableFilters.append(std::make_shared<GrayscaleFilter>());
    m_availableFilters.append(std::make_shared<DesaturateFilter>());
    m_availableFilters.append(std::make_shared<SaturationFilter>());
    m_availableFilters.append(std::make_shared<HueFilter>());
    m_availableFilters.append(std::make_shared<GammaFilter>());
    m_availableFilters.append(std::make_shared<ExposureFilter>());
    m_availableFilters.append(std::make_shared<SepiaFilter>());
    m_availableFilters.append(std::make_shared<CLAHEFilter>());
    m_availableFilters.append(std::make_shared<TemperatureFilter>());

    // Edge filters
    m_availableFilters.append(std::make_shared<CannyFilter>());
    m_availableFilters.append(std::make_shared<SobelFilter>());
    m_availableFilters.append(std::make_shared<LaplacianFilter>());
    m_availableFilters.append(std::make_shared<PrewittFilter>());
    m_availableFilters.append(std::make_shared<ScharrFilter>());
    m_availableFilters.append(std::make_shared<DoGFilter>());
    m_availableFilters.append(std::make_shared<RidgeDetectionFilter>());
    m_availableFilters.append(std::make_shared<HighPassFilter>());

    // Blur filters
    m_availableFilters.append(std::make_shared<GaussianBlurFilter>());
    m_availableFilters.append(std::make_shared<MedianBlurFilter>());
    m_availableFilters.append(std::make_shared<BoxBlurFilter>());
    m_availableFilters.append(std::make_shared<MotionBlurFilter>());
    m_availableFilters.append(std::make_shared<BilateralFilter>());
    m_availableFilters.append(std::make_shared<LowPassFilter>());
    m_availableFilters.append(std::make_shared<SharpenMoreFilter>());


    // Noise filters
    m_availableFilters.append(std::make_shared<GaussianNoiseFilter>());
    m_availableFilters.append(std::make_shared<SaltAndPepperFilter>());
    m_availableFilters.append(std::make_shared<DenoiseFilter>());
    m_availableFilters.append(std::make_shared<GaussianNoise2Filter>());
    m_availableFilters.append(std::make_shared<SpeckleNoiseFilter>());

    // Morph filters
    m_availableFilters.append(std::make_shared<DilationFilter>());
    m_availableFilters.append(std::make_shared<ErosionFilter>());
    m_availableFilters.append(std::make_shared<OpeningFilter>());
    m_availableFilters.append(std::make_shared<ClosingFilter>());
    m_availableFilters.append(std::make_shared<MorphGradientFilter>());
    m_availableFilters.append(std::make_shared<TopHatFilter>());
    m_availableFilters.append(std::make_shared<BlackHatFilter>());
    m_availableFilters.append(std::make_shared<DilateCrossFilter>());
    m_availableFilters.append(std::make_shared<ErodeCrossFilter>());
    m_availableFilters.append(std::make_shared<DilateEllipseFilter>());
    m_availableFilters.append(std::make_shared<ErodeEllipseFilter>());

    // Artistic / Distort filters
    m_availableFilters.append(std::make_shared<PosterizeFilter>());
    m_availableFilters.append(std::make_shared<EdgePreserveFilter>());
    m_availableFilters.append(std::make_shared<StylizationFilter>());
    m_availableFilters.append(std::make_shared<OilPaintingFilter>());
    m_availableFilters.append(std::make_shared<DetailEnhanceFilter>());
    m_availableFilters.append(std::make_shared<PencilSketchFilter>());
    m_availableFilters.append(std::make_shared<EmbossFilter>());
    m_availableFilters.append(std::make_shared<QuantizeFilter>());
    m_availableFilters.append(std::make_shared<VignetteFilter>());
    m_availableFilters.append(std::make_shared<PixelateFilter>());
    m_availableFilters.append(std::make_shared<WaveFilter>());

    // Threshold / Extra filters
    m_availableFilters.append(std::make_shared<OtsuThresholdFilter>());
    m_availableFilters.append(std::make_shared<AdaptiveThresholdFilter>());
    m_availableFilters.append(std::make_shared<TruncateThresholdFilter>());
    m_availableFilters.append(std::make_shared<ToZeroThresholdFilter>());
    m_availableFilters.append(std::make_shared<EqualizeHistFilter>());
    m_availableFilters.append(std::make_shared<AutoContrastFilter>());
    m_availableFilters.append(std::make_shared<AutoColorFilter>());
    m_availableFilters.append(std::make_shared<ColorizeFilter>());
    m_availableFilters.append(std::make_shared<InvertHueFilter>());




    buildUI();
    createMenus();
    populateSettingsPanel(); // -1 means PFM is default
}



MainWindow::~MainWindow() {
    if (m_worker && m_worker->isRunning()) {
        // Cancel and wait up to 3 seconds
        auto pfm = m_pfms.value(m_pfmCombo->currentIndex());
        if (pfm) pfm->cancel();
        m_worker->wait(3000);
    }
}

// ---------------------------------------------------------------------------
// Build UI
// ---------------------------------------------------------------------------
void MainWindow::createMenus() {
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* openProjectAction = new QAction("Open Project...", this);
    openProjectAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::onOpenProject);
    fileMenu->addAction(openProjectAction);

    QAction* saveProjectAction = new QAction("Save Project", this);
    saveProjectAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(saveProjectAction, &QAction::triggered, this, &MainWindow::onSaveProject);
    fileMenu->addAction(saveProjectAction);

    QAction* saveProjectAsAction = new QAction("Save Project As...", this);
    connect(saveProjectAsAction, &QAction::triggered, this, &MainWindow::onSaveProjectAs);
    fileMenu->addAction(saveProjectAsAction);

    fileMenu->addSeparator();

    QAction* importVideoAction = new QAction("Import Video Frame...", this);
    connect(importVideoAction, &QAction::triggered, this, &MainWindow::onImportVideoFrame);
    fileMenu->addAction(importVideoAction);

    QAction* exportSettingsAction = new QAction("Export Settings...", this);
    connect(exportSettingsAction, &QAction::triggered, this, &MainWindow::onExportSettings);
    fileMenu->addAction(exportSettingsAction);

    QAction* penEditorAction = new QAction("Pen Editor...", this);
    connect(penEditorAction, &QAction::triggered, this, &MainWindow::onEditPens);
    fileMenu->addAction(penEditorAction);

    fileMenu->addSeparator();

    QAction* exportAction = new QAction("Export SVG...", this);
    exportAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportSVG);
    fileMenu->addAction(exportAction);

    QAction* exportGCodeAction = new QAction("Export G-Code...", this);
    connect(exportGCodeAction, &QAction::triggered, this, &MainWindow::onExportGCode);
    fileMenu->addAction(exportGCodeAction);

    QAction* exportHPGLAction = new QAction("Export HPGL...", this);
    connect(exportHPGLAction, &QAction::triggered, this, &MainWindow::onExportHPGL);
    fileMenu->addAction(exportHPGLAction);

    QAction* exportPDFAction = new QAction("Export PDF...", this);
    connect(exportPDFAction, &QAction::triggered, this, &MainWindow::onExportPDF);
    fileMenu->addAction(exportPDFAction);

    fileMenu->addSeparator();

    QAction* runBatchAction = new QAction("Batch Process Directory...", this);
    connect(runBatchAction, &QAction::triggered, this, &MainWindow::onRunBatch);
    fileMenu->addAction(runBatchAction);

#ifdef HAS_SERIAL_PORT
    QAction* streamAction = new QAction("Hardware Stream G-Code...", this);
    connect(streamAction, &QAction::triggered, this, &MainWindow::onStreamGCode);
    fileMenu->addAction(streamAction);
#endif

    fileMenu->addSeparator();

    QAction* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
}

void MainWindow::buildUI() {
    // Palette: dark theme
    QPalette p = QApplication::palette();
    p.setColor(QPalette::Window,         QColor(30, 30, 35));
    p.setColor(QPalette::WindowText,     QColor(220, 220, 225));
    p.setColor(QPalette::Base,           QColor(22, 22, 26));
    p.setColor(QPalette::AlternateBase,  QColor(35, 35, 42));
    p.setColor(QPalette::Button,         QColor(50, 50, 60));
    p.setColor(QPalette::ButtonText,     QColor(220, 220, 225));
    p.setColor(QPalette::Highlight,      QColor(100, 140, 255));
    p.setColor(QPalette::HighlightedText,QColor(255, 255, 255));
    p.setColor(QPalette::Text,           QColor(210, 210, 215));
    setPalette(p);

    // Fonts
    QFont uiFont("Segoe UI", 9);
    QApplication::setFont(uiFont);

    // Central widget + splitter
    auto* central  = new QWidget(this);
    auto* mainHBox = new QHBoxLayout(central);
    mainHBox->setContentsMargins(8, 8, 8, 8);
    mainHBox->setSpacing(8);
    setCentralWidget(central);

    // ---- Left panel ----
    m_leftPanel = new QWidget;
    m_leftPanel->setFixedWidth(300);
    auto* leftVBox = new QVBoxLayout(m_leftPanel);
    leftVBox->setContentsMargins(0, 0, 0, 0);
    leftVBox->setSpacing(6);

    // Title
    auto* titleLabel = new QLabel("imgto2d");
    QFont titleFont("Segoe UI", 14, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #8ab4ff; padding: 4px 0;");
    titleLabel->setAlignment(Qt::AlignHCenter);
    leftVBox->addWidget(titleLabel);

    // Separator
    auto* sep = new QFrame; sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #444;"); leftVBox->addWidget(sep);

    m_loadBtn = new QPushButton("📂  Load Image");
    m_loadBtn->setFixedHeight(36);
    m_loadBtn->setStyleSheet(
        "QPushButton { background: #3a4a7a; border-radius: 6px; color: #dde; font-weight: bold; }"
        "QPushButton:hover { background: #4a5a9a; }"
        "QPushButton:pressed { background: #2a3a6a; }");
    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadImage);
    leftVBox->addWidget(m_loadBtn);

    m_loadMaskBtn = new QPushButton("🎭  Load Mask...");
    m_loadMaskBtn->setFixedHeight(28);
    m_loadMaskBtn->setStyleSheet(
        "QPushButton { background: #444; border-radius: 6px; color: #ccc; }"
        "QPushButton:hover { background: #555; }"
        "QPushButton:pressed { background: #333; }");
    connect(m_loadMaskBtn, &QPushButton::clicked, this, &MainWindow::onLoadMask);
    leftVBox->addWidget(m_loadMaskBtn);

    // ---- Filter stack ----
    auto* filterGroup = new QGroupBox("Image Filters");
    filterGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #aac; border: 1px solid #444; border-radius:4px; margin-top:6px; }"
                               "QGroupBox::title { subcontrol-origin: margin; left: 8px; }");
    auto* filterVBox = new QVBoxLayout(filterGroup);
    
    auto* filterAddHBox = new QHBoxLayout;
    m_filterCombo = new QComboBox;
    for (auto& f : m_availableFilters) m_filterCombo->addItem(f->name());
    m_filterCombo->setStyleSheet("QComboBox { background: #2a2a36; border:1px solid #555; border-radius:4px; padding:3px; }");
    
    m_addFilterBtn = new QPushButton("➕");
    m_addFilterBtn->setFixedWidth(30);
    connect(m_addFilterBtn, &QPushButton::clicked, this, &MainWindow::onAddFilter);
    
    filterAddHBox->addWidget(m_filterCombo);
    filterAddHBox->addWidget(m_addFilterBtn);
    filterVBox->addLayout(filterAddHBox);

    m_filterList = new QListWidget;
    m_filterList->setFixedHeight(100);
    m_filterList->setStyleSheet("QListWidget { background: #1a1a20; border:1px solid #333; }");
    connect(m_filterList, &QListWidget::itemSelectionChanged, this, &MainWindow::onFilterListSelectionChanged);
    filterVBox->addWidget(m_filterList);

    auto* filterCtrlHBox = new QHBoxLayout;
    m_filterUpBtn = new QPushButton("⬆");
    m_filterDownBtn = new QPushButton("⬇");
    m_filterDelBtn = new QPushButton("🗑");
    connect(m_filterUpBtn, &QPushButton::clicked, this, &MainWindow::onMoveFilterUp);
    connect(m_filterDownBtn, &QPushButton::clicked, this, &MainWindow::onMoveFilterDown);
    connect(m_filterDelBtn, &QPushButton::clicked, this, &MainWindow::onRemoveFilter);
    filterCtrlHBox->addWidget(m_filterUpBtn);
    filterCtrlHBox->addWidget(m_filterDownBtn);
    filterCtrlHBox->addWidget(m_filterDelBtn);
    filterVBox->addLayout(filterCtrlHBox);
    
    leftVBox->addWidget(filterGroup);

    // ---- PFM selector ----
    auto* pfmGroup = new QGroupBox("Path Finding Module");
    pfmGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #aac; border: 1px solid #444; border-radius:4px; margin-top:6px; }"
                            "QGroupBox::title { subcontrol-origin: margin; left: 8px; }");
    auto* pfmVBox = new QVBoxLayout(pfmGroup);
    m_pfmCombo = new QComboBox;
    for (auto& pfm : m_pfms)
        m_pfmCombo->addItem(pfm->name());
    m_pfmCombo->setStyleSheet("QComboBox { background: #2a2a36; border:1px solid #555; border-radius:4px; padding:3px; }");
    
    pfmVBox->addWidget(new QLabel("Path Finding Module:"));
    pfmVBox->addWidget(m_pfmCombo);

    pfmVBox->addWidget(new QLabel("Colour Separation:"));
    m_separationCombo = new QComboBox;
    m_separationCombo->addItems({"None", "CMYK", "Colour Match"});
    m_separationCombo->setStyleSheet("QComboBox { background: #2a2a36; border:1px solid #555; border-radius:4px; padding:3px; }");
    pfmVBox->addWidget(m_separationCombo);

    connect(m_pfmCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onPFMSelectionChanged);
    leftVBox->addWidget(pfmGroup);

    // ---- Start / Cancel ----
    m_startBtn = new QPushButton("▶  Start Processing");
    m_startBtn->setFixedHeight(38);
    m_startBtn->setStyleSheet(
        "QPushButton { background: #2a6a3a; border-radius: 6px; color: #cfe; font-weight: bold; font-size: 10pt; }"
        "QPushButton:hover { background: #3a8a4a; }"
        "QPushButton:pressed { background: #1a5a2a; }"
        "QPushButton:disabled { background: #2a3a2a; color: #667; }");
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::onStartProcessing);
    leftVBox->addWidget(m_startBtn);

    // ---- Options row ----
    auto* optGroup = new QGroupBox("Preview Options");
    optGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #aac; border: 1px solid #444; border-radius:4px; margin-top:6px; }"
                            "QGroupBox::title { subcontrol-origin: margin; left: 8px; }");
    auto* optForm = new QFormLayout(optGroup);
    optForm->setLabelAlignment(Qt::AlignLeft);
    optForm->setSpacing(4);

    m_fastPreview = new QCheckBox("Fast Preview (downsample)");
    m_fastPreview->setChecked(true);
    optForm->addRow(m_fastPreview);

    m_resCombo = new QComboBox;
    m_resCombo->addItems({"720p", "1080p", "1440p", "2160p", "Native"});
    m_resCombo->setCurrentIndex(1);
    m_resCombo->setStyleSheet("QComboBox { background: #2a2a36; border:1px solid #555; border-radius:4px; padding:2px; }");
    optForm->addRow("Resolution:", m_resCombo);

    leftVBox->addWidget(optGroup);

    // ---- Reset ----
    m_resetBtn = new QPushButton("🔄  Reset");
    m_resetBtn->setFixedHeight(32);
    m_resetBtn->setStyleSheet(
        "QPushButton { background: #5a3a2a; border-radius: 5px; color: #fed; }"
        "QPushButton:hover { background: #7a5a3a; }"
        "QPushButton:pressed { background: #4a2a1a; }");
    connect(m_resetBtn, &QPushButton::clicked, this, &MainWindow::onReset);
    leftVBox->addWidget(m_resetBtn);

    // ---- Progress bar ----
    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setFixedHeight(18);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 1px solid #444; border-radius: 4px; background: #1a1a22; text-align: center; color: #aae; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #3060c0,stop:1 #60a0ff); border-radius:3px; }");
    leftVBox->addWidget(m_progressBar);

    // ---- Status label ----
    m_statusLabel = new QLabel("Ready — load an image to start");
    m_statusLabel->setStyleSheet("color: #889; font-size: 8pt; padding: 2px;");
    m_statusLabel->setWordWrap(true);
    leftVBox->addWidget(m_statusLabel);

    // ---- Settings panel (scrollable) ----
    auto* settingsHeader = new QLabel("⚙  PFM Settings");
    settingsHeader->setStyleSheet("color: #8ab4ff; font-weight: bold; padding-top: 6px;");
    leftVBox->addWidget(settingsHeader);

    m_settingsScroll = new QScrollArea;
    m_settingsScroll->setWidgetResizable(true);
    m_settingsScroll->setStyleSheet("QScrollArea { border: 1px solid #333; border-radius:4px; background: #18181f; }");
    m_settingsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_settingsWidget = new QWidget;
    m_settingsForm   = new QFormLayout(m_settingsWidget);
    m_settingsForm->setSpacing(3);
    m_settingsForm->setContentsMargins(6, 6, 6, 6);
    m_settingsForm->setLabelAlignment(Qt::AlignLeft);
    m_settingsScroll->setWidget(m_settingsWidget);
    leftVBox->addWidget(m_settingsScroll, 1);

    // ---- Right panel: canvas ----
    m_canvas = new QLabel;
    m_canvas->setAlignment(Qt::AlignCenter);
    m_canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_canvas->setStyleSheet("background: #181820; border: 1px solid #333; border-radius: 6px;");
    m_canvas->setMinimumSize(400, 300);
    m_canvas->setText("<span style='color:#444; font-size:24pt;'>🖼</span><br/>"
                      "<span style='color:#555;'>Load an image to begin</span>");
    m_canvas->setTextFormat(Qt::RichText);

    mainHBox->addWidget(m_leftPanel);
    mainHBox->addWidget(m_canvas, 1);
}

// ---------------------------------------------------------------------------
// Dynamic settings panel
// ---------------------------------------------------------------------------
void MainWindow::populateSettingsPanel() {
    qDebug() << "populateSettingsPanel START. filterIdx=" << m_currentEditingFilterIdx;
    // Clear old widgets
    m_settingWidgets.clear();
    QLayoutItem* item;
    while ((item = m_settingsForm->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    QVector<PFMSetting> settingsList;
    if (m_currentEditingFilterIdx >= 0 && m_currentEditingFilterIdx < m_activeFilters.size()) {
        settingsList = m_activeFilters[m_currentEditingFilterIdx]->settingsList();
    } else {
        int pfmIndex = m_pfmCombo->currentIndex();
        if (pfmIndex >= 0 && pfmIndex < m_pfms.size()) {
            settingsList = m_pfms[pfmIndex]->settingsList();
        }
    }

    QString lastCategory;
    for (auto& s : settingsList) {
        if (s.category != lastCategory) {
            auto* catLabel = new QLabel(s.category.toUpper());
            catLabel->setStyleSheet("color: #8ab4ff; font-size: 8pt; font-weight: bold; padding-top:4px;");
            m_settingsForm->addRow(catLabel);
            lastCategory = s.category;
        }

        QWidget* widget = nullptr;
        switch (s.type) {
        case SettingType::Boolean: {
            auto* cb = new QCheckBox;
            cb->setChecked(s.toBool());
            connect(cb, &QCheckBox::stateChanged, this, &MainWindow::onSettingChanged);
            widget = cb;
            break;
        }
        case SettingType::Integer: {
            auto* spin = new QSpinBox;
            spin->setRange((int)s.minVal, (int)s.maxVal);
            spin->setValue(s.toInt());
            spin->setStyleSheet("QSpinBox { background: #2a2a36; border:1px solid #555; border-radius:3px; color:#ddd; padding:1px; }");
            connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSettingChanged);
            widget = spin;
            break;
        }
        default: {
            auto* dspin = new QDoubleSpinBox;
            dspin->setRange(s.minVal, s.maxVal);
            dspin->setSingleStep(s.step);
            dspin->setDecimals(s.step < 0.1 ? 2 : (s.step < 1.0 ? 1 : 0));
            dspin->setValue(s.toDouble());
            dspin->setStyleSheet("QDoubleSpinBox { background: #2a2a36; border:1px solid #555; border-radius:3px; color:#ddd; padding:1px; }");
            connect(dspin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onSettingChanged);
            widget = dspin;
            break;
        }
        }

        if (widget) {
            auto* lbl = new QLabel(s.label);
            lbl->setStyleSheet("color: #bbb; font-size: 8pt;");
            if (!s.tooltip.isEmpty()) { lbl->setToolTip(s.tooltip); widget->setToolTip(s.tooltip); }
            m_settingsForm->addRow(lbl, widget);
            m_settingWidgets.insert(s.key, widget);
        }
    }
}

void MainWindow::applySettingsToActiveObject() {
    QVector<PFMSetting> settingsList;
    if (m_currentEditingFilterIdx >= 0 && m_currentEditingFilterIdx < m_activeFilters.size()) {
        auto filter = m_activeFilters[m_currentEditingFilterIdx];
        for (auto& s : filter->settingsList()) {
            QWidget* w = m_settingWidgets.value(s.key, nullptr);
            if (!w) continue;
            if (auto* cb   = qobject_cast<QCheckBox*>(w))
                filter->set(s.key, cb->isChecked());
            else if (auto* sp = qobject_cast<QSpinBox*>(w))
                filter->set(s.key, sp->value());
            else if (auto* dp = qobject_cast<QDoubleSpinBox*>(w))
                filter->set(s.key, dp->value());
        }
    } else {
        int pfmIndex = m_pfmCombo->currentIndex();
        if (pfmIndex < 0 || pfmIndex >= m_pfms.size()) return;
        auto& pfm = m_pfms[pfmIndex];

        for (auto& s : pfm->settingsList()) {
            QWidget* w = m_settingWidgets.value(s.key, nullptr);
            if (!w) continue;
            if (auto* cb   = qobject_cast<QCheckBox*>(w))
                pfm->set(s.key, cb->isChecked());
            else if (auto* sp = qobject_cast<QSpinBox*>(w))
                pfm->set(s.key, sp->value());
            else if (auto* dp = qobject_cast<QDoubleSpinBox*>(w))
                pfm->set(s.key, dp->value());
        }
    }
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------
void MainWindow::onPFMSelectionChanged(int idx) {
    // If we select a PFM, we show its settings
    m_currentEditingFilterIdx = -1;
    m_filterList->clearSelection();
    populateSettingsPanel();
}

void MainWindow::onLoadImage() {
    QString path = QFileDialog::getOpenFileName(this, "Load Image", {},
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tif *.tiff *.webp)");
    if (path.isEmpty()) return;

    cv::Mat img = cv::imread(path.toStdString(), cv::IMREAD_COLOR);
    if (img.empty()) {
        m_statusLabel->setText("❌ Failed to load image");
        return;
    }
    m_image = img;
    m_imagePath = path;
    updatePreview();
    QFileInfo fi(path);
    m_statusLabel->setText(QString("Loaded: %1 (%2×%3)")
                           .arg(fi.fileName()).arg(img.cols).arg(img.rows));
    m_progressBar->setValue(0);
}

void MainWindow::onLoadMask() {
    QString filepath = QFileDialog::getOpenFileName(this, "Open Mask Image", "", "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (filepath.isEmpty()) return;
    
    if (m_maskMgr.load(filepath)) {
        m_loadMaskBtn->setText("🎭 Mask: " + QFileInfo(filepath).fileName());
        m_loadMaskBtn->setStyleSheet(
            "QPushButton { background: #2a6a4a; border-radius: 6px; color: #fff; font-weight: bold; }");
        updatePreview();
    }
}

void MainWindow::onImportVideoFrame() {
    QString path = QFileDialog::getOpenFileName(this, "Import Video Frame", {},
        "Video Files (*.mp4 *.mov *.avi *.mkv *.webm);;All Files (*)");
    if (path.isEmpty()) return;

    cv::VideoCapture cap(path.toStdString());
    if (!cap.isOpened()) {
        QMessageBox::warning(this, "Video", "Could not open video.");
        return;
    }

    int total = (int)cap.get(cv::CAP_PROP_FRAME_COUNT);
    int frameIdx = std::max(0, total / 2);
    if (total > 0) cap.set(cv::CAP_PROP_POS_FRAMES, frameIdx);

    cv::Mat frame;
    if (!cap.read(frame) || frame.empty()) {
        QMessageBox::warning(this, "Video", "Could not read frame.");
        return;
    }

    m_image = frame;
    m_imagePath = path;
    updatePreview();
    m_lastGeoms.clear();
    m_statusLabel->setText(QString("Video frame %1/%2 (%3x%4)")
                           .arg(frameIdx).arg(total).arg(frame.cols).arg(frame.rows));
    m_progressBar->setValue(0);
}

void MainWindow::onExportSettings() {
    bool ok = false;
    QString pipeline = QInputDialog::getText(
        this, "Export Settings", "vpype pipeline (blank disables vpype):",
        QLineEdit::Normal, m_useVpype ? m_vpypePipeline : QString(), &ok);
    if (!ok) return;

    m_useVpype = !pipeline.trimmed().isEmpty();
    if (m_useVpype) m_vpypePipeline = pipeline.trimmed();
    else m_vpypePipeline = "linemerge linesimplify";

    QMessageBox::information(this, "Export Settings",
        m_useVpype ? "vpype SVG optimization enabled." : "vpype SVG optimization disabled.");
}

QVector<QColor> MainWindow::paletteForCurrentMode(bool exportPalette) const {
    QString sepMode = m_separationCombo->currentText();
    QVector<QColor> fallback;
    if (sepMode == "CMYK") {
        fallback = {QColor(0, 255, 255), QColor(255, 0, 255), QColor(255, 255, 0), QColor(30, 30, 30)};
    } else if (sepMode == "Colour Match") {
        fallback = {QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255)};
    } else {
        fallback = {exportPalette ? QColor(0, 0, 0) : QColor(200, 220, 255)};
    }

    QVector<QColor> palette = m_penColors.isEmpty() ? fallback : m_penColors;
    while (palette.size() < fallback.size())
        palette.append(fallback[palette.size()]);
    return palette;
}

void MainWindow::onEditPens() {
    QDialog dlg(this);
    dlg.setWindowTitle("Pen Editor");
    auto* form = new QFormLayout(&dlg);

    QVector<QColor> colors = paletteForCurrentMode(true);
    QVector<QPushButton*> colorButtons;
    int editableCount = std::max(1, static_cast<int>(colors.size()));
    for (int i = 0; i < editableCount; ++i) {
        auto* btn = new QPushButton(colors[i].name().toUpper());
        btn->setStyleSheet(QString("background:%1; color:%2;")
                               .arg(colors[i].name())
                               .arg(colors[i].lightness() < 128 ? "#fff" : "#000"));
        connect(btn, &QPushButton::clicked, &dlg, [btn, &colors, i, this]() {
            QColor picked = QColorDialog::getColor(colors[i], this, "Choose Pen Color");
            if (!picked.isValid()) return;
            colors[i] = picked;
            btn->setText(picked.name().toUpper());
            btn->setStyleSheet(QString("background:%1; color:%2;")
                                   .arg(picked.name())
                                   .arg(picked.lightness() < 128 ? "#fff" : "#000"));
        });
        colorButtons.append(btn);
        form->addRow(QString("Pen %1 Color:").arg(i + 1), btn);
    }

    auto* widthSpin = new QDoubleSpinBox;
    widthSpin->setRange(0.05, 20.0);
    widthSpin->setSingleStep(0.05);
    widthSpin->setDecimals(2);
    widthSpin->setValue(m_penWidthMm);
    form->addRow("Stroke Width (mm):", widthSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        m_penColors = colors;
        m_penWidthMm = widthSpin->value();
        if (!m_lastGeoms.isEmpty()) renderAndShowGeometries(m_lastGeoms);
        m_statusLabel->setText(QString("Updated %1 pen(s), stroke %2 mm").arg(m_penColors.size()).arg(m_penWidthMm));
    }
}

void MainWindow::onOpenProject() {
    QString path = QFileDialog::getOpenFileName(this, "Open Project", {}, "DrawingBot Project (*.dbv3);;All Files (*)");
    if (path.isEmpty()) return;
    if (loadProjectFile(path)) {
        m_projectPath = path;
        setWindowTitle("imgto2d - " + QFileInfo(path).fileName());
    }
}

void MainWindow::onSaveProject() {
    if (m_projectPath.isEmpty()) {
        onSaveProjectAs();
        return;
    }
    saveProjectFile(m_projectPath);
}

void MainWindow::onSaveProjectAs() {
    QString path = QFileDialog::getSaveFileName(this, "Save Project", {}, "DrawingBot Project (*.dbv3)");
    if (path.isEmpty()) return;
    if (!path.endsWith(".dbv3", Qt::CaseInsensitive)) path += ".dbv3";
    if (saveProjectFile(path)) {
        m_projectPath = path;
        setWindowTitle("imgto2d - " + QFileInfo(path).fileName());
    }
}

bool MainWindow::saveProjectFile(const QString& path) {
    applySettingsToActiveObject();

    auto settingsToJson = [](const QMap<QString, PFMSetting>& settings) {
        QJsonObject obj;
        for (auto it = settings.constBegin(); it != settings.constEnd(); ++it)
            obj.insert(it.key(), QJsonValue::fromVariant(it.value().currentValue()));
        return obj;
    };

    int pfmIdx = m_pfmCombo->currentIndex();
    QJsonObject root;
    root["version"] = "1.6.5-cpp";
    root["image_path"] = m_imagePath;
    root["pfm_name"] = m_pfmCombo->currentText();
    if (pfmIdx >= 0 && pfmIdx < m_pfms.size())
        root["pfm_settings"] = settingsToJson(m_pfms[pfmIdx]->settingsMap());
    root["colour_separation"] = m_separationCombo->currentText();
    root["fast_preview"] = m_fastPreview->isChecked();
    root["preview_resolution"] = m_resCombo->currentText();
    root["use_vpype"] = m_useVpype;
    root["vpype_pipeline"] = m_vpypePipeline;
    root["pen_width_mm"] = m_penWidthMm;

    QJsonArray penColors;
    for (const QColor& color : m_penColors)
        penColors.append(color.name(QColor::HexRgb));
    root["pen_colors"] = penColors;

    QJsonObject mask;
    mask["enabled"] = m_maskMgr.isEnabled();
    mask["mask_path"] = m_maskMgr.maskPath();
    root["mask_settings"] = mask;

    QJsonArray filters;
    for (auto& filter : m_activeFilters) {
        QJsonObject f;
        f["name"] = filter->name();
        f["settings"] = settingsToJson(filter->settingsMap());
        filters.append(f);
    }
    root["filter_chain"] = filters;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(this, "Save Project", "Could not write project:\n" + file.errorString());
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    m_statusLabel->setText("Saved project: " + QFileInfo(path).fileName());
    return true;
}

bool MainWindow::loadProjectFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Open Project", "Could not read project:\n" + file.errorString());
        return false;
    }
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        QMessageBox::critical(this, "Open Project", "Invalid project JSON:\n" + err.errorString());
        return false;
    }

    QJsonObject root = doc.object();
    m_imagePath = root.value("image_path").toString();
    m_image = cv::Mat();
    if (!m_imagePath.isEmpty() && QFileInfo::exists(m_imagePath)) {
        cv::Mat img = cv::imread(m_imagePath.toStdString(), cv::IMREAD_COLOR);
        if (!img.empty()) m_image = img;
    } else if (!m_imagePath.isEmpty()) {
        QMessageBox::warning(this, "Open Project", "Project image path was not found:\n" + m_imagePath);
    }

    QString pfmName = root.value("pfm_name").toString("Sketch Lines");
    for (int i = 0; i < m_pfms.size(); ++i) {
        if (m_pfms[i]->name() == pfmName) {
            m_pfmCombo->setCurrentIndex(i);
            break;
        }
    }
    QJsonObject pfmSettings = root.value("pfm_settings").toObject();
    int pfmIdx = m_pfmCombo->currentIndex();
    if (pfmIdx >= 0 && pfmIdx < m_pfms.size()) {
        for (auto it = pfmSettings.constBegin(); it != pfmSettings.constEnd(); ++it)
            m_pfms[pfmIdx]->set(it.key(), it.value().toVariant());
    }

    m_activeFilters.clear();
    m_filterList->clear();
    for (const auto& v : root.value("filter_chain").toArray()) {
        QJsonObject fObj = v.toObject();
        auto filter = createFilterByName(fObj.value("name").toString());
        if (!filter) continue;
        QJsonObject settings = fObj.value("settings").toObject();
        for (auto it = settings.constBegin(); it != settings.constEnd(); ++it)
            filter->set(it.key(), it.value().toVariant());
        m_activeFilters.append(filter);
        m_filterList->addItem(filter->name());
    }

    QJsonObject mask = root.value("mask_settings").toObject();
    QString maskPath = mask.value("mask_path").toString();
    m_maskMgr.clear();
    m_loadMaskBtn->setText("Load Mask...");
    if (mask.value("enabled").toBool(false) && !maskPath.isEmpty()) {
        if (QFileInfo::exists(maskPath) && m_maskMgr.load(maskPath)) {
            m_loadMaskBtn->setText("Mask: " + QFileInfo(maskPath).fileName());
        } else {
            QMessageBox::warning(this, "Open Project", "Project mask path was not found:\n" + maskPath);
        }
    }

    m_separationCombo->setCurrentText(root.value("colour_separation").toString("None"));
    m_fastPreview->setChecked(root.value("fast_preview").toBool(true));
    m_resCombo->setCurrentText(root.value("preview_resolution").toString("1080p"));
    m_useVpype = root.value("use_vpype").toBool(false);
    m_vpypePipeline = root.value("vpype_pipeline").toString("linemerge linesimplify");
    m_penWidthMm = root.value("pen_width_mm").toDouble(0.5);
    QVector<QColor> loadedPenColors;
    for (const auto& v : root.value("pen_colors").toArray()) {
        QColor color(v.toString());
        if (color.isValid()) loadedPenColors.append(color);
    }
    if (!loadedPenColors.isEmpty()) m_penColors = loadedPenColors;
    m_currentEditingFilterIdx = -1;
    m_filterList->clearSelection();
    populateSettingsPanel();
    updatePreview();
    m_lastGeoms.clear();
    m_statusLabel->setText("Opened project: " + QFileInfo(path).fileName());
    return true;
}

void MainWindow::onStartProcessing() {
    qDebug() << "onStartProcessing START";
    if (m_image.empty()) {
        qDebug() << "onStartProcessing: image is empty, returning";
        QMessageBox::warning(this, "Error", "Please load an image first.");
        return;
    }

    if (m_worker && m_worker->isRunning()) {
        qDebug() << "onStartProcessing: canceling existing worker";
        // Cancel current run
        auto pfm = m_pfms.value(m_pfmCombo->currentIndex());
        if (pfm) pfm->cancel();
        m_startBtn->setText("▶  Start Processing");
        m_startBtn->setStyleSheet(
            "QPushButton { background: #2a6a3a; border-radius:6px; color:#cfe; font-weight:bold; font-size:10pt;}"
            "QPushButton:hover{background:#3a8a4a;} QPushButton:pressed{background:#1a5a2a;}"
            "QPushButton:disabled{background:#2a3a2a;color:#667;}");
        m_statusLabel->setText("Cancelling...");
        return;
    }

    // Apply settings to whichever object is currently being edited
    applySettingsToActiveObject();

    int idx = m_pfmCombo->currentIndex();
    if (idx < 0 || idx >= m_pfms.size()) return;
    auto& pfm = m_pfms[idx];

    // Process image through filter stack
    cv::Mat filteredImage = m_image.clone();
    for (auto& filter : m_activeFilters) {
        filteredImage = filter->process(filteredImage);
    }
    filteredImage = m_maskMgr.applyMask(filteredImage);

    // Reset processing state
    m_lastGeoms.clear();
    m_progressBar->setValue(0);
    
    // Downsample if fast preview is requested
    cv::Mat processImg = filteredImage;
    if (m_fastPreview->isChecked() && (filteredImage.cols > 800 || filteredImage.rows > 800)) {
        float scale = 800.0f / std::max(filteredImage.cols, filteredImage.rows);
        cv::resize(filteredImage, processImg, cv::Size(), scale, scale, cv::INTER_AREA);
    }

    m_startBtn->setText("⏹  Cancel");
    m_startBtn->setStyleSheet(
        "QPushButton { background: #6a2a2a; border-radius:6px; color:#fcc; font-weight:bold; font-size:10pt;}"
        "QPushButton:hover{background:#8a3a3a;} QPushButton:pressed{background:#5a1a1a;}");

    m_statusLabel->setText(QString("Processing with %1...").arg(pfm->name()));

    QString sepMode = m_separationCombo->currentText();
    qDebug() << "onStartProcessing: starting worker thread with PFM:" << pfm->name() << " Mode:" << sepMode;
    m_worker = new PFMWorker(pfm.get(), processImg, sepMode, this);
    connect(m_worker, &PFMWorker::finished,       this, &MainWindow::onProcessingFinished);

    connect(m_worker, &PFMWorker::progressUpdate, this, &MainWindow::onProgressUpdate);
    connect(m_worker, &PFMWorker::errorOccurred,  this, &MainWindow::onProcessingError);
    connect(m_worker, &QThread::finished, m_worker, &QObject::deleteLater);

    m_worker->start();
    qDebug() << "onStartProcessing END";
}

void MainWindow::onReset() {
    if (m_worker && m_worker->isRunning()) {
        auto pfm = m_pfms.value(m_pfmCombo->currentIndex());
        if (pfm) pfm->cancel();
    }
    m_image = cv::Mat();
    m_activeFilters.clear();
    m_filterList->clear();
    m_lastGeoms.clear();
    m_canvas->clear();
    m_canvas->setText("<span style='color:#444; font-size:24pt;'>🖼</span><br/>"
                      "<span style='color:#555;'>Load an image to begin</span>");
    m_canvas->setTextFormat(Qt::RichText);
    m_statusLabel->setText("Reset");
    m_progressBar->setValue(0);
    m_startBtn->setText("▶  Start Processing");
    m_startBtn->setStyleSheet(
        "QPushButton { background: #2a6a3a; border-radius:6px; color:#cfe; font-weight:bold; font-size:10pt;}"
        "QPushButton:hover{background:#3a8a4a;} QPushButton:pressed{background:#1a5a2a;}"
        "QPushButton:disabled{background:#2a3a2a;color:#667;}");
}

void MainWindow::onProcessingFinished(QVector<DrawingGeometry> geometries) {
    m_lastGeoms = geometries;
    m_startBtn->setText("▶  Start Processing");
    m_startBtn->setStyleSheet(
        "QPushButton { background: #2a6a3a; border-radius:6px; color:#cfe; font-weight:bold; font-size:10pt;}"
        "QPushButton:hover{background:#3a8a4a;} QPushButton:pressed{background:#1a5a2a;}"
        "QPushButton:disabled{background:#2a3a2a;color:#667;}");
    m_statusLabel->setText(QString("✅ Done — %1 shapes").arg(geometries.size()));
    m_progressBar->setValue(100);

    renderAndShowGeometries(geometries);
}

void MainWindow::onProgressUpdate(float pct, int shapes, QString text) {
    m_progressBar->setValue((int)(pct * 100));
    m_statusLabel->setText(text);
}

void MainWindow::onProcessingError(QString message) {
    m_startBtn->setText("▶  Start Processing");
    m_startBtn->setStyleSheet(
        "QPushButton { background: #2a6a3a; border-radius:6px; color:#cfe; font-weight:bold; font-size:10pt;}"
        "QPushButton:hover{background:#3a8a4a;} QPushButton:pressed{background:#1a5a2a;}"
        "QPushButton:disabled{background:#2a3a2a;color:#667;}");
    m_statusLabel->setText("❌ Error: " + message);
    m_progressBar->setValue(0);
}

// ---------------------------------------------------------------------------
// Image display helpers
// ---------------------------------------------------------------------------
void MainWindow::showImage(const cv::Mat& image) {
    if (image.empty()) return;
    cv::Mat rgb;
    if (image.channels() == 1) {
        cv::cvtColor(image, rgb, cv::COLOR_GRAY2RGB);
    } else if (image.channels() == 3) {
        cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
    } else if (image.channels() == 4) {
        cv::cvtColor(image, rgb, cv::COLOR_BGRA2RGB);
    } else {
        rgb = image.clone();
    }
    
    QImage qi(rgb.data, rgb.cols, rgb.rows, (int)rgb.step, QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(qi.copy());
    m_canvas->setPixmap(pix.scaled(m_canvas->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

cv::Mat MainWindow::getPreviewImage() const {
    if (m_image.empty()) return {};
    if (!m_fastPreview->isChecked()) return m_image;

    // Downsample to chosen resolution
    static const QMap<QString,int> resMap = {
        {"720p", 1280}, {"1080p", 1920}, {"1440p", 2560}, {"2160p", 3840}, {"Native", 0}
    };
    int targetW = resMap.value(m_resCombo->currentText(), 1920);
    if (targetW == 0) return m_image;

    int maxDim = std::max(m_image.cols, m_image.rows);
    if (maxDim <= targetW) return m_image;

    double scale = (double)targetW / maxDim;
    int nw = std::max(1, (int)(m_image.cols * scale));
    int nh = std::max(1, (int)(m_image.rows * scale));
    cv::Mat small;
    cv::resize(m_image, small, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    return small;
}

void MainWindow::renderAndShowGeometries(const QVector<DrawingGeometry>& geoms) {
    if (m_image.empty() && geoms.isEmpty()) return;

    // Determine canvas size
    int W = m_canvas->width();
    int H = m_canvas->height();
    if (W < 10 || H < 10) { W = 800; H = 600; }

    // Use preview image dimensions for bounds
    cv::Mat preview = getPreviewImage();
    int imgW = preview.empty() ? W : preview.cols;
    int imgH = preview.empty() ? H : preview.rows;

    // Scale to fit canvas
    double scale = std::min((double)W / imgW, (double)H / imgH);
    int dispW = (int)(imgW * scale), dispH = (int)(imgH * scale);

    // Create white QImage
    QImage img(dispW, dispH, QImage::Format_RGB32);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QString sepMode = m_separationCombo->currentText();
    QVector<QColor> palette = paletteForCurrentMode(false);
    if (sepMode == "CMYK") {
        img.fill(Qt::white); // CMYK is usually on white paper
    } else if (sepMode == "Colour Match") {
        img.fill(Qt::white); // RGB drawn on white paper as density
    } else {
        img.fill(QColor(30, 30, 35)); // Default dark theme for single color
    }

    for (auto& geom : geoms) {
        const auto& path = geom.path;
        if (path.size() < 2) continue;

        int pIdx = geom.penIndex;
        if (pIdx < 0 || pIdx >= palette.size()) pIdx = 0;
        
        QColor penColor = palette[pIdx];
        if (sepMode != "None") {
            penColor.setAlpha(180); // Slight transparency for CMYK blending
            painter.setCompositionMode(QPainter::CompositionMode_Multiply);
        } else {
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }

        painter.setPen(QPen(penColor, std::max(0.25, m_penWidthMm * 2.0)));

        for (std::size_t i = 1; i < path.size(); ++i) {
            painter.drawLine(
                QPointF(path[i-1].first * scale, path[i-1].second * scale),
                QPointF(path[i].first  * scale, path[i].second  * scale));
        }
    }
    painter.end();

    QPixmap pix = QPixmap::fromImage(img);
    m_canvas->setPixmap(pix);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    // Re-render preview on resize if we have geometries
    if (!m_lastGeoms.isEmpty())
        QTimer::singleShot(50, this, [this]{ renderAndShowGeometries(m_lastGeoms); });
    else if (!m_image.empty())
        QTimer::singleShot(50, this, [this]{ updatePreview(); });
}

// ---------------------------------------------------------------------------
// Image Preview helpers
// ---------------------------------------------------------------------------
void MainWindow::updatePreview() {
    if (m_image.empty()) return;
    cv::Mat filtered = m_image.clone();
    for (auto& f : m_activeFilters) {
        filtered = f->process(filtered);
    }
    filtered = m_maskMgr.applyMask(filtered);
    showImage(filtered);
}

void MainWindow::onExportSVG() {
    if (m_lastGeoms.isEmpty()) {
        QMessageBox::warning(this, "Export SVG", "No generated paths to export! Please run processing first.");
        return;
    }
    
    QString filepath = QFileDialog::getSaveFileName(this, "Export SVG", "", "SVG Files (*.svg)");
    if (filepath.isEmpty()) return;

    DrawingAreaConfig da;
    // For now we use the default DrawingAreaConfig properties.
    // Future work: add a UI panel for Drawing Area settings.
    
    QVector<QColor> palette = paletteForCurrentMode(true);

    cv::Mat preview = getPreviewImage();
    int imgW = preview.empty() ? m_image.cols : preview.cols;
    int imgH = preview.empty() ? m_image.rows : preview.rows;

    QString exportPath = filepath;
    QString tempPath;
    if (m_useVpype) {
        QTemporaryFile tmp(QDir::tempPath() + "/imgto2d_vpype_XXXXXX.svg");
        tmp.setAutoRemove(false);
        if (!tmp.open()) {
            QMessageBox::critical(this, "Export SVG", "Could not create temporary SVG for vpype.");
            return;
        }
        tempPath = tmp.fileName();
        tmp.close();
        exportPath = tempPath;
    }
    if (SVGExporter::exportSVG(exportPath, m_lastGeoms, da, imgW, imgH, palette, m_penWidthMm)) {
        if (m_useVpype) {
            QString err;
            if (!runVpypeEmbedded(tempPath, m_vpypePipeline, filepath, err)) {
                QFile::remove(tempPath);
                QMessageBox::critical(this, "Export SVG", "VPype optimization failed:\n" + err);
                return;
            }
            QFile::remove(tempPath);
        }
        QMessageBox::information(this, "Export SVG", "Successfully exported SVG to:\n" + filepath);
    } else {
        if (!tempPath.isEmpty()) QFile::remove(tempPath);
        QMessageBox::critical(this, "Export SVG", "Failed to export SVG to:\n" + filepath);
    }
}

void MainWindow::onExportGCode() {
    if (m_lastGeoms.isEmpty()) {
        QMessageBox::warning(this, "Export G-Code", "No generated paths to export! Please run processing first.");
        return;
    }
    
    QString filepath = QFileDialog::getSaveFileName(this, "Export G-Code", "", "G-Code Files (*.gcode *.nc)");
    if (filepath.isEmpty()) return;

    DrawingAreaConfig da;
    cv::Mat preview = getPreviewImage();
    int imgW = preview.empty() ? m_image.cols : preview.cols;
    int imgH = preview.empty() ? m_image.rows : preview.rows;

    if (GCodeExporter::exportGCode(filepath, m_lastGeoms, da, imgW, imgH)) {
        QMessageBox::information(this, "Export G-Code", "Successfully exported G-Code to:\n" + filepath);
    } else {
        QMessageBox::critical(this, "Export G-Code", "Failed to export G-Code to:\n" + filepath);
    }
}

void MainWindow::onExportHPGL() {
    if (m_lastGeoms.isEmpty()) {
        QMessageBox::warning(this, "Export HPGL", "No generated paths to export! Please run processing first.");
        return;
    }
    
    QString filepath = QFileDialog::getSaveFileName(this, "Export HPGL", "", "HPGL Files (*.hpgl *.plt)");
    if (filepath.isEmpty()) return;

    DrawingAreaConfig da;
    cv::Mat preview = getPreviewImage();
    int imgW = preview.empty() ? m_image.cols : preview.cols;
    int imgH = preview.empty() ? m_image.rows : preview.rows;

    if (HPGLExporter::exportHPGL(filepath, m_lastGeoms, da, imgW, imgH)) {
        QMessageBox::information(this, "Export HPGL", "Successfully exported HPGL to:\n" + filepath);
    } else {
        QMessageBox::critical(this, "Export HPGL", "Failed to export HPGL to:\n" + filepath);
    }
}

void MainWindow::onExportPDF() {
    if (m_lastGeoms.isEmpty()) {
        QMessageBox::warning(this, "Export PDF", "No generated paths to export! Please run processing first.");
        return;
    }
    
    QString filepath = QFileDialog::getSaveFileName(this, "Export PDF", "", "PDF Files (*.pdf)");
    if (filepath.isEmpty()) return;

    DrawingAreaConfig da;
    
    QVector<QColor> palette = paletteForCurrentMode(true);

    cv::Mat preview = getPreviewImage();
    int imgW = preview.empty() ? m_image.cols : preview.cols;
    int imgH = preview.empty() ? m_image.rows : preview.rows;

    if (PDFExporter::exportPDF(filepath, m_lastGeoms, da, imgW, imgH, palette, m_penWidthMm)) {
        QMessageBox::information(this, "Export PDF", "Successfully exported PDF to:\n" + filepath);
    } else {
        QMessageBox::critical(this, "Export PDF", "Failed to export PDF to:\n" + filepath);
    }
}

void MainWindow::onRunBatch() {
    QString inputDir = QFileDialog::getExistingDirectory(this, "Select Input Image Directory");
    if (inputDir.isEmpty()) return;

    QString outputDir = QFileDialog::getExistingDirectory(this, "Select Output Directory");
    if (outputDir.isEmpty()) return;

    auto pfm = m_pfms.value(m_pfmCombo->currentIndex());
    DrawingAreaConfig da;

    BatchProcessor::runBatch(inputDir, outputDir, m_activeFilters, pfm, m_maskMgr, da, this);
}

#ifdef HAS_SERIAL_PORT
void MainWindow::onStreamGCode() {
    if (m_lastGeoms.isEmpty()) {
        QMessageBox::warning(this, "Hardware Stream", "No paths to stream! Please run processing first.");
        return;
    }

    QString tempPath = QDir::tempPath() + "/imgto2d_stream.gcode";
    DrawingAreaConfig da;
    cv::Mat preview = getPreviewImage();
    int imgW = preview.empty() ? m_image.cols : preview.cols;
    int imgH = preview.empty() ? m_image.rows : preview.rows;
    
    if (GCodeExporter::exportGCode(tempPath, m_lastGeoms, da, imgW, imgH)) {
        QFile f(tempPath);
        if (f.open(QIODevice::ReadOnly)) {
            QString gcodeData = QString::fromUtf8(f.readAll());
            f.close();
            
            SerialDialog dlg(gcodeData, this);
            dlg.exec();
        }
    }
}
#endif

void MainWindow::onSettingChanged() {
    applySettingsToActiveObject();
    
    // Only update preview if we are editing a filter, because PFM changes don't affect the input image.
    if (m_currentEditingFilterIdx >= 0) {
        updatePreview();
    }
}

std::shared_ptr<ImageFilter> MainWindow::createFilterByName(const QString& name) const {
    if (name == "Brightness") return std::make_shared<BrightnessFilter>();
    if (name == "Contrast") return std::make_shared<ContrastFilter>();
    if (name == "Invert") return std::make_shared<InvertFilter>();
    if (name == "Threshold") return std::make_shared<ThresholdFilter>();
    if (name == "Unsharp Mask") return std::make_shared<UnsharpMaskFilter>();
    if (name == "Grayscale") return std::make_shared<GrayscaleFilter>();
    if (name == "Desaturate") return std::make_shared<DesaturateFilter>();
    if (name == "Saturation") return std::make_shared<SaturationFilter>();
    if (name == "Hue Shift") return std::make_shared<HueFilter>();
    if (name == "Gamma Correction") return std::make_shared<GammaFilter>();
    if (name == "Exposure") return std::make_shared<ExposureFilter>();
    if (name == "Sepia") return std::make_shared<SepiaFilter>();
    if (name == "CLAHE") return std::make_shared<CLAHEFilter>();
    if (name == "Temperature") return std::make_shared<TemperatureFilter>();
    if (name == "Canny Edge Detection") return std::make_shared<CannyFilter>();
    if (name == "Sobel Edge Detection") return std::make_shared<SobelFilter>();
    if (name == "Laplacian Edge Detection") return std::make_shared<LaplacianFilter>();
    if (name == "Prewitt Edge") return std::make_shared<PrewittFilter>();
    if (name == "Scharr Edge") return std::make_shared<ScharrFilter>();
    if (name == "Difference of Gaussians (DoG)") return std::make_shared<DoGFilter>();
    if (name == "Ridge Detection (Hessian)") return std::make_shared<RidgeDetectionFilter>();
    if (name == "High Pass") return std::make_shared<HighPassFilter>();
    if (name == "Gaussian Blur") return std::make_shared<GaussianBlurFilter>();
    if (name == "Median Blur") return std::make_shared<MedianBlurFilter>();
    if (name == "Box Blur") return std::make_shared<BoxBlurFilter>();
    if (name == "Motion Blur") return std::make_shared<MotionBlurFilter>();
    if (name == "Bilateral Filter") return std::make_shared<BilateralFilter>();
    if (name == "Low Pass") return std::make_shared<LowPassFilter>();
    if (name == "Sharpen More") return std::make_shared<SharpenMoreFilter>();
    if (name == "Add Gaussian Noise") return std::make_shared<GaussianNoiseFilter>();
    if (name == "Salt & Pepper Noise") return std::make_shared<SaltAndPepperFilter>();
    if (name == "Denoise (NL Means)" || name == "Denoise (Non-Local Means)") return std::make_shared<DenoiseFilter>();
    if (name == "Gaussian Noise 2") return std::make_shared<GaussianNoise2Filter>();
    if (name == "Speckle Noise") return std::make_shared<SpeckleNoiseFilter>();
    if (name == "Dilation") return std::make_shared<DilationFilter>();
    if (name == "Erosion") return std::make_shared<ErosionFilter>();
    if (name == "Opening") return std::make_shared<OpeningFilter>();
    if (name == "Closing") return std::make_shared<ClosingFilter>();
    if (name == "Morphological Gradient") return std::make_shared<MorphGradientFilter>();
    if (name == "Top Hat") return std::make_shared<TopHatFilter>();
    if (name == "Black Hat") return std::make_shared<BlackHatFilter>();
    if (name == "Dilate Cross") return std::make_shared<DilateCrossFilter>();
    if (name == "Erode Cross") return std::make_shared<ErodeCrossFilter>();
    if (name == "Dilate Ellipse") return std::make_shared<DilateEllipseFilter>();
    if (name == "Erode Ellipse") return std::make_shared<ErodeEllipseFilter>();
    if (name == "Posterize") return std::make_shared<PosterizeFilter>();
    if (name == "Edge Preserve Smooth") return std::make_shared<EdgePreserveFilter>();
    if (name == "Stylization") return std::make_shared<StylizationFilter>();
    if (name == "Oil Painting") return std::make_shared<OilPaintingFilter>();
    if (name == "Detail Enhance") return std::make_shared<DetailEnhanceFilter>();
    if (name == "Pencil Sketch") return std::make_shared<PencilSketchFilter>();
    if (name == "Emboss") return std::make_shared<EmbossFilter>();
    if (name == "Quantize (8 Colors)") return std::make_shared<QuantizeFilter>();
    if (name == "Vignette") return std::make_shared<VignetteFilter>();
    if (name == "Pixelate") return std::make_shared<PixelateFilter>();
    if (name == "Wave Distortion") return std::make_shared<WaveFilter>();
    if (name == "Otsu Threshold") return std::make_shared<OtsuThresholdFilter>();
    if (name == "Adaptive Threshold") return std::make_shared<AdaptiveThresholdFilter>();
    if (name == "Truncate Threshold") return std::make_shared<TruncateThresholdFilter>();
    if (name == "To Zero Threshold") return std::make_shared<ToZeroThresholdFilter>();
    if (name == "Equalize Histogram") return std::make_shared<EqualizeHistFilter>();
    if (name == "Auto Contrast") return std::make_shared<AutoContrastFilter>();
    if (name == "Auto Color") return std::make_shared<AutoColorFilter>();
    if (name == "Colorize (Tint)") return std::make_shared<ColorizeFilter>();
    if (name == "Invert Hue") return std::make_shared<InvertHueFilter>();
    return {};
}

// ---------------------------------------------------------------------------
// Filter Slots
// ---------------------------------------------------------------------------
void MainWindow::onAddFilter() {
    int idx = m_filterCombo->currentIndex();
    if (idx < 0 || idx >= m_availableFilters.size()) return;
    
    QString name = m_availableFilters[idx]->name();
    std::shared_ptr<ImageFilter> newFilter = createFilterByName(name);
    
    if (newFilter) {
        m_activeFilters.append(newFilter);
        m_filterList->addItem(name);
        m_filterList->setCurrentRow(m_activeFilters.size() - 1);
        updatePreview();
    }
}

void MainWindow::onRemoveFilter() {
    int row = m_filterList->currentRow();
    if (row < 0 || row >= m_activeFilters.size()) return;
    m_activeFilters.removeAt(row);
    delete m_filterList->takeItem(row);
    
    if (m_activeFilters.isEmpty()) {
        m_pfmCombo->setCurrentIndex(m_pfmCombo->currentIndex()); // trigger update
        onPFMSelectionChanged(m_pfmCombo->currentIndex());
    }
    updatePreview();
}

void MainWindow::onMoveFilterUp() {
    int row = m_filterList->currentRow();
    if (row <= 0 || row >= m_activeFilters.size()) return;
    
    m_activeFilters.swapItemsAt(row, row - 1);
    auto item = m_filterList->takeItem(row);
    m_filterList->insertItem(row - 1, item);
    m_filterList->setCurrentRow(row - 1);
    updatePreview();
}

void MainWindow::onMoveFilterDown() {
    int row = m_filterList->currentRow();
    if (row < 0 || row >= m_activeFilters.size() - 1) return;
    
    m_activeFilters.swapItemsAt(row, row + 1);
    auto item = m_filterList->takeItem(row);
    m_filterList->insertItem(row + 1, item);
    m_filterList->setCurrentRow(row + 1);
    updatePreview();
}

void MainWindow::onFilterListSelectionChanged() {
    int row = m_filterList->currentRow();
    if (row >= 0 && row < m_activeFilters.size()) {
        applySettingsToActiveObject(); // save current
        m_currentEditingFilterIdx = row;
        populateSettingsPanel();
    }
}

