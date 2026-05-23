/**
 * pfm_base.cpp — Implementation of PathFindingModule base class.
 */

#include "pfm/pfm_base.h"
#include <numeric>
#include <random>

PathFindingModule::PathFindingModule(QObject* parent)
    : QObject(parent), m_rng(42), m_cancelled(0)
{
    // defineSettings() is pure virtual — call from subclass ctors after construction
    // We cannot call it here safely; subclasses must call initSettings() themselves.
}

void PathFindingModule::initSettings() {
    m_settings.clear();
    for (auto& s : defineSettings())
        m_settings.insert(s.key, s);
}

QVector<PFMSetting> PathFindingModule::settingsList() const {
    return QVector<PFMSetting>(m_settings.values().begin(), m_settings.values().end());
}

QVariant PathFindingModule::get(const QString& key) const {
    auto it = m_settings.find(key);
    if (it == m_settings.end()) return {};
    return it->currentValue();
}

void PathFindingModule::set(const QString& key, const QVariant& value) {
    auto it = m_settings.find(key);
    if (it != m_settings.end()) it->value = value;
}

void PathFindingModule::resetAll() {
    for (auto& s : m_settings) s.value = QVariant();
}

QVector<DrawingGeometry> PathFindingModule::process(const cv::Mat& grayImage) {
    m_cancelled.storeRelaxed(0);

    // Apply random seed
    QVariant seedVar = get("random_seed");
    quint32 seed = seedVar.isNull() ? 42u : (quint32)seedVar.toInt();
    m_rng.seed(seed);

    emitProgress(0.0f, 0, QString("Starting %1...").arg(name()));
    auto result = _process(grayImage);
    emitProgress(1.0f, result.size(), QString("%1 complete").arg(name()));
    return result;
}

void PathFindingModule::emitProgress(float pct, int shapes, const QString& text) {
    emit progressUpdate(std::clamp(pct, 0.0f, 1.0f), shapes, text);
}

double PathFindingModule::randUniform(double lo, double hi) {
    std::uniform_real_distribution<double> dist(lo, hi);
    return dist(m_rng);
}

double PathFindingModule::randGauss(double mean, double sigma) {
    std::normal_distribution<double> dist(mean, sigma);
    return dist(m_rng);
}

int PathFindingModule::randInt(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(m_rng);
}

int PathFindingModule::weightedChoice(const std::vector<double>& weights) {
    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    return dist(m_rng);
}
