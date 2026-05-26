/****************************************************************************
** Meta object code from reading C++ file 'maze_tri_pfm.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../pfm/maze_tri_pfm.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'maze_tri_pfm.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MazeTriPFM_t {
    uint offsetsAndSizes[2];
    char stringdata0[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MazeTriPFM_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MazeTriPFM_t qt_meta_stringdata_MazeTriPFM = {
    {
        QT_MOC_LITERAL(0, 10)   // "MazeTriPFM"
    },
    "MazeTriPFM"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MazeTriPFM[] = {

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

Q_CONSTINIT const QMetaObject MazeTriPFM::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseMazePFM::staticMetaObject>(),
    qt_meta_stringdata_MazeTriPFM.offsetsAndSizes,
    qt_meta_data_MazeTriPFM,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MazeTriPFM_t,
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
    if (!strcmp(_clname, qt_meta_stringdata_MazeTriPFM.stringdata0))
        return static_cast<void*>(this);
    return BaseMazePFM::qt_metacast(_clname);
}

int MazeTriPFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseMazePFM::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
