QT -= gui
QT += xml

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        headerfooterodt.cpp \
        imageodt.cpp \
        listodt.cpp \
        main.cpp \
        pageodt.cpp \
        tableelement.cpp \
        tableodt.cpp \
        textodt.cpp \
        writerodt.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    headerfooterodt.h \
    imageodt.h \
    listodt.h \
    pageodt.h \
    tableelement.h \
    tableodt.h \
    textodt.h \
    writerodt.h
