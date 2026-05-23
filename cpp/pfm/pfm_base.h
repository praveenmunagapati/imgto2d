#pragma once
/**
 * pfm_base.h — Abstract base class for all Path Finding Modules (PFMs).
 * C++ port of app/pfm/__init__.py (PathFindingModule, PFMSetting, DrawingGeometry).
 */

#include <QString>
#include <QVariant>
#include <QObject>
#include <QVector>
#include <QMap>
#include <QAtomicInt>
#include <QStringList>

#include <random>
#include <functional>

#include <opencv2/core.hpp>

#include "core/geometry.h"

// ---------------------------------------------------------------------------
// Setting types
// ---------------------------------------------------------------------------
enum class SettingType {
    Number,
    Integer,
    Percentage,
    Boolean,
    Enum,
    Text,
};

struct PFMSetting {
    QString     key;
    QString     label;
    SettingType type        = SettingType::Number;
    QVariant    defaultVal;
    QVariant    value;          // null = use default
    double      minVal      = 0.0;
    double      maxVal      = 100.0;
    double      safeMin     = 0.0;
    double      safeMax     = 100.0;
    double      step        = 1.0;
    QStringList options;        // for Enum type
    QString     category    = "Default";
    QString     tooltip;
    bool        randomiseExclude = false;

    QVariant currentValue() const {
        return value.isNull() ? defaultVal : value;
    }

    double toDouble() const { return currentValue().toDouble(); }
    int    toInt()    const { return currentValue().toInt(); }
    bool   toBool()   const { return currentValue().toBool(); }
};

// ---------------------------------------------------------------------------
// Output geometry
// ---------------------------------------------------------------------------
struct DrawingGeometry {
    Path path;
    int  penIndex   = 0;
    int  groupIndex = 0;
};

// ---------------------------------------------------------------------------
// Abstract base class
// ---------------------------------------------------------------------------
class PathFindingModule : public QObject {
    Q_OBJECT
public:
    explicit PathFindingModule(QObject* parent = nullptr);
    virtual ~PathFindingModule() = default;

    // Identity
    virtual QString name()        const = 0;
    virtual QString description() const { return {}; }
    virtual QString category()    const { return "General"; }
    virtual bool    isPremium()   const { return false; }

    // Settings
    QVector<PFMSetting>       settingsList() const;
    const QMap<QString, PFMSetting>& settingsMap() const { return m_settings; }
    QVariant get(const QString& key) const;
    void     set(const QString& key, const QVariant& value);
    void     resetAll();

    // Processing
    QVector<DrawingGeometry> process(const cv::Mat& grayImage);
    void cancel() { m_cancelled.storeRelaxed(1); }
    bool isCancelled() const { return m_cancelled.loadRelaxed() != 0; }

    // RNG helpers
    double  randUniform(double lo, double hi);
    double  randGauss(double mean, double sigma);
    int     randInt(int lo, int hi);  // inclusive
    int     weightedChoice(const std::vector<double>& weights);

    // Public helper so free-function algorithm helpers can call it
    void emitProgress(float pct, int shapes, const QString& text);

signals:
    void progressUpdate(float pct, int shapes, QString text);

protected:
    virtual QVector<PFMSetting>    defineSettings() const = 0;
    virtual QVector<DrawingGeometry> _process(const cv::Mat& image) = 0;


    QMap<QString, PFMSetting> m_settings;
    std::mt19937              m_rng;
    QAtomicInt                m_cancelled;

    // Must be called at end of each subclass constructor
    void initSettings();
};
