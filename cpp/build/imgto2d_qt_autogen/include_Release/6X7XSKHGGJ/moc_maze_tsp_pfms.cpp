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
QT_WARNING_POP
