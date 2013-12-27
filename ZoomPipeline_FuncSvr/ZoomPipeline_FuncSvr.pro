#-------------------------------------------------
#
# Project created by QtCreator 2013-12-13T08:12:31
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZoomPipeline_FuncSvr
TEMPLATE = app


SOURCES += main.cpp\
        zpmainframe.cpp \
    network/zp_tcpserver.cpp \
    network/zp_nettransthread.cpp \
    network/zp_netlistenthread.cpp \
    network/zp_net_threadpool.cpp \
    pipeline/zp_pipeline.cpp \
    dynhash/dynhash.cpp \
    smartlink/st_client_table.cpp \
    pipeline/zp_plworkingthread.cpp \
    smartlink/st_clientnode.cpp

HEADERS  += zpmainframe.h \
    network/zp_tcpserver.h \
    network/zp_nettransthread.h \
    network/zp_netlistenthread.h \
    network/zp_net_threadpool.h \
    pipeline/zp_pipeline.h \
    dynhash/dynhash.h \
    smartlink/st_client_table.h \
    pipeline/zp_plworkingthread.h \
    smartlink/st_clientnode.h

FORMS    += zpmainframe.ui

RESOURCES += \
    resource.qrc
