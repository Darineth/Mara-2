#!/bin/bash
qmake -project -t lib -o MaraLib.pro -after "CONFIG += debug_and_release dll" "QT += core network webkit" "DESTDIR=../lib" "INCLUDEPATH += ./include" "TEMPLATE = lib" "TARGET = MaraLib" "CONFIG(release, debug|release) { TARGET = MaraLib }" "CONFIG(debug, debug|release) { TARGET = MaraLibD }" "CONFIG(debug, debug|release) { DEFINES += __DEBUG__ }"
qmake
