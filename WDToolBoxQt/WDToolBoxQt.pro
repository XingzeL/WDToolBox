QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Infrastructure layer
SOURCES += \
    src/infrastructure/Observer.cpp \
    src/infrastructure/ConfigReader.cpp \
    src/infrastructure/Executor.cpp \
    src/infrastructure/ProcessLauncher.cpp \
    src/infrastructure/ConfigFileWatcher.cpp \
    src/infrastructure/WorkLogWriter.cpp

HEADERS += \
    src/infrastructure/Observer.h \
    src/infrastructure/ConfigReader.h \
    src/infrastructure/Executor.h \
    src/infrastructure/ProcessLauncher.h \
    src/infrastructure/ConfigFileWatcher.h \
    src/infrastructure/WorkLogWriter.h

# Core layer
SOURCES += \
    src/core/ToolManager.cpp \
    src/core/WorkLogManager.cpp

HEADERS += \
    src/core/ToolInfo.h \
    src/core/ToolManager.h \
    src/core/LogLibraryInfo.h \
    src/core/WorkLogManager.h

# UI layer
SOURCES += \
    src/ui/ToolManagerPage.cpp \
    src/ui/WorkLogPage.cpp \
    src/ui/MainWidget.cpp \
    src/ui/WriteWorkLogDialog.cpp

HEADERS += \
    src/ui/TabPageBase.h \
    src/ui/ToolManagerPage.h \
    src/ui/WorkLogPage.h \
    src/ui/MainWidget.h \
    src/ui/WriteWorkLogDialog.h

# Main entry
SOURCES += \
    main.cpp

TRANSLATIONS += \
    WDToolBoxQt_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
