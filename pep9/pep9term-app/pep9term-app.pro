# Project created by Matthew McRaven, 12/30/2018
# -------------------------------------------------
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Console application specific configuration.
# QT -= gui
CONFIG += c++17 console

TARGET = Pep9Term
#Prevent Windows from trying to parse the project three times per build.
CONFIG -= debug_and_release \
    debug_and_release_target
#Flag for enabling C++17 features.
#Due to support for C++17 features being added before the standard was finalized, and the placeholder text of "C++1z" has remained
CONFIG += c++17
win32{
    #MSVC doesn't recognize c++1z flag, so use the MSVC specific flag here
    win32-msvc*: QMAKE_CXXFLAGS += /std:c++17
    #Flags needed to generate PDB information in release. Necessary information to profile program.
    #Flags also picked to provide a ~15% speed increase in release mode (at the cost of increased compile times).
    QMAKE_LFLAGS_RELEASE +=/MAP
    QMAKE_CFLAGS_RELEASE += /MD /zi
    QMAKE_LFLAGS_RELEASE +=/debug /opt:ref
}

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    asmbuildhelper.h \
    asmrunhelper.h \
    boundexecmicrocpu.h \
    cpubuildhelper.h \
    cpurunhelper.h \
    CLI11.hpp \
    microstephelper.h \
    termformatter.h \
    termhelper.h \
    boundexecisacpu.h

RESOURCES += \
    ../../pep-core/common/common-helpresources.qrc \
    ../../pep-core/common/common-resources.qrc \
    ../pep9cpu-lib/pep9cpu-helpresources.qrc \
    ../pep9cpu-lib/pep9cpu-resources.qrc \
    ../pep9asm-lib/pep9asm-helpresources.qrc \
    ../pep9asm-lib/pep9asm-resources.qrc \
    ../pep9micro-lib/pep9micro-helpresources.qrc \
    ../pep9micro-lib/pep9micro-resources.qrc \
    pep9term-helpresources.qrc \
    pep9term-resources.qrc

DISTFILES += \
    help-term/about.txt

SOURCES += \
    asmbuildhelper.cpp \
    asmrunhelper.cpp \
    boundexecmicrocpu.cpp \
    cpubuildhelper.cpp \
    cpurunhelper.cpp \
    microstephelper.cpp \
    termhelper.cpp \
    boundexecisacpu.cpp \
    termmain.cpp

# Mac icon/plist
ICON = images/icon.icns
QMAKE_INFO_PLIST = app.plist

#Windows RC file for icon:
RC_FILE = pep9resources.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#Add this include to the bottom of your project to enable automated installer creation
#Include the definitions file that sets all variables needed for the InstallerConfig Script
include("installer-config.pri")

#Include and run the installer config script
include("../../installer/installer-creator.pri")

# Link against special libraries needed for dark mode.
include(../../mac-objc-fix.pri)

# Link against Pep core code
unix|win32: LIBS += -L$$OUT_PWD/../../pep-core/common/ -lpep-core-common

INCLUDEPATH += $$PWD/../../pep-core/common
DEPENDPATH += $$PWD/../../pep-core/common

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../pep-core/common/pep-core-common.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../pep-core/common/libpep-core-common.a

unix|win32: LIBS += -L$$OUT_PWD/../../pep-core/cpu/ -lpep-core-cpu

INCLUDEPATH += $$PWD/../../pep-core/cpu
DEPENDPATH += $$PWD/../../pep-core/cpu

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../pep-core/cpu/pep-core-cpu.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../pep-core/cpu/libpep-core-cpu.a

unix|win32: LIBS += -L$$OUT_PWD/../../pep-core/asm/ -lpep-core-asm

INCLUDEPATH += $$PWD/../../pep-core/asm
DEPENDPATH += $$PWD/../../pep-core/asm

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../pep-core/asm/pep-core-asm.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../pep-core/asm/libpep-core-asm.a


# Link against shared code for Pep9.
unix|win32: LIBS += -L$$OUT_PWD/../pep9def-lib/ -lpep9def-lib

INCLUDEPATH += $$PWD/../pep9def-lib
DEPENDPATH += $$PWD/../pep9def-lib

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../pep9def-lib/pep9def-lib.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../pep9def-lib/libpep9def-lib.a

unix|win32: LIBS += -L$$OUT_PWD/../pep9cpu-lib/ -lpep9cpu-lib

INCLUDEPATH += $$PWD/../pep9cpu-lib
DEPENDPATH += $$PWD/../pep9cpu-lib

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../pep9cpu-lib/pep9cpu-lib.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../pep9cpu-lib/libpep9cpu-lib.a

unix|win32: LIBS += -L$$OUT_PWD/../pep9asm-lib/ -lpep9asm-lib

INCLUDEPATH += $$PWD/../pep9asm-lib
DEPENDPATH += $$PWD/../pep9asm-lib

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../pep9asm-lib/pep9asm-lib.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../pep9asm-lib/libpep9asm-lib.a

unix|win32: LIBS += -L$$OUT_PWD/../pep9micro-lib/ -lpep9micro-lib

INCLUDEPATH += $$PWD/../pep9micro-lib
DEPENDPATH += $$PWD/../pep9micro-lib

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../pep9micro-lib/pep9micro-lib.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../pep9micro-lib/libpep9micro-lib.a
