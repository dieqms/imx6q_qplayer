#-------------------------------------------------
#
# Project created by QtCreator 2018-05-07T09:34:21
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia  multimediawidgets
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QPlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -Wl,-dn -L./3rd/lib -lbase -lparson -Wl,-dy -ldl -rdynamic
LIBS += -L ./3rd/lib -lgobject-2.0 -lglib-2.0 -lgstreamer-1.0 -lgstvideo-1.0 -lgstbase-1.0

INCLUDEPATH += ./3rd/include/
INCLUDEPATH += ./3rd/include/gstreamer-1.0/
INCLUDEPATH += ./3rd/include/glib-2.0/
INCLUDEPATH += ./3rd/include/parson

QMAKE_CFLAGS += -fPIC -DQT_OPENGL_ES_2 -Wdeprecated-declarations -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable
QMAKE_CXXFLAGS += -std=c++0x -fPIC -DQT_OPENGL_ES_2 -Wdeprecated-declarations -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable

RESOURCES += \


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    glyuvwidget.cpp \
    glwidget.cpp \
    paintlabel.cpp \
    player_core.cpp \
    player_record.cpp \
    player_video.cpp \
    player_audio.cpp \
    player_widget.cpp \
    config.cpp

HEADERS += \
    mainwindow.h \
    glyuvwidget.h \
    glwidget.h \
    paintlabel.h \
    player_core.h \
    player_record.h \
    player_video.h \
    player_audio.h \
    player_widget.h \
    config.h

FORMS += \
    mainwindow.ui
