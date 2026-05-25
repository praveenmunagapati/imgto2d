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

// PFM includes — all 10
#include "pfm/sketch_lines.h"
#include "pfm/sketch_squares.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_shapes.h"
#include "pfm/sketch_waves.h"
#include "pfm/sketch_spirals.h"
#include "pfm/hatch_lines.h"
#include "pfm/stipple_dots.h"
#include "pfm/adaptive_pfm.h"
#include "pfm/sketch_extras.h"
#include "pfm/maze_tsp_pfms.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("imgto2d — Qt Fast Preview");
    resize(1200, 800);

    // Register all PFMs
    m_pfms.append(std::make_shared<SketchLinesPFM>());
    m_pfms.append(std::make_shared<SketchSquaresPFM>());
    m_pfms.append(std::make_shared<SketchCurvesPFM>());
    m_pfms.append(std::make_shared<SketchShapesPFM>());
    m_pfms.append(std::make_shared<SketchQuadBeziersPFM>());
    m_pfms.append(std::make_shared<SketchWavesPFM>());
    m_pfms.append(std::make_shared<SketchSpiralsPFM>());
    m_pfms.append(std::make_shared<HatchLinesPFM>());
    m_pfms.append(std::make_shared<StippleDotsPFM>());
    m_pfms.append(std::make_shared<AdaptiveStipplingPFM>());
    
    // Extras
    m_pfms.append(std::make_shared<SketchSuperformulaPFM>());
    m_pfms.append(std::make_shared<SketchCubicBeziers2PFM>());
    m_pfms.append(std::make_shared<SketchSweepingCurvesPFM>());
    m_pfms.append(std::make_shared<SketchFlowFieldsPFM>());
    
    // Phase 2: Triangulation and Voronoi
    m_pfms.append(std::make_shared<SketchVoronoiPFM>());
    m_pfms.append(std::make_shared<SketchDelaunayPFM>());
    
    // Phase 3: Graph and TSP
    m_pfms.append(std::make_shared<TSPClassicPFM>());
    m_pfms.append(std::make_shared<TSPOutlinePFM>());
    m_pfms.append(std::make_shared<TSPShadingPFM>());
    m_pfms.append(std::make_shared<TSPMSTPFM>());

    buildUI();
    populateSettingsPanel(0);
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

    // ---- PFM selector ----
    auto* pfmGroup = new QGroupBox("Path Finding Module");
    pfmGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #aac; border: 1px solid #444; border-radius:4px; margin-top:6px; }"
                            "QGroupBox::title { subcontrol-origin: margin; left: 8px; }");
    auto* pfmVBox = new QVBoxLayout(pfmGroup);
    m_pfmCombo = new QComboBox;
    for (auto& pfm : m_pfms)
        m_pfmCombo->addItem(pfm->name());
    m_pfmCombo->setStyleSheet("QComboBox { background: #2a2a36; border:1px solid #555; border-radius:4px; padding:3px; }");
    connect(m_pfmCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onPFMSelectionChanged);
    pfmVBox->addWidget(m_pfmCombo);
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
void MainWindow::populateSettingsPanel(int pfmIndex) {
    qDebug() << "populateSettingsPanel START. index=" << pfmIndex;
    // Clear old widgets
    m_settingWidgets.clear();
    QLayoutItem* item;
    while ((item = m_settingsForm->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    qDebug() << "populateSettingsPanel: cleared old widgets";

    if (pfmIndex < 0 || pfmIndex >= m_pfms.size()) {
        qDebug() << "populateSettingsPanel: index out of bounds. size=" << m_pfms.size();
        return;
    }
    auto& pfm = m_pfms[pfmIndex];
    qDebug() << "populateSettingsPanel: got PFM:" << pfm->name();

    QString lastCategory;
    for (auto& s : pfm->settingsList()) {
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
            widget = cb;
            break;
        }
        case SettingType::Integer: {
            auto* spin = new QSpinBox;
            spin->setRange((int)s.minVal, (int)s.maxVal);
            spin->setValue(s.toInt());
            spin->setStyleSheet("QSpinBox { background: #2a2a36; border:1px solid #555; border-radius:3px; color:#ddd; padding:1px; }");
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

void MainWindow::applySettingsToPFM(int pfmIndex) {
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

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------
void MainWindow::onPFMSelectionChanged(int idx) {
    populateSettingsPanel(idx);
}

void MainWindow::onLoadImage() {
    QString path = QFileDialog::getOpenFileName(this, "Load Image", {},
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tif *.tiff *.webp)");
    if (path.isEmpty()) return;

    cv::Mat img = cv::imread(path.toStdString(), cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        m_statusLabel->setText("❌ Failed to load image");
        return;
    }
    m_image = img;
    showImage(img);
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

    qDebug() << "onStartProcessing: getting current PFM index:" << m_pfmCombo->currentIndex();
    int idx = m_pfmCombo->currentIndex();
    if (idx < 0 || idx >= m_pfms.size()) { m_statusLabel->setText("⚠ Select a PFM"); return; }

    applySettingsToPFM(idx);

    cv::Mat imgForPFM = getPreviewImage();
    m_startBtn->setText("⏹  Cancel");
    m_startBtn->setStyleSheet(
        "QPushButton { background: #6a2a2a; border-radius:6px; color:#fcc; font-weight:bold; font-size:10pt;}"
        "QPushButton:hover{background:#8a3a3a;} QPushButton:pressed{background:#5a1a1a;}");
    m_statusLabel->setText(QString("Processing with %1...").arg(m_pfms[idx]->name()));
    m_progressBar->setValue(0);

    qDebug() << "onStartProcessing: applying UI settings to PFM:" << m_pfms[idx]->name();
    m_worker = new PFMWorker(m_pfms[idx].get(), imgForPFM, this);
    connect(m_worker, &PFMWorker::finished,       this, &MainWindow::onProcessingFinished);
    connect(m_worker, &PFMWorker::progressUpdate, this, &MainWindow::onProgressUpdate);
    connect(m_worker, &PFMWorker::errorOccurred,  this, &MainWindow::onProcessingError);
    connect(m_worker, &QThread::finished, m_worker, &QObject::deleteLater);

    qDebug() << "onStartProcessing: starting worker thread";
    m_worker->start();
    qDebug() << "onStartProcessing END";
}

void MainWindow::onReset() {
    if (m_worker && m_worker->isRunning()) {
        auto pfm = m_pfms.value(m_pfmCombo->currentIndex());
        if (pfm) pfm->cancel();
    }
    m_image = cv::Mat();
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
void MainWindow::showImage(const cv::Mat& gray) {
    cv::Mat rgb;
    cv::cvtColor(gray, rgb, cv::COLOR_GRAY2RGB);
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
    img.fill(QColor(30, 30, 35));

    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(200, 220, 255), 0.8));

    for (auto& geom : geoms) {
        const auto& path = geom.path;
        if (path.size() < 2) continue;
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
        QTimer::singleShot(50, this, [this]{ showImage(m_image); });
}
