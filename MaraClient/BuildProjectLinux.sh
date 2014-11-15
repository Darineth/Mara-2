#!/bin/bash
qmake -project -t lib -o MaraClient.pro -after "CONFIG += debug_and_release dll" "QT += core network" "DESTDIR=../lib" "INCLUDEPATH += ./include;../MaraLib/include" "TEMPLATE = lib" "TARGET = MaraClient" "CONFIG(release, debug|release) { TARGET = MaraClient }" "CONFIG(debug, debug|release) { TARGET = MaraClientD }" "win32:DEFINES += __MARACLIENTDLL__" "CONFIG(debug, debug|release) { DEFINES += __DEBUG__ }" "CONFIG(debug, debug|release) { LIBS += -L../lib/ -lMaraLibD }" "CONFIG(release, debug|release) { LIBS += -L../lib/ -lMaraLib }"
qmake

