#pragma once
/**
 * main_window.h — Qt MainWindow replacing TkApp from tk_gui_fast.py.
 */

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QScrollArea>
#include <QFormLayout>
#include <QProgressBar>
#include <QVector>
#include <QPixmap>
#include <memory>

#include <opencv2/core.hpp>

#include "pfm/pfm_base.h"
#include "ui/pfm_worker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onLoadImage();
    void onStartProcessing();
    void onReset();
    void onPFMSelectionChanged(int idx);
    void onProcessingFinished(QVector<DrawingGeometry> geometries);
    void onProgressUpdate(float pct, int shapes, QString text);
    void onProcessingError(QString message);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void buildUI();
    void showImage(const cv::Mat& gray);
    void renderAndShowGeometries(const QVector<DrawingGeometry>& geoms);
    void populateSettingsPanel(int pfmIndex);
    void applySettingsToPFM(int pfmIndex);
    cv::Mat getPreviewImage() const;

    // ---- UI elements ----
    QWidget*      m_leftPanel   = nullptr;
    QPushButton*  m_loadBtn     = nullptr;
    QComboBox*    m_pfmCombo    = nullptr;
    QPushButton*  m_startBtn    = nullptr;
    QCheckBox*    m_fastPreview = nullptr;
    QComboBox*    m_resCombo    = nullptr;
    QPushButton*  m_resetBtn    = nullptr;
    QLabel*       m_statusLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QScrollArea*  m_settingsScroll = nullptr;
    QWidget*      m_settingsWidget = nullptr;
    QFormLayout*  m_settingsForm   = nullptr;

    QLabel*       m_canvas      = nullptr;

    // ---- State ----
    cv::Mat                          m_image;      // loaded grayscale
    QVector<std::shared_ptr<PathFindingModule>> m_pfms;
    PFMWorker*                       m_worker     = nullptr;
    QVector<DrawingGeometry>         m_lastGeoms;

    // Setting widgets keyed by setting index in current PFM
    QMap<QString, QWidget*> m_settingWidgets;
};
