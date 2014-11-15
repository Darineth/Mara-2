#!/bin/bash
qmake -project -t lib -o MaraPlugin.pro -after "CONFIG += debug_and_release core thread dll" "QT += network script" "DESTDIR=../lib" "INCLUDEPATH += ./include;../MaraLib/include;../MaraClient/include" "TARGET = MaraPlugin" "CONFIG(release, debug|release) { TARGET = MaraPlugin }" "CONFIG(debug, debug|release) { TARGET = MaraPluginD }" "CONFIG(debug, debug|release) { LIBS += -L../lib -lMaraLibD -lMaraClientD }" "DEFINES += __MARAPLUGINDLL__" "CONFIG(debug, debug|release) { DEFINES += __DEBUG__ }" "CONFIG(release, debug|release) { LIBS += -L../lib -lMaraLib -lMaraClient }"
qmake
