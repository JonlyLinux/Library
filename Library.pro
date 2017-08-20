TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    jhash.c

DISTFILES += \
    Library.pro.user \
    README.md

HEADERS += \
    list.h \
    queue.h \
    jhash.h
