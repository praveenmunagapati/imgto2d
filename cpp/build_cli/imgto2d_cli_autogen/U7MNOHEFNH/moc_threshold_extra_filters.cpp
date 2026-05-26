/****************************************************************************
** Meta object code from reading C++ file 'threshold_extra_filters.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../filters/threshold_extra_filters.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'threshold_extra_filters.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_OtsuThresholdFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_OtsuThresholdFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_OtsuThresholdFilter_t qt_meta_stringdata_OtsuThresholdFilter = {
    {
        QT_MOC_LITERAL(0, 19)   // "OtsuThresholdFilter"
    },
    "OtsuThresholdFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_OtsuThresholdFilter[] = {

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

Q_CONSTINIT const QMetaObject OtsuThresholdFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_OtsuThresholdFilter.offsetsAndSizes,
    qt_meta_data_OtsuThresholdFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_OtsuThresholdFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<OtsuThresholdFilter, std::true_type>
    >,
    nullptr
} };

void OtsuThresholdFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *OtsuThresholdFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OtsuThresholdFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OtsuThresholdFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int OtsuThresholdFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_AdaptiveThresholdFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_AdaptiveThresholdFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_AdaptiveThresholdFilter_t qt_meta_stringdata_AdaptiveThresholdFilter = {
    {
        QT_MOC_LITERAL(0, 23)   // "AdaptiveThresholdFilter"
    },
    "AdaptiveThresholdFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_AdaptiveThresholdFilter[] = {

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

Q_CONSTINIT const QMetaObject AdaptiveThresholdFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_AdaptiveThresholdFilter.offsetsAndSizes,
    qt_meta_data_AdaptiveThresholdFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_AdaptiveThresholdFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AdaptiveThresholdFilter, std::true_type>
    >,
    nullptr
} };

void AdaptiveThresholdFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *AdaptiveThresholdFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AdaptiveThresholdFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AdaptiveThresholdFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int AdaptiveThresholdFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_TruncateThresholdFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_TruncateThresholdFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_TruncateThresholdFilter_t qt_meta_stringdata_TruncateThresholdFilter = {
    {
        QT_MOC_LITERAL(0, 23)   // "TruncateThresholdFilter"
    },
    "TruncateThresholdFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_TruncateThresholdFilter[] = {

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

Q_CONSTINIT const QMetaObject TruncateThresholdFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_TruncateThresholdFilter.offsetsAndSizes,
    qt_meta_data_TruncateThresholdFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_TruncateThresholdFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TruncateThresholdFilter, std::true_type>
    >,
    nullptr
} };

void TruncateThresholdFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TruncateThresholdFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TruncateThresholdFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TruncateThresholdFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int TruncateThresholdFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_ToZeroThresholdFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[22];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ToZeroThresholdFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ToZeroThresholdFilter_t qt_meta_stringdata_ToZeroThresholdFilter = {
    {
        QT_MOC_LITERAL(0, 21)   // "ToZeroThresholdFilter"
    },
    "ToZeroThresholdFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ToZeroThresholdFilter[] = {

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

Q_CONSTINIT const QMetaObject ToZeroThresholdFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_ToZeroThresholdFilter.offsetsAndSizes,
    qt_meta_data_ToZeroThresholdFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ToZeroThresholdFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ToZeroThresholdFilter, std::true_type>
    >,
    nullptr
} };

void ToZeroThresholdFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *ToZeroThresholdFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToZeroThresholdFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ToZeroThresholdFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int ToZeroThresholdFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_EqualizeHistFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_EqualizeHistFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_EqualizeHistFilter_t qt_meta_stringdata_EqualizeHistFilter = {
    {
        QT_MOC_LITERAL(0, 18)   // "EqualizeHistFilter"
    },
    "EqualizeHistFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_EqualizeHistFilter[] = {

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

Q_CONSTINIT const QMetaObject EqualizeHistFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_EqualizeHistFilter.offsetsAndSizes,
    qt_meta_data_EqualizeHistFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_EqualizeHistFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<EqualizeHistFilter, std::true_type>
    >,
    nullptr
} };

void EqualizeHistFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *EqualizeHistFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EqualizeHistFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EqualizeHistFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int EqualizeHistFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_AutoContrastFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_AutoContrastFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_AutoContrastFilter_t qt_meta_stringdata_AutoContrastFilter = {
    {
        QT_MOC_LITERAL(0, 18)   // "AutoContrastFilter"
    },
    "AutoContrastFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_AutoContrastFilter[] = {

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

Q_CONSTINIT const QMetaObject AutoContrastFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_AutoContrastFilter.offsetsAndSizes,
    qt_meta_data_AutoContrastFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_AutoContrastFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AutoContrastFilter, std::true_type>
    >,
    nullptr
} };

void AutoContrastFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *AutoContrastFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AutoContrastFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AutoContrastFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int AutoContrastFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_AutoColorFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_AutoColorFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_AutoColorFilter_t qt_meta_stringdata_AutoColorFilter = {
    {
        QT_MOC_LITERAL(0, 15)   // "AutoColorFilter"
    },
    "AutoColorFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_AutoColorFilter[] = {

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

Q_CONSTINIT const QMetaObject AutoColorFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_AutoColorFilter.offsetsAndSizes,
    qt_meta_data_AutoColorFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_AutoColorFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AutoColorFilter, std::true_type>
    >,
    nullptr
} };

void AutoColorFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *AutoColorFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AutoColorFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AutoColorFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int AutoColorFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_ColorizeFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ColorizeFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ColorizeFilter_t qt_meta_stringdata_ColorizeFilter = {
    {
        QT_MOC_LITERAL(0, 14)   // "ColorizeFilter"
    },
    "ColorizeFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ColorizeFilter[] = {

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

Q_CONSTINIT const QMetaObject ColorizeFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_ColorizeFilter.offsetsAndSizes,
    qt_meta_data_ColorizeFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ColorizeFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ColorizeFilter, std::true_type>
    >,
    nullptr
} };

void ColorizeFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *ColorizeFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ColorizeFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ColorizeFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int ColorizeFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_InvertHueFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_InvertHueFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_InvertHueFilter_t qt_meta_stringdata_InvertHueFilter = {
    {
        QT_MOC_LITERAL(0, 15)   // "InvertHueFilter"
    },
    "InvertHueFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_InvertHueFilter[] = {

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

Q_CONSTINIT const QMetaObject InvertHueFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_InvertHueFilter.offsetsAndSizes,
    qt_meta_data_InvertHueFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_InvertHueFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<InvertHueFilter, std::true_type>
    >,
    nullptr
} };

void InvertHueFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *InvertHueFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *InvertHueFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_InvertHueFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int InvertHueFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
