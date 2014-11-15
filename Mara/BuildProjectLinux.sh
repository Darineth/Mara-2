#!/bin/bash
qmake -project -t app -o Mara.pro "CONFIG += debug_and_release" "QT += core gui network webkit" "DESTDIR=../bin" "INCLUDEPATH += ../MaraLib/include ../MaraClient/include ../MaraPlugin/include ./include" "UI_HEADERS_DIR = ./include/ui" "UI_SOURCES_DIR = ./src" "CONFIG(debug, debug|release){ LIBS += -L../lib/ -lMaraLibD -lMaraClientD -lMaraPluginD }" "CONFIG(release, debug|release) { LIBS += -L../lib/ -lMaraLib -lMaraClient -lMaraPlugin }" "ICON = ../icons/Mara2.icns"
qmake

