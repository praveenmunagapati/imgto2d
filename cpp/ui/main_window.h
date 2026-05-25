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
#include <QListWidget>
#include <opencv2/core.hpp>

#include "pfm/pfm_base.h"
#include "filters/base_filter.h"
#include "ui/pfm_worker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    QComboBox*    m_pfmCombo    = nullptr;

private slots:
    void onLoadImage();
    void onStartProcessing();
    void onReset();
    void onPFMSelectionChanged(int idx);
    void onProcessingFinished(QVector<DrawingGeometry> geometries);
    void onProgressUpdate(float pct, int shapes, QString text);
    void onProcessingError(QString message);
    
    // Filters
    void onAddFilter();
    void onRemoveFilter();
    void onMoveFilterUp();
    void onMoveFilterDown();
    void onFilterListSelectionChanged();
    void onSettingChanged();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void buildUI();
    void showImage(const cv::Mat& gray);
    void renderAndShowGeometries(const QVector<DrawingGeometry>& geoms);
    void populateSettingsPanel();
    void applySettingsToActiveObject();
    void updatePreview();
    cv::Mat getPreviewImage() const;

    // ---- UI elements ----
    QWidget*      m_leftPanel   = nullptr;
    QPushButton*  m_loadBtn     = nullptr;
    QPushButton*  m_startBtn    = nullptr;
    QCheckBox*    m_fastPreview = nullptr;
    QComboBox*    m_resCombo    = nullptr;
    QPushButton*  m_resetBtn    = nullptr;
    QLabel*       m_statusLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    
    // ---- Filter UI ----
    QComboBox*    m_filterCombo = nullptr;
    QPushButton*  m_addFilterBtn = nullptr;
    QListWidget*  m_filterList   = nullptr;
    QPushButton*  m_filterUpBtn  = nullptr;
    QPushButton*  m_filterDownBtn= nullptr;
    QPushButton*  m_filterDelBtn = nullptr;

    QScrollArea*  m_settingsScroll = nullptr;
    QWidget*      m_settingsWidget = nullptr;
    QFormLayout*  m_settingsForm   = nullptr;

    QLabel*       m_canvas      = nullptr;

    // ---- State ----
    cv::Mat                          m_image;      // loaded grayscale
    QVector<std::shared_ptr<PathFindingModule>> m_pfms;
    
    // Filter definitions and active stack
    QVector<std::shared_ptr<ImageFilter>> m_availableFilters;
    QVector<std::shared_ptr<ImageFilter>> m_activeFilters;
    
    PFMWorker*                       m_worker     = nullptr;
    QVector<DrawingGeometry>         m_lastGeoms;

    // Setting widgets keyed by setting index in current PFM or Filter
    QMap<QString, QWidget*> m_settingWidgets;
    int m_currentEditingFilterIdx = -1; // -1 means editing PFM
};
