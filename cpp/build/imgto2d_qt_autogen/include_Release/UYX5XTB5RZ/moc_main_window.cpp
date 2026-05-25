/****************************************************************************
** Meta object code from reading C++ file 'main_window.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../ui/main_window.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "onExportSVG",
    "",
    "onExportGCode",
    "onExportHPGL",
    "onExportPDF",
    "onOpenProject",
    "onSaveProject",
    "onSaveProjectAs",
    "onImportVideoFrame",
    "onExportSettings",
    "onEditPens",
    "onLoadImage",
    "onLoadMask",
    "onRunBatch",
    "onStartProcessing",
    "onReset",
    "onPFMSelectionChanged",
    "idx",
    "onProcessingFinished",
    "QList<DrawingGeometry>",
    "geometries",
    "onProgressUpdate",
    "pct",
    "shapes",
    "text",
    "onProcessingError",
    "message",
    "onAddFilter",
    "onRemoveFilter",
    "onMoveFilterUp",
    "onMoveFilterDown",
    "onFilterListSelectionChanged",
    "onSettingChanged"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  164,    2, 0x08,    1 /* Private */,
       3,    0,  165,    2, 0x08,    2 /* Private */,
       4,    0,  166,    2, 0x08,    3 /* Private */,
       5,    0,  167,    2, 0x08,    4 /* Private */,
       6,    0,  168,    2, 0x08,    5 /* Private */,
       7,    0,  169,    2, 0x08,    6 /* Private */,
       8,    0,  170,    2, 0x08,    7 /* Private */,
       9,    0,  171,    2, 0x08,    8 /* Private */,
      10,    0,  172,    2, 0x08,    9 /* Private */,
      11,    0,  173,    2, 0x08,   10 /* Private */,
      12,    0,  174,    2, 0x08,   11 /* Private */,
      13,    0,  175,    2, 0x08,   12 /* Private */,
      14,    0,  176,    2, 0x08,   13 /* Private */,
      15,    0,  177,    2, 0x08,   14 /* Private */,
      16,    0,  178,    2, 0x08,   15 /* Private */,
      17,    1,  179,    2, 0x08,   16 /* Private */,
      19,    1,  182,    2, 0x08,   18 /* Private */,
      22,    3,  185,    2, 0x08,   20 /* Private */,
      26,    1,  192,    2, 0x08,   24 /* Private */,
      28,    0,  195,    2, 0x08,   26 /* Private */,
      29,    0,  196,    2, 0x08,   27 /* Private */,
      30,    0,  197,    2, 0x08,   28 /* Private */,
      31,    0,  198,    2, 0x08,   29 /* Private */,
      32,    0,  199,    2, 0x08,   30 /* Private */,
      33,    0,  200,    2, 0x08,   31 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, QMetaType::Float, QMetaType::Int, QMetaType::QString,   23,   24,   25,
    QMetaType::Void, QMetaType::QString,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'onExportSVG'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportGCode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportHPGL'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportPDF'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onOpenProject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveProject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSaveProjectAs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImportVideoFrame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditPens'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLoadImage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLoadMask'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRunBatch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStartProcessing'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPFMSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onProcessingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<DrawingGeometry>, std::false_type>,
        // method 'onProgressUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'onProcessingError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'onAddFilter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRemoveFilter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMoveFilterUp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMoveFilterDown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFilterListSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSettingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onExportSVG(); break;
        case 1: _t->onExportGCode(); break;
        case 2: _t->onExportHPGL(); break;
        case 3: _t->onExportPDF(); break;
        case 4: _t->onOpenProject(); break;
        case 5: _t->onSaveProject(); break;
        case 6: _t->onSaveProjectAs(); break;
        case 7: _t->onImportVideoFrame(); break;
        case 8: _t->onExportSettings(); break;
        case 9: _t->onEditPens(); break;
        case 10: _t->onLoadImage(); break;
        case 11: _t->onLoadMask(); break;
        case 12: _t->onRunBatch(); break;
        case 13: _t->onStartProcessing(); break;
        case 14: _t->onReset(); break;
        case 15: _t->onPFMSelectionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->onProcessingFinished((*reinterpret_cast< std::add_pointer_t<QList<DrawingGeometry>>>(_a[1]))); break;
        case 17: _t->onProgressUpdate((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 18: _t->onProcessingError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: _t->onAddFilter(); break;
        case 20: _t->onRemoveFilter(); break;
        case 21: _t->onMoveFilterUp(); break;
        case 22: _t->onMoveFilterDown(); break;
        case 23: _t->onFilterListSelectionChanged(); break;
        case 24: _t->onSettingChanged(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 25;
    }
    return _id;
}
QT_WARNING_POP
