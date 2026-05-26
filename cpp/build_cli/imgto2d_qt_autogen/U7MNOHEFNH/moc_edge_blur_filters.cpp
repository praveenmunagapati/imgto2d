/****************************************************************************
** Meta object code from reading C++ file 'edge_blur_filters.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../filters/edge_blur_filters.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'edge_blur_filters.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CannyFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CannyFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CannyFilter_t qt_meta_stringdata_CannyFilter = {
    {
        QT_MOC_LITERAL(0, 11)   // "CannyFilter"
    },
    "CannyFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CannyFilter[] = {

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

Q_CONSTINIT const QMetaObject CannyFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_CannyFilter.offsetsAndSizes,
    qt_meta_data_CannyFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CannyFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CannyFilter, std::true_type>
    >,
    nullptr
} };

void CannyFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *CannyFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CannyFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CannyFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int CannyFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_SobelFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SobelFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SobelFilter_t qt_meta_stringdata_SobelFilter = {
    {
        QT_MOC_LITERAL(0, 11)   // "SobelFilter"
    },
    "SobelFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SobelFilter[] = {

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

Q_CONSTINIT const QMetaObject SobelFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_SobelFilter.offsetsAndSizes,
    qt_meta_data_SobelFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SobelFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SobelFilter, std::true_type>
    >,
    nullptr
} };

void SobelFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *SobelFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SobelFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SobelFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int SobelFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_LaplacianFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LaplacianFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_LaplacianFilter_t qt_meta_stringdata_LaplacianFilter = {
    {
        QT_MOC_LITERAL(0, 15)   // "LaplacianFilter"
    },
    "LaplacianFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_LaplacianFilter[] = {

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

Q_CONSTINIT const QMetaObject LaplacianFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_LaplacianFilter.offsetsAndSizes,
    qt_meta_data_LaplacianFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LaplacianFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LaplacianFilter, std::true_type>
    >,
    nullptr
} };

void LaplacianFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *LaplacianFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LaplacianFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LaplacianFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int LaplacianFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_PrewittFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PrewittFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PrewittFilter_t qt_meta_stringdata_PrewittFilter = {
    {
        QT_MOC_LITERAL(0, 13)   // "PrewittFilter"
    },
    "PrewittFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PrewittFilter[] = {

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

Q_CONSTINIT const QMetaObject PrewittFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_PrewittFilter.offsetsAndSizes,
    qt_meta_data_PrewittFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PrewittFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PrewittFilter, std::true_type>
    >,
    nullptr
} };

void PrewittFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *PrewittFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PrewittFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PrewittFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int PrewittFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_ScharrFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ScharrFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ScharrFilter_t qt_meta_stringdata_ScharrFilter = {
    {
        QT_MOC_LITERAL(0, 12)   // "ScharrFilter"
    },
    "ScharrFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ScharrFilter[] = {

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

Q_CONSTINIT const QMetaObject ScharrFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_ScharrFilter.offsetsAndSizes,
    qt_meta_data_ScharrFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ScharrFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScharrFilter, std::true_type>
    >,
    nullptr
} };

void ScharrFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *ScharrFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScharrFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScharrFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int ScharrFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_DoGFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DoGFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DoGFilter_t qt_meta_stringdata_DoGFilter = {
    {
        QT_MOC_LITERAL(0, 9)   // "DoGFilter"
    },
    "DoGFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DoGFilter[] = {

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

Q_CONSTINIT const QMetaObject DoGFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_DoGFilter.offsetsAndSizes,
    qt_meta_data_DoGFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DoGFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DoGFilter, std::true_type>
    >,
    nullptr
} };

void DoGFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DoGFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DoGFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DoGFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int DoGFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_RidgeDetectionFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RidgeDetectionFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RidgeDetectionFilter_t qt_meta_stringdata_RidgeDetectionFilter = {
    {
        QT_MOC_LITERAL(0, 20)   // "RidgeDetectionFilter"
    },
    "RidgeDetectionFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RidgeDetectionFilter[] = {

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

Q_CONSTINIT const QMetaObject RidgeDetectionFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_RidgeDetectionFilter.offsetsAndSizes,
    qt_meta_data_RidgeDetectionFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RidgeDetectionFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RidgeDetectionFilter, std::true_type>
    >,
    nullptr
} };

void RidgeDetectionFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *RidgeDetectionFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RidgeDetectionFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RidgeDetectionFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int RidgeDetectionFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_HighPassFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_HighPassFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_HighPassFilter_t qt_meta_stringdata_HighPassFilter = {
    {
        QT_MOC_LITERAL(0, 14)   // "HighPassFilter"
    },
    "HighPassFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_HighPassFilter[] = {

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

Q_CONSTINIT const QMetaObject HighPassFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_HighPassFilter.offsetsAndSizes,
    qt_meta_data_HighPassFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_HighPassFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HighPassFilter, std::true_type>
    >,
    nullptr
} };

void HighPassFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *HighPassFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HighPassFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HighPassFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int HighPassFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_GaussianBlurFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GaussianBlurFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GaussianBlurFilter_t qt_meta_stringdata_GaussianBlurFilter = {
    {
        QT_MOC_LITERAL(0, 18)   // "GaussianBlurFilter"
    },
    "GaussianBlurFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GaussianBlurFilter[] = {

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

Q_CONSTINIT const QMetaObject GaussianBlurFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_GaussianBlurFilter.offsetsAndSizes,
    qt_meta_data_GaussianBlurFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GaussianBlurFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GaussianBlurFilter, std::true_type>
    >,
    nullptr
} };

void GaussianBlurFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *GaussianBlurFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GaussianBlurFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GaussianBlurFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int GaussianBlurFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_MedianBlurFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MedianBlurFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MedianBlurFilter_t qt_meta_stringdata_MedianBlurFilter = {
    {
        QT_MOC_LITERAL(0, 16)   // "MedianBlurFilter"
    },
    "MedianBlurFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MedianBlurFilter[] = {

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

Q_CONSTINIT const QMetaObject MedianBlurFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_MedianBlurFilter.offsetsAndSizes,
    qt_meta_data_MedianBlurFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MedianBlurFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MedianBlurFilter, std::true_type>
    >,
    nullptr
} };

void MedianBlurFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MedianBlurFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MedianBlurFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MedianBlurFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int MedianBlurFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_BoxBlurFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_BoxBlurFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_BoxBlurFilter_t qt_meta_stringdata_BoxBlurFilter = {
    {
        QT_MOC_LITERAL(0, 13)   // "BoxBlurFilter"
    },
    "BoxBlurFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_BoxBlurFilter[] = {

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

Q_CONSTINIT const QMetaObject BoxBlurFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_BoxBlurFilter.offsetsAndSizes,
    qt_meta_data_BoxBlurFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_BoxBlurFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BoxBlurFilter, std::true_type>
    >,
    nullptr
} };

void BoxBlurFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *BoxBlurFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BoxBlurFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BoxBlurFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int BoxBlurFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_MotionBlurFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MotionBlurFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MotionBlurFilter_t qt_meta_stringdata_MotionBlurFilter = {
    {
        QT_MOC_LITERAL(0, 16)   // "MotionBlurFilter"
    },
    "MotionBlurFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MotionBlurFilter[] = {

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

Q_CONSTINIT const QMetaObject MotionBlurFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_MotionBlurFilter.offsetsAndSizes,
    qt_meta_data_MotionBlurFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MotionBlurFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MotionBlurFilter, std::true_type>
    >,
    nullptr
} };

void MotionBlurFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MotionBlurFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MotionBlurFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MotionBlurFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int MotionBlurFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_BilateralFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_BilateralFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_BilateralFilter_t qt_meta_stringdata_BilateralFilter = {
    {
        QT_MOC_LITERAL(0, 15)   // "BilateralFilter"
    },
    "BilateralFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_BilateralFilter[] = {

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

Q_CONSTINIT const QMetaObject BilateralFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_BilateralFilter.offsetsAndSizes,
    qt_meta_data_BilateralFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_BilateralFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BilateralFilter, std::true_type>
    >,
    nullptr
} };

void BilateralFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *BilateralFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BilateralFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BilateralFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int BilateralFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_LowPassFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LowPassFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_LowPassFilter_t qt_meta_stringdata_LowPassFilter = {
    {
        QT_MOC_LITERAL(0, 13)   // "LowPassFilter"
    },
    "LowPassFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_LowPassFilter[] = {

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

Q_CONSTINIT const QMetaObject LowPassFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_LowPassFilter.offsetsAndSizes,
    qt_meta_data_LowPassFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LowPassFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LowPassFilter, std::true_type>
    >,
    nullptr
} };

void LowPassFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *LowPassFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LowPassFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LowPassFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int LowPassFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_SharpenMoreFilter_t {
    uint offsetsAndSizes[2];
    char stringdata0[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_SharpenMoreFilter_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_SharpenMoreFilter_t qt_meta_stringdata_SharpenMoreFilter = {
    {
        QT_MOC_LITERAL(0, 17)   // "SharpenMoreFilter"
    },
    "SharpenMoreFilter"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_SharpenMoreFilter[] = {

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

Q_CONSTINIT const QMetaObject SharpenMoreFilter::staticMetaObject = { {
    QMetaObject::SuperData::link<ImageFilter::staticMetaObject>(),
    qt_meta_stringdata_SharpenMoreFilter.offsetsAndSizes,
    qt_meta_data_SharpenMoreFilter,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_SharpenMoreFilter_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SharpenMoreFilter, std::true_type>
    >,
    nullptr
} };

void SharpenMoreFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *SharpenMoreFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SharpenMoreFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SharpenMoreFilter.stringdata0))
        return static_cast<void*>(this);
    return ImageFilter::qt_metacast(_clname);
}

int SharpenMoreFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
