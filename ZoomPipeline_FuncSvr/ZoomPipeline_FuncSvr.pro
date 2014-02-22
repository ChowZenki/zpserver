#-------------------------------------------------
#
# Project created by QtCreator 2013-12-13T08:12:31
#
#-------------------------------------------------

QT       += core gui network sql

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
    smartlink/st_client_table.cpp \
    pipeline/zp_plworkingthread.cpp \
    pipeline/zp_pltaskbase.cpp \
    database/databaseresource.cpp \
    smartlink/st_clientnode_basetrans.cpp \
    smartlink/st_clientnode_app_imp.cpp \
    smartlink/st_clientnode_applayer.cpp

HEADERS  += zpmainframe.h \
    network/zp_tcpserver.h \
    network/zp_nettransthread.h \
    network/zp_netlistenthread.h \
    network/zp_net_threadpool.h \
    pipeline/zp_pipeline.h \
    smartlink/st_client_table.h \
    pipeline/zp_plworkingthread.h \
    smartlink/st_message.h \
    pipeline/zp_pltaskbase.h \
    database/databaseresource.h \
    smartlink/st_msg_applayer.h \
    smartlink/st_clientnode_basetrans.h \
    smartlink/st_clientnode_applayer.h

FORMS    += zpmainframe.ui

RESOURCES += \
    resource.qrc

