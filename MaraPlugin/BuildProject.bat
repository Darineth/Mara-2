qmake -project -t $$TEMPLATE -o MaraPlugin.pro -after "CONFIG += core thread dll" "QT += network script" "DESTDIR=../lib" "INCLUDEPATH += ./include;../MaraLib/include;../MaraClient/include" "TEMPLATE = vclib" "TARGET = MaraPlugin" "CONFIG(release, debug|release) { TARGET = MaraPlugin }" "CONFIG(debug, debug|release) { TARGET = $$join(TARGET,,,D) }" "CONFIG(debug, debug|release) { LIBS += ../lib/MaraLibD.lib }" "DEFINES += __MARAPLUGINDLL__" "CONFIG(debug, debug|release) { DEFINES += __DEBUG__ }" "CONFIG(release, debug|release) { LIBS += ../lib/MaraLib.lib ../lib/MaraClient.lib }" "win32:MAKEFILE = MaraPlugin" "QMAKE_POST_LINK = copy $(TargetDir)$(TargetName).dll $(TargetDir)..\bin"
qmake -spec win32-msvc2010
