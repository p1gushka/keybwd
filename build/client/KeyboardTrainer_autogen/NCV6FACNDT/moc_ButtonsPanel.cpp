/****************************************************************************
** Meta object code from reading C++ file 'ButtonsPanel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../client/view/components/ButtonsPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ButtonsPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_view__ButtonsPanel_t {
    const uint offsetsAndSize[10];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_view__ButtonsPanel_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_view__ButtonsPanel_t qt_meta_stringdata_view__ButtonsPanel = {
    {
QT_MOC_LITERAL(0, 18), // "view::ButtonsPanel"
QT_MOC_LITERAL(19, 13), // "repeatClicked"
QT_MOC_LITERAL(33, 0), // ""
QT_MOC_LITERAL(34, 14), // "restartClicked"
QT_MOC_LITERAL(49, 13) // "errorsClicked"

    },
    "view::ButtonsPanel\0repeatClicked\0\0"
    "restartClicked\0errorsClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_view__ButtonsPanel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x06,    1 /* Public */,
       3,    0,   33,    2, 0x06,    2 /* Public */,
       4,    0,   34,    2, 0x06,    3 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void view::ButtonsPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ButtonsPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->repeatClicked(); break;
        case 1: _t->restartClicked(); break;
        case 2: _t->errorsClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ButtonsPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ButtonsPanel::repeatClicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ButtonsPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ButtonsPanel::restartClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ButtonsPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ButtonsPanel::errorsClicked)) {
                *result = 2;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject view::ButtonsPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_view__ButtonsPanel.offsetsAndSize,
    qt_meta_data_view__ButtonsPanel,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_view__ButtonsPanel_t
, QtPrivate::TypeAndForceComplete<ButtonsPanel, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *view::ButtonsPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *view::ButtonsPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_view__ButtonsPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int view::ButtonsPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void view::ButtonsPanel::repeatClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void view::ButtonsPanel::restartClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void view::ButtonsPanel::errorsClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
