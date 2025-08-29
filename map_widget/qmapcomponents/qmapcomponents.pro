_BUILD_QT_DESIGNER_COMPONENTS = yes
TARGET = mapcomponents
TEMPLATE = lib

CONFIG += qt warn_on qt_no_compat_warning release plugin thread

SOURCES += \
  qdmcmp.cpp qdmcompa.cpp qdmdsel.cpp qdmfind.cpp qdmlayr.cpp qdmmet.cpp qdmmtr.cpp qdmobja.cpp \
  qdmobj.cpp qdmpoina.cpp qdmpoint.cpp qdmrst.cpp qdmselob.cpp qdmselt.cpp qdmsem.cpp \
  qdmsite.cpp qdmviewa.cpp qdmwina.cpp qdmwinw.cpp qdmact.cpp qdmrect.cpp \
  qdmconv.cpp qdmactpt.cpp qdmdlist.cpp \
  qdmdcmtw.cpp qdmdprn.cpp qdmdgtob.cpp qdmvport.cpp qdmgsrv.cpp qdmd3d.cpp qdmnet.cpp qdmdwmspopup.cpp \
  qdmactline.cpp qdmanimator.cpp

HEADERS += \
  qdmcmp.h qdmcompa.h qdmdsel.h qdmfind.h qdmlayr.h qdmmet.h qdmmtr.h qdmobja.h \
  qdmobj.h qdmpoina.h qdmpoint.h qdmrst.h qdmselob.h qdmselt.h qdmsem.h \
  qdmsite.h qdmviewa.h qdmwina.h qdmwinw.h \
  qdmact.h qdmrect.h qdmconv.h qdmactpt.h \
  qdmdlist.h qdmdcmtw.h qdmdprn.h \
  qdmdgtob.h qdmvport.h qdmgsrv.h qdmd3d.h qdmnet.h qdmdwmspopup.h \
  qdmactline.h qdmanimator.h

!isEmpty( _HIDE_3DSUPPORT ) {
  SOURCES -= qdmd3d.cpp
  HEADERS -= qdmd3d.h
  DEFINES += HIDE_3DSUPPORT
}
!isEmpty( _HIDE_PRINTSUPPORT ) {
  SOURCES -= qdmdprn.cpp
  HEADERS -= qdmdprn.h
  DEFINES += HIDE_PRINTSUPPORT
}
!isEmpty( _HIDE_GISSERVER_SUPPORT ) {
  SOURCES -= qdmgsrv.cpp
  HEADERS -= qdmgsrv.h
  DEFINES += HIDE_GISSERVER_SUPPORT
}

!isEmpty( _BUILD_QT_DESIGNER_COMPONENTS ) {
  greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
  } else {
    CONFIG += designer
  }
  SOURCES += qdmapcomp.cpp qdmapcomp_plugin.cpp
  HEADERS += qdmapcomp_plugin.h qdmapcomp.h
}

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets printsupport
}

DEFINES += HIDEMAXMIN GIS_DESIGNER

target.path  = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target
INCLUDEPATH += /usr/include/gisdesigner/
LIBS += -lqdmapacces -lqdobjnet
