#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T00:34:10
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = UltraSmart
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    SmartPointer.cpp

HEADERS += \
    SmartPointer.h \
    StaticLogic.h

QMAKE_CXXFLAGS += -std=c++0x
#DEFINES += __GXX_EXPERIMENTAL_CXX0X__
CONFIG += c++11
