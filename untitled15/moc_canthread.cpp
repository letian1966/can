/****************************************************************************
** Meta object code from reading C++ file 'canthread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "canthread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'canthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CANThread_t {
    QByteArrayData data[12];
    char stringdata0[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CANThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CANThread_t qt_meta_stringdata_CANThread = {
    {
QT_MOC_LITERAL(0, 0, 9), // "CANThread"
QT_MOC_LITERAL(1, 10, 16), // "canStatusChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 9), // "connected"
QT_MOC_LITERAL(4, 38, 13), // "errorOccurred"
QT_MOC_LITERAL(5, 52, 5), // "error"
QT_MOC_LITERAL(6, 58, 12), // "positionSent"
QT_MOC_LITERAL(7, 71, 2), // "id"
QT_MOC_LITERAL(8, 74, 7), // "int32_t"
QT_MOC_LITERAL(9, 82, 8), // "position"
QT_MOC_LITERAL(10, 91, 9), // "frameInfo"
QT_MOC_LITERAL(11, 101, 4) // "info"

    },
    "CANThread\0canStatusChanged\0\0connected\0"
    "errorOccurred\0error\0positionSent\0id\0"
    "int32_t\0position\0frameInfo\0info"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CANThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       6,    2,   40,    2, 0x06 /* Public */,
      10,    1,   45,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 8,    7,    9,
    QMetaType::Void, QMetaType::QString,   11,

       0        // eod
};

void CANThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CANThread *_t = static_cast<CANThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->canStatusChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->positionSent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int32_t(*)>(_a[2]))); break;
        case 3: _t->frameInfo((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (CANThread::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CANThread::canStatusChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (CANThread::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CANThread::errorOccurred)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (CANThread::*_t)(int , int32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CANThread::positionSent)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (CANThread::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CANThread::frameInfo)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject CANThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_CANThread.data,
      qt_meta_data_CANThread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CANThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CANThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CANThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int CANThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
void CANThread::canStatusChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CANThread::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CANThread::positionSent(int _t1, int32_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CANThread::frameInfo(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
