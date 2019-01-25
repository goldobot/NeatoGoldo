#-------------------------------------------------
#
# Project created by QtCreator 2018-05-07T21:58:37
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = Neato
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# *msvc* { # visual studio spec filter
#       QMAKE_CXXFLAGS += -MP
# }
# FIXME : DEBUG : FDE : pour compil ds Ubuntu
QMAKE_CFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++11

# FIXME : DEBUG : FDE : NO precompile_header
# PRECOMPILED_HEADER = Pch/my_precompiled_header.h
# CONFIG += precompile_header

# FIXME : DEBUG : FDE

HEADERS += \
        C_Main.h \
    NeatoAPI/Constants.h \
    NeatoAPI/Command/C_NApi_Command.h \
    NeatoAPI/CoreSlam/C_NApi_CoreSlam_8bppMap.h \
    NeatoAPI/Sensors/C_NApi_Sensors.h \
    NeatoAPI/SerialCom/C_NApi_SerialCom.h \
    NeatoAPI/SerialCom/C_NApi_SerialCom_Protocol.h \
    NeatoAPI/SerialCom/C_NApi_SerialCom_Settings.h \
    NeatoAPI/SocketComRplidar/C_NApi_SocketComRplidar.h \
    NeatoAPI/SocketComRplidar/C_NApi_SocketComRplidar_Protocol.h \
    Tools/C_Tools_DataTable.h

SOURCES += \
        main.cpp \
        C_Main.cpp \
    NeatoAPI/Command/C_NApi_Command.cpp \
    NeatoAPI/CoreSlam/C_NApi_CoreSlam_8bppMap.cpp \
    NeatoAPI/Sensors/C_NApi_Sensors.cpp \
    NeatoAPI/SerialCom/C_NApi_SerialCom.cpp \
    NeatoAPI/SerialCom/C_NApi_SerialCom_Protocol.cpp \
    NeatoAPI/SerialCom/C_NApi_SerialCom_Settings.cpp \
    NeatoAPI/SocketComRplidar/C_NApi_SocketComRplidar.cpp \
    NeatoAPI/SocketComRplidar/C_NApi_SocketComRplidar_Protocol.cpp \
    Tools/C_Tools_DataTable.cpp
