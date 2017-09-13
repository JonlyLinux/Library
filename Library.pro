TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS  += -D__ARCH_QT__

SOURCES += main.c \
    jhash.c \
    avltree.c \
    ring.c \
    btree.c \
    rbt.c \
    test_rbt.c \
    test_skiplist.c \
    skiplist.c

DISTFILES += \
    Library.pro.user \
    README.md

HEADERS += \
    list.h \
    queue.h \
    jhash.h \
    stdmacro.h \
    avltree.h \
    sync.h \
    builtin.h \
    ring.h \
    btree.h \
    rbt.h \
    skiplist.h
