#pragma once
#include <QVector>
#include <memory>
#include <QString>

class PathFindingModule;
class ImageFilter;

class ModuleRegistry {
public:
    static QVector<std::shared_ptr<PathFindingModule>> createAllPFMs();
    static QVector<std::shared_ptr<ImageFilter>> createAllFilters();
};
