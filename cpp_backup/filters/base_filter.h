#pragma once
/**
 * base_filter.h — Base class for all image pre-processing filters.
 * Port of app/filters/base_filter.py
 */

#include <QString>
#include <QVariant>
#include <QObject>
#include <QVector>
#include <QMap>
#include <opencv2/core.hpp>

#include "pfm/pfm_base.h" // For PFMSetting and SettingType

class ImageFilter : public QObject {
    Q_OBJECT
public:
    explicit ImageFilter(QObject* parent = nullptr);
    virtual ~ImageFilter() = default;

    virtual QString name() const = 0;
    virtual QString category() const { return "General"; }
    virtual QString description() const { return ""; }

    QVector<PFMSetting> settingsList() const;
    const QMap<QString, PFMSetting>& settingsMap() const { return m_settings; }
    
    QVariant get(const QString& key) const;
    void set(const QString& key, const QVariant& value);
    void resetAll();

    // The main processing function. Takes an image, returns a filtered image.
    virtual cv::Mat process(const cv::Mat& image) = 0;

protected:
    virtual QVector<PFMSetting> defineSettings() const { return {}; }
    
    void initSettings();
    QMap<QString, PFMSetting> m_settings;
};
