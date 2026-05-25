/****************************************************************************
** Meta object code from reading C++ file 'maze_tsp_pfms.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../pfm/maze_tsp_pfms.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'maze_tsp_pfms.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSBaseTSPPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSBaseTSPPFMENDCLASS = QtMocHelpers::stringData(
    "BaseTSPPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBaseTSPPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject BaseTSPPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<PathFindingModule::staticMetaObject>(),
    qt_meta_stringdata_CLASSBaseTSPPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBaseTSPPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBaseTSPPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BaseTSPPFM, std::true_type>
    >,
    nullptr
} };

void BaseTSPPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *BaseTSPPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BaseTSPPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBaseTSPPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return PathFindingModule::qt_metacast(_clname);
}

int BaseTSPPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PathFindingModule::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTSPClassicPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTSPClassicPFMENDCLASS = QtMocHelpers::stringData(
    "TSPClassicPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTSPClassicPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject TSPClassicPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseTSPPFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSTSPClassicPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTSPClassicPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTSPClassicPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TSPClassicPFM, std::true_type>
    >,
    nullptr
} };

void TSPClassicPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TSPClassicPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TSPClassicPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTSPClassicPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseTSPPFM::qt_metacast(_clname);
}

int TSPClassicPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseTSPPFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTSPOutlinePFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTSPOutlinePFMENDCLASS = QtMocHelpers::stringData(
    "TSPOutlinePFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTSPOutlinePFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject TSPOutlinePFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseTSPPFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSTSPOutlinePFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTSPOutlinePFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTSPOutlinePFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TSPOutlinePFM, std::true_type>
    >,
    nullptr
} };

void TSPOutlinePFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TSPOutlinePFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TSPOutlinePFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTSPOutlinePFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseTSPPFM::qt_metacast(_clname);
}

int TSPOutlinePFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseTSPPFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTSPShadingPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTSPShadingPFMENDCLASS = QtMocHelpers::stringData(
    "TSPShadingPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTSPShadingPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject TSPShadingPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseTSPPFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSTSPShadingPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTSPShadingPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTSPShadingPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TSPShadingPFM, std::true_type>
    >,
    nullptr
} };

void TSPShadingPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TSPShadingPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TSPShadingPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTSPShadingPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseTSPPFM::qt_metacast(_clname);
}

int TSPShadingPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseTSPPFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTSPMSTPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTSPMSTPFMENDCLASS = QtMocHelpers::stringData(
    "TSPMSTPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTSPMSTPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject TSPMSTPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<PathFindingModule::staticMetaObject>(),
    qt_meta_stringdata_CLASSTSPMSTPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTSPMSTPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTSPMSTPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TSPMSTPFM, std::true_type>
    >,
    nullptr
} };

void TSPMSTPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TSPMSTPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TSPMSTPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTSPMSTPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return PathFindingModule::qt_metacast(_clname);
}

int TSPMSTPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PathFindingModule::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTSPStipplePFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTSPStipplePFMENDCLASS = QtMocHelpers::stringData(
    "TSPStipplePFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTSPStipplePFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject TSPStipplePFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseTSPPFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSTSPStipplePFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTSPStipplePFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTSPStipplePFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TSPStipplePFM, std::true_type>
    >,
    nullptr
} };

void TSPStipplePFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TSPStipplePFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TSPStipplePFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTSPStipplePFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseTSPPFM::qt_metacast(_clname);
}

int TSPStipplePFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseTSPPFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTSPVoronoiPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTSPVoronoiPFMENDCLASS = QtMocHelpers::stringData(
    "TSPVoronoiPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTSPVoronoiPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject TSPVoronoiPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseTSPPFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSTSPVoronoiPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTSPVoronoiPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTSPVoronoiPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TSPVoronoiPFM, std::true_type>
    >,
    nullptr
} };

void TSPVoronoiPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TSPVoronoiPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TSPVoronoiPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTSPVoronoiPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseTSPPFM::qt_metacast(_clname);
}

int TSPVoronoiPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseTSPPFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSBaseMazePFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSBaseMazePFMENDCLASS = QtMocHelpers::stringData(
    "BaseMazePFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBaseMazePFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject BaseMazePFM::staticMetaObject = { {
    QMetaObject::SuperData::link<PathFindingModule::staticMetaObject>(),
    qt_meta_stringdata_CLASSBaseMazePFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBaseMazePFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBaseMazePFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BaseMazePFM, std::true_type>
    >,
    nullptr
} };

void BaseMazePFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *BaseMazePFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BaseMazePFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBaseMazePFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return PathFindingModule::qt_metacast(_clname);
}

int BaseMazePFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PathFindingModule::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMazeRectPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMazeRectPFMENDCLASS = QtMocHelpers::stringData(
    "MazeRectPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMazeRectPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject MazeRectPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseMazePFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSMazeRectPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMazeRectPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMazeRectPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MazeRectPFM, std::true_type>
    >,
    nullptr
} };

void MazeRectPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MazeRectPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MazeRectPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMazeRectPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseMazePFM::qt_metacast(_clname);
}

int MazeRectPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseMazePFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMazeHexPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMazeHexPFMENDCLASS = QtMocHelpers::stringData(
    "MazeHexPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMazeHexPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject MazeHexPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseMazePFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSMazeHexPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMazeHexPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMazeHexPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MazeHexPFM, std::true_type>
    >,
    nullptr
} };

void MazeHexPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MazeHexPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MazeHexPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMazeHexPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseMazePFM::qt_metacast(_clname);
}

int MazeHexPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseMazePFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMazeTriPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMazeTriPFMENDCLASS = QtMocHelpers::stringData(
    "MazeTriPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMazeTriPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject MazeTriPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseMazePFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSMazeTriPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMazeTriPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMazeTriPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MazeTriPFM, std::true_type>
    >,
    nullptr
} };

void MazeTriPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MazeTriPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MazeTriPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMazeTriPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseMazePFM::qt_metacast(_clname);
}

int MazeTriPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseMazePFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMazeVoronoiPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMazeVoronoiPFMENDCLASS = QtMocHelpers::stringData(
    "MazeVoronoiPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMazeVoronoiPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject MazeVoronoiPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseMazePFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSMazeVoronoiPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMazeVoronoiPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMazeVoronoiPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MazeVoronoiPFM, std::true_type>
    >,
    nullptr
} };

void MazeVoronoiPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MazeVoronoiPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MazeVoronoiPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMazeVoronoiPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseMazePFM::qt_metacast(_clname);
}

int MazeVoronoiPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseMazePFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMazeCirclePFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMazeCirclePFMENDCLASS = QtMocHelpers::stringData(
    "MazeCirclePFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMazeCirclePFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject MazeCirclePFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseMazePFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSMazeCirclePFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMazeCirclePFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMazeCirclePFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MazeCirclePFM, std::true_type>
    >,
    nullptr
} };

void MazeCirclePFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MazeCirclePFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MazeCirclePFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMazeCirclePFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseMazePFM::qt_metacast(_clname);
}

int MazeCirclePFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseMazePFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMazeLabyrinthPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMazeLabyrinthPFMENDCLASS = QtMocHelpers::stringData(
    "MazeLabyrinthPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMazeLabyrinthPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject MazeLabyrinthPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseMazePFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSMazeLabyrinthPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMazeLabyrinthPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMazeLabyrinthPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MazeLabyrinthPFM, std::true_type>
    >,
    nullptr
} };

void MazeLabyrinthPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MazeLabyrinthPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MazeLabyrinthPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMazeLabyrinthPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return BaseMazePFM::qt_metacast(_clname);
}

int MazeLabyrinthPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseMazePFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSLabyrinthClassicPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSLabyrinthClassicPFMENDCLASS = QtMocHelpers::stringData(
    "LabyrinthClassicPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLabyrinthClassicPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject LabyrinthClassicPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<PathFindingModule::staticMetaObject>(),
    qt_meta_stringdata_CLASSLabyrinthClassicPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLabyrinthClassicPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLabyrinthClassicPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LabyrinthClassicPFM, std::true_type>
    >,
    nullptr
} };

void LabyrinthClassicPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *LabyrinthClassicPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LabyrinthClassicPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLabyrinthClassicPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return PathFindingModule::qt_metacast(_clname);
}

int LabyrinthClassicPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PathFindingModule::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSLabyrinthRomanPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSLabyrinthRomanPFMENDCLASS = QtMocHelpers::stringData(
    "LabyrinthRomanPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLabyrinthRomanPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject LabyrinthRomanPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<LabyrinthClassicPFM::staticMetaObject>(),
    qt_meta_stringdata_CLASSLabyrinthRomanPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLabyrinthRomanPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLabyrinthRomanPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LabyrinthRomanPFM, std::true_type>
    >,
    nullptr
} };

void LabyrinthRomanPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *LabyrinthRomanPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LabyrinthRomanPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLabyrinthRomanPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return LabyrinthClassicPFM::qt_metacast(_clname);
}

int LabyrinthRomanPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LabyrinthClassicPFM::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMazeHilbertPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMazeHilbertPFMENDCLASS = QtMocHelpers::stringData(
    "MazeHilbertPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMazeHilbertPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject MazeHilbertPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<PathFindingModule::staticMetaObject>(),
    qt_meta_stringdata_CLASSMazeHilbertPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMazeHilbertPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMazeHilbertPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MazeHilbertPFM, std::true_type>
    >,
    nullptr
} };

void MazeHilbertPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MazeHilbertPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MazeHilbertPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMazeHilbertPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return PathFindingModule::qt_metacast(_clname);
}

int MazeHilbertPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PathFindingModule::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMazePeanoPFMENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMazePeanoPFMENDCLASS = QtMocHelpers::stringData(
    "MazePeanoPFM"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMazePeanoPFMENDCLASS[] = {

 // content:
      12,       // revision
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

Q_CONSTINIT const QMetaObject MazePeanoPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<PathFindingModule::staticMetaObject>(),
    qt_meta_stringdata_CLASSMazePeanoPFMENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMazePeanoPFMENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMazePeanoPFMENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MazePeanoPFM, std::true_type>
    >,
    nullptr
} };

void MazePeanoPFM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MazePeanoPFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MazePeanoPFM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMazePeanoPFMENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return PathFindingModule::qt_metacast(_clname);
}

int MazePeanoPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PathFindingModule::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
