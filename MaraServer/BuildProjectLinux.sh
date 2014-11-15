#!/bin/bash
qmake -project -t app -o MaraServer.pro "CONFIG += thread" "QT += core gui network script" "DESTDIR=../bin" "INCLUDEPATH += ../MaraLib/include;./include" "UI_HEADERS_DIR = ./include/ui" "UI_SOURCES_DIR = ./src" "CONFIG(debug, debug|release) { LIBS += ../lib/libMaraLibD.so }" "CONFIG(debug, debug|release) { DEFINES += __DEBUG__ }" "CONFIG(release, debug|release) { LIBS += ../lib/libMaraLib.so }"
qmake
