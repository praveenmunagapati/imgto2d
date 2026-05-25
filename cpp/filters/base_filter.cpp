#include "filters/base_filter.h"

ImageFilter::ImageFilter(QObject* parent) : QObject(parent) {
    // Note: Subclasses must call initSettings() inside their constructors!
}

void ImageFilter::initSettings() {
    m_settings.clear();
    auto sList = defineSettings();
    for (const auto& s : sList) {
        m_settings.insert(s.key, s);
    }
}

QVector<PFMSetting> ImageFilter::settingsList() const {
    QVector<PFMSetting> list;
    // We fetch them dynamically to ensure any GUI updates to m_settings are reflected
    auto defined = defineSettings();
    for (const auto& s : defined) {
        if (m_settings.contains(s.key)) {
            list.append(m_settings[s.key]);
        } else {
            list.append(s);
        }
    }
    return list;
}

QVariant ImageFilter::get(const QString& key) const {
    if (m_settings.contains(key)) {
        return m_settings[key].currentValue();
    }
    return QVariant();
}

void ImageFilter::set(const QString& key, const QVariant& value) {
    if (m_settings.contains(key)) {
        m_settings[key].value = value;
    }
}

void ImageFilter::resetAll() {
    for (auto it = m_settings.begin(); it != m_settings.end(); ++it) {
        it->value = QVariant(); // clear value so currentValue() falls back to defaultVal
    }
}
