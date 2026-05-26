/****************************************************************************
** Meta object code from reading C++ file 'noise_filters.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../filters/noise_filters.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'noise_filters.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_GaussianNoiseFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GaussianNoiseFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GaussianNoiseFilter_t qt_meta_stringdata_GaussianNoiseFilter = {
    {
        QT_MOC_LITERAL(0, 19)   // "GaussianNoiseFilter"
    },
    "GaussianNoiseFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GaussianNoiseFilter[] = {

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

Q_CONSTINIT const QMetaObject GaussianNoiseFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_GaussianNoiseFilter.offsetsAndSizes,
    qt_meta_data_GaussianNoiseFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GaussianNoiseFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GaussianNoiseFilter, std::true_type>
    >,
    nullptr
} };

void GaussianNoiseFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *GaussianNoiseFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GaussianNoiseFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GaussianNoiseFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int GaussianNoiseFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_SaltAndPepperFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SaltAndPepperFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SaltAndPepperFilter_t qt_meta_stringdata_SaltAndPepperFilter = {
    {
        QT_MOC_LITERAL(0, 19)   // "SaltAndPepperFilter"
    },
    "SaltAndPepperFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SaltAndPepperFilter[] = {

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

Q_CONSTINIT const QMetaObject SaltAndPepperFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_SaltAndPepperFilter.offsetsAndSizes,
    qt_meta_data_SaltAndPepperFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SaltAndPepperFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SaltAndPepperFilter, std::true_type>
    >,
    nullptr
} };

void SaltAndPepperFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *SaltAndPepperFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SaltAndPepperFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SaltAndPepperFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int SaltAndPepperFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_DenoiseFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DenoiseFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DenoiseFilter_t qt_meta_stringdata_DenoiseFilter = {
    {
        QT_MOC_LITERAL(0, 13)   // "DenoiseFilter"
    },
    "DenoiseFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DenoiseFilter[] = {

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

Q_CONSTINIT const QMetaObject DenoiseFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_DenoiseFilter.offsetsAndSizes,
    qt_meta_data_DenoiseFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DenoiseFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DenoiseFilter, std::true_type>
    >,
    nullptr
} };

void DenoiseFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DenoiseFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DenoiseFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DenoiseFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int DenoiseFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_GaussianNoise2Filter_t {
    uint offsetsAndSizes[2];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GaussianNoise2Filter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GaussianNoise2Filter_t qt_meta_stringdata_GaussianNoise2Filter = {
    {
        QT_MOC_LITERAL(0, 20)   // "GaussianNoise2Filter"
    },
    "GaussianNoise2Filter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GaussianNoise2Filter[] = {

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

Q_CONSTINIT const QMetaObject GaussianNoise2Filter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_GaussianNoise2Filter.offsetsAndSizes,
    qt_meta_data_GaussianNoise2Filter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GaussianNoise2Filter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GaussianNoise2Filter, std::true_type>
    >,
    nullptr
} };

void GaussianNoise2Filter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *GaussianNoise2Filter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GaussianNoise2Filter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GaussianNoise2Filter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int GaussianNoise2Filter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_SpeckleNoiseFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SpeckleNoiseFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SpeckleNoiseFilter_t qt_meta_stringdata_SpeckleNoiseFilter = {
    {
        QT_MOC_LITERAL(0, 18)   // "SpeckleNoiseFilter"
    },
    "SpeckleNoiseFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SpeckleNoiseFilter[] = {

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

Q_CONSTINIT const QMetaObject SpeckleNoiseFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_SpeckleNoiseFilter.offsetsAndSizes,
    qt_meta_data_SpeckleNoiseFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SpeckleNoiseFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SpeckleNoiseFilter, std::true_type>
    >,
    nullptr
} };

void SpeckleNoiseFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *SpeckleNoiseFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpeckleNoiseFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpeckleNoiseFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int SpeckleNoiseFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
