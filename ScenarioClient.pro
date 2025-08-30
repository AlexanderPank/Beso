QT       += websockets network core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

CONFIG += precompile_header
PRECOMPILED_HEADER = pch.h


QMAKE_LFLAGS += -Wl,--no-keep-memory  # Оптимизация линковки
CONFIG += reduce_exports  # Уменьшает размер бинарников
CONFIG += optimize_size  # Оптимизирует размер исполняемого файла

DESTDIR = $$PWD/bin



INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MapComponent.cpp \
    SimObjectViewer.cpp \
    engineservice.cpp \
    main.cpp \
    mainwindow.cpp \
    map_widget/map_additional/map_coordinates.cpp \
    map_widget/map_additional/map_scale.cpp \
    map_widget/map_additional/map_tool_panel.cpp \
    map_widget/map_tools/coord_ctx.cpp \
    map_widget/mapwidget.cpp \
    signs/geoutil.cpp \
    signs/sign_base.cpp \
    signs/sign_controller.cpp \
    signs/SignAnimator.cpp \
    signs/sign_ship.cpp \
    websocketservice.cpp

HEADERS += \
    MapComponent.h \
    SimObjectModel.h \
    SimObjectViewer.h \
    engineservice.h \
    mainwindow.h \
    map_widget/map_additional/map_coordinates.h \
    map_widget/map_additional/map_scale.h \
    map_widget/map_additional/map_tool_panel.h \
    map_widget/map_enums.h \
    map_widget/map_tools/coord_ctx.h \
    map_widget/mapwidget.h \
    pch.h \
    signs/geoutil.h \
    signs/sign_base.h \
    signs/sign_controller.h \
    signs/SignAnimator.h \
    signs/sign_enums.h \
    signs/sign_ship.h \
    websocketservice.h

FORMS += \
    mainwindow.ui \
    map_widget/map_additional/map_coordinates.ui \
    map_widget/map_additional/map_scale.ui \
    map_widget/map_additional/map_tool_panel.ui \
    map_widget/mapwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    map_widget/map_additional/signs.qrc \
    resource.qrc

SUBDIRS += \
    map_widget/qmapcomponents/qmapcomponents.pro

INCLUDEPATH += \
    map_widget/qmapcomponents \
    /usr/include/gisdesigner

LIBS += \
    -lqdmapacces                    \
    -lqdmapmtrex                    \
    -lmapcomponents                 \
    -ldl
