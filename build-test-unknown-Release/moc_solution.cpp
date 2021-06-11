/****************************************************************************
** Meta object code from reading C++ file 'solution.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../test/solution.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'solution.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Solution_t {
    QByteArrayData data[12];
    char stringdata0[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Solution_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Solution_t qt_meta_stringdata_Solution = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Solution"
QT_MOC_LITERAL(1, 9, 14), // "caculatefinish"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 5), // "solve"
QT_MOC_LITERAL(4, 31, 6), // "string"
QT_MOC_LITERAL(5, 38, 11), // "char[20][4]"
QT_MOC_LITERAL(6, 50, 10), // "Start_cube"
QT_MOC_LITERAL(7, 61, 10), // "string[20]"
QT_MOC_LITERAL(8, 72, 4), // "goal"
QT_MOC_LITERAL(9, 77, 11), // "start_solve"
QT_MOC_LITERAL(10, 89, 9), // "start_str"
QT_MOC_LITERAL(11, 99, 10) // "stop_solve"

    },
    "Solution\0caculatefinish\0\0solve\0string\0"
    "char[20][4]\0Start_cube\0string[20]\0"
    "goal\0start_solve\0start_str\0stop_solve"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Solution[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    2,   35,    2, 0x0a /* Public */,
       9,    1,   40,    2, 0x0a /* Public */,
      11,    0,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    0x80000000 | 4, 0x80000000 | 5, 0x80000000 | 7,    6,    8,
    QMetaType::Void, 0x80000000 | 4,   10,
    QMetaType::Void,

       0        // eod
};

void Solution::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Solution *_t = static_cast<Solution *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->caculatefinish(); break;
        case 1: { string _r = _t->solve((*reinterpret_cast< char(*)[20][4]>(_a[1])),(*reinterpret_cast< string(*)[20]>(_a[2])));
            if (_a[0]) *reinterpret_cast< string*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->start_solve((*reinterpret_cast< string(*)>(_a[1]))); break;
        case 3: _t->stop_solve(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Solution::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Solution::caculatefinish)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Solution::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Solution.data,
      qt_meta_data_Solution,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Solution::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Solution::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Solution.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Solution::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Solution::caculatefinish()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
