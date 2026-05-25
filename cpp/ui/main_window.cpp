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
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QMessageBox>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <QFont>
#include <QStyleFactory>
#include <QFrame>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

// PFM includes — all families
#include "pfm/sketch_lines.h"
#include "pfm/sketch_squares.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_shapes.h"
#include "pfm/sketch_waves.h"
#include "pfm/sketch_spirals.h"
#include "pfm/sketch_extras.h"
#include "pfm/sketch_beziers.h"
#include "pfm/hatch_lines.h"
#include "pfm/hatch_extras.h"
#include "pfm/stipple_dots.h"
#include "pfm/stipple_extras.h"
#include "pfm/adaptive_pfm.h"
#include "pfm/adaptive_extras.h"
#include "pfm/voronoi_pfms.h"
#include "pfm/grid_pfms.h"
#include "pfm/mosaic_pfms.h"
#include "pfm/letters_pfms.h"
#include "pfm/spiral_dbv3.h"
#include "pfm/streamline_pfms.h"
#include "pfm/special_pfms.h"
#include "pfm/maze_tsp_pfms.h"
#include "pfm/composite_pfms.h"

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

    // --- Letters ---
    m_pfms.append(std::make_shared<LBGLettersPFM>());

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

    // ---- Load button ----
    m_loadBtn = new QPushButton("📂  Load Image");
    m_loadBtn->setFixedHeight(36);
    m_loadBtn->setStyleSheet(
        "QPushButton { background: #3a4a7a; border-radius: 6px; color: #dde; font-weight: bold; }"
        "QPushButton:hover { background: #4a5a9a; }"
        "QPushButton:pressed { background: #2a3a6a; }");
    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadImage);
    leftVBox->addWidget(m_loadBtn);

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
    updatePreview();
    QFileInfo fi(path);
    m_statusLabel->setText(QString("Loaded: %1 (%2×%3)")
                           .arg(fi.fileName()).arg(img.cols).arg(img.rows));
    m_progressBar->setValue(0);
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

    // Reset processing state
    m_lastGeoms.clear();
    m_progressBar->setValue(0);
    
    // Downsample if fast preview is requested
    cv::Mat processImg = filteredImage;
    if (m_fastPreview->isChecked() && (filteredImage.cols > 800 || filteredImage.rows > 800)) {
        float scale = 800.0f / std::max(filteredImage.cols, filteredImage.rows);
        cv::resize(filteredImage, processImg, cv::Size(), 0, 0, cv::INTER_AREA);
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
    QVector<QColor> palette;
    if (sepMode == "CMYK") {
        palette = {QColor(0, 255, 255), QColor(255, 0, 255), QColor(255, 255, 0), QColor(30, 30, 30)};
        img.fill(Qt::white); // CMYK is usually on white paper
    } else if (sepMode == "Colour Match") {
        palette = {QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255)};
        img.fill(Qt::white); // RGB drawn on white paper as density
    } else {
        palette = {QColor(200, 220, 255)};
        img.fill(QColor(30, 30, 35)); // Default dark theme for single color
    }

    // Since we fill here based on sepMode, do we need to override the img.fill above?
    // Yes, but img is already filled above. We'll just let the if block above set the correct background.

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

        painter.setPen(QPen(penColor, 0.8));

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
    showImage(filtered);
}

void MainWindow::onSettingChanged() {
    applySettingsToActiveObject();
    
    // Only update preview if we are editing a filter, because PFM changes don't affect the input image.
    if (m_currentEditingFilterIdx >= 0) {
        updatePreview();
    }
}

// ---------------------------------------------------------------------------
// Filter Slots
// ---------------------------------------------------------------------------
void MainWindow::onAddFilter() {
    int idx = m_filterCombo->currentIndex();
    if (idx < 0 || idx >= m_availableFilters.size()) return;
    
    QString name = m_availableFilters[idx]->name();
    std::shared_ptr<ImageFilter> newFilter;
    if (name == "Brightness") newFilter = std::make_shared<BrightnessFilter>();
    else if (name == "Contrast") newFilter = std::make_shared<ContrastFilter>();
    else if (name == "Invert") newFilter = std::make_shared<InvertFilter>();
    else if (name == "Threshold") newFilter = std::make_shared<ThresholdFilter>();
    else if (name == "Unsharp Mask") newFilter = std::make_shared<UnsharpMaskFilter>();
    else if (name == "Grayscale") newFilter = std::make_shared<GrayscaleFilter>();
    else if (name == "Saturation") newFilter = std::make_shared<SaturationFilter>();
    else if (name == "Hue Shift") newFilter = std::make_shared<HueFilter>();
    else if (name == "Gamma Correction") newFilter = std::make_shared<GammaFilter>();
    else if (name == "Exposure") newFilter = std::make_shared<ExposureFilter>();
    else if (name == "Sepia") newFilter = std::make_shared<SepiaFilter>();
    else if (name == "CLAHE") newFilter = std::make_shared<CLAHEFilter>();
    else if (name == "Temperature") newFilter = std::make_shared<TemperatureFilter>();
    else if (name == "Canny Edge Detection") newFilter = std::make_shared<CannyFilter>();
    else if (name == "Sobel Edge Detection") newFilter = std::make_shared<SobelFilter>();
    else if (name == "Laplacian Edge Detection") newFilter = std::make_shared<LaplacianFilter>();
    else if (name == "Prewitt Edge") newFilter = std::make_shared<PrewittFilter>();
    else if (name == "Scharr Edge") newFilter = std::make_shared<ScharrFilter>();
    else if (name == "Difference of Gaussians (DoG)") newFilter = std::make_shared<DoGFilter>();
    else if (name == "Ridge Detection (Hessian)") newFilter = std::make_shared<RidgeDetectionFilter>();
    else if (name == "High Pass") newFilter = std::make_shared<HighPassFilter>();
    else if (name == "Gaussian Blur") newFilter = std::make_shared<GaussianBlurFilter>();
    else if (name == "Median Blur") newFilter = std::make_shared<MedianBlurFilter>();
    else if (name == "Box Blur") newFilter = std::make_shared<BoxBlurFilter>();
    else if (name == "Motion Blur") newFilter = std::make_shared<MotionBlurFilter>();
    else if (name == "Bilateral Filter") newFilter = std::make_shared<BilateralFilter>();
    else if (name == "Low Pass") newFilter = std::make_shared<LowPassFilter>();
    else if (name == "Sharpen More") newFilter = std::make_shared<SharpenMoreFilter>();
    else if (name == "Add Gaussian Noise") newFilter = std::make_shared<GaussianNoiseFilter>();
    else if (name == "Salt & Pepper Noise") newFilter = std::make_shared<SaltAndPepperFilter>();
    else if (name == "Denoise (NL Means)") newFilter = std::make_shared<DenoiseFilter>();
    else if (name == "Gaussian Noise 2") newFilter = std::make_shared<GaussianNoise2Filter>();
    else if (name == "Speckle Noise") newFilter = std::make_shared<SpeckleNoiseFilter>();
    else if (name == "Dilation") newFilter = std::make_shared<DilationFilter>();
    else if (name == "Erosion") newFilter = std::make_shared<ErosionFilter>();
    else if (name == "Opening") newFilter = std::make_shared<OpeningFilter>();
    else if (name == "Closing") newFilter = std::make_shared<ClosingFilter>();
    else if (name == "Morphological Gradient") newFilter = std::make_shared<MorphGradientFilter>();
    else if (name == "Top Hat") newFilter = std::make_shared<TopHatFilter>();
    else if (name == "Black Hat") newFilter = std::make_shared<BlackHatFilter>();
    else if (name == "Dilate Cross") newFilter = std::make_shared<DilateCrossFilter>();
    else if (name == "Erode Cross") newFilter = std::make_shared<ErodeCrossFilter>();
    else if (name == "Dilate Ellipse") newFilter = std::make_shared<DilateEllipseFilter>();
    else if (name == "Erode Ellipse") newFilter = std::make_shared<ErodeEllipseFilter>();
    else if (name == "Posterize") newFilter = std::make_shared<PosterizeFilter>();
    else if (name == "Edge Preserve Smooth") newFilter = std::make_shared<EdgePreserveFilter>();
    else if (name == "Stylization") newFilter = std::make_shared<StylizationFilter>();
    else if (name == "Emboss") newFilter = std::make_shared<EmbossFilter>();
    else if (name == "Quantize (8 Colors)") newFilter = std::make_shared<QuantizeFilter>();
    else if (name == "Vignette") newFilter = std::make_shared<VignetteFilter>();
    else if (name == "Pixelate") newFilter = std::make_shared<PixelateFilter>();
    else if (name == "Wave Distortion") newFilter = std::make_shared<WaveFilter>();
    else if (name == "Otsu Threshold") newFilter = std::make_shared<OtsuThresholdFilter>();
    else if (name == "Adaptive Threshold") newFilter = std::make_shared<AdaptiveThresholdFilter>();
    else if (name == "Truncate Threshold") newFilter = std::make_shared<TruncateThresholdFilter>();
    else if (name == "To Zero Threshold") newFilter = std::make_shared<ToZeroThresholdFilter>();
    else if (name == "Equalize Histogram") newFilter = std::make_shared<EqualizeHistFilter>();
    else if (name == "Auto Contrast") newFilter = std::make_shared<AutoContrastFilter>();
    else if (name == "Auto Color") newFilter = std::make_shared<AutoColorFilter>();
    else if (name == "Colorize (Tint)") newFilter = std::make_shared<ColorizeFilter>();
    else if (name == "Invert Hue") newFilter = std::make_shared<InvertHueFilter>();



    
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

