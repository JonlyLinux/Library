TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    jhash.c \
    avltree.c \
    ring.c \
    btree.c \
    rbt.c \
    test_rbt.c

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
    rbt.h
