/****************************************************************************
** Meta object code from reading C++ file 'color_filters.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../filters/color_filters.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'color_filters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_GrayscaleFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GrayscaleFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GrayscaleFilter_t qt_meta_stringdata_GrayscaleFilter = {
    {
        QT_MOC_LITERAL(0, 15)   // "GrayscaleFilter"
    },
    "GrayscaleFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GrayscaleFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject GrayscaleFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_GrayscaleFilter.offsetsAndSizes,
    qt_meta_data_GrayscaleFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GrayscaleFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GrayscaleFilter, std::true_type>
    >,
    nullptr
} };

void GrayscaleFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *GrayscaleFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GrayscaleFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GrayscaleFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int GrayscaleFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_DesaturateFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DesaturateFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DesaturateFilter_t qt_meta_stringdata_DesaturateFilter = {
    {
        QT_MOC_LITERAL(0, 16)   // "DesaturateFilter"
    },
    "DesaturateFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DesaturateFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject DesaturateFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_DesaturateFilter.offsetsAndSizes,
    qt_meta_data_DesaturateFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DesaturateFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DesaturateFilter, std::true_type>
    >,
    nullptr
} };

void DesaturateFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DesaturateFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DesaturateFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DesaturateFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int DesaturateFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_SaturationFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SaturationFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SaturationFilter_t qt_meta_stringdata_SaturationFilter = {
    {
        QT_MOC_LITERAL(0, 16)   // "SaturationFilter"
    },
    "SaturationFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SaturationFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject SaturationFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_SaturationFilter.offsetsAndSizes,
    qt_meta_data_SaturationFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SaturationFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SaturationFilter, std::true_type>
    >,
    nullptr
} };

void SaturationFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *SaturationFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SaturationFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SaturationFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int SaturationFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_HueFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_HueFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_HueFilter_t qt_meta_stringdata_HueFilter = {
    {
        QT_MOC_LITERAL(0, 9)   // "HueFilter"
    },
    "HueFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_HueFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject HueFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_HueFilter.offsetsAndSizes,
    qt_meta_data_HueFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_HueFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HueFilter, std::true_type>
    >,
    nullptr
} };

void HueFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *HueFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HueFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HueFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int HueFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_GammaFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GammaFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GammaFilter_t qt_meta_stringdata_GammaFilter = {
    {
        QT_MOC_LITERAL(0, 11)   // "GammaFilter"
    },
    "GammaFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GammaFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject GammaFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_GammaFilter.offsetsAndSizes,
    qt_meta_data_GammaFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GammaFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GammaFilter, std::true_type>
    >,
    nullptr
} };

void GammaFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *GammaFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GammaFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GammaFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int GammaFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_ExposureFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ExposureFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ExposureFilter_t qt_meta_stringdata_ExposureFilter = {
    {
        QT_MOC_LITERAL(0, 14)   // "ExposureFilter"
    },
    "ExposureFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ExposureFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject ExposureFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_ExposureFilter.offsetsAndSizes,
    qt_meta_data_ExposureFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ExposureFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ExposureFilter, std::true_type>
    >,
    nullptr
} };

void ExposureFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *ExposureFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ExposureFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ExposureFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int ExposureFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_SepiaFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SepiaFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SepiaFilter_t qt_meta_stringdata_SepiaFilter = {
    {
        QT_MOC_LITERAL(0, 11)   // "SepiaFilter"
    },
    "SepiaFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SepiaFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject SepiaFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_SepiaFilter.offsetsAndSizes,
    qt_meta_data_SepiaFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SepiaFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SepiaFilter, std::true_type>
    >,
    nullptr
} };

void SepiaFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *SepiaFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SepiaFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SepiaFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int SepiaFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_CLAHEFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLAHEFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLAHEFilter_t qt_meta_stringdata_CLAHEFilter = {
    {
        QT_MOC_LITERAL(0, 11)   // "CLAHEFilter"
    },
    "CLAHEFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLAHEFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject CLAHEFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_CLAHEFilter.offsetsAndSizes,
    qt_meta_data_CLAHEFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLAHEFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CLAHEFilter, std::true_type>
    >,
    nullptr
} };

void CLAHEFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *CLAHEFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CLAHEFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLAHEFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int CLAHEFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_TemperatureFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_TemperatureFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_TemperatureFilter_t qt_meta_stringdata_TemperatureFilter = {
    {
        QT_MOC_LITERAL(0, 17)   // "TemperatureFilter"
    },
    "TemperatureFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_TemperatureFilter[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject TemperatureFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_TemperatureFilter.offsetsAndSizes,
    qt_meta_data_TemperatureFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_TemperatureFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TemperatureFilter, std::true_type>
    >,
    nullptr
} };

void TemperatureFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TemperatureFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TemperatureFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TemperatureFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int TemperatureFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
