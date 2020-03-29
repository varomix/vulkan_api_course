TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lvulkan -lxcb -lglfw

SOURCES += \
        main.cpp \
    renderer.cpp

HEADERS += \
    renderer.h \
    utilities.h
