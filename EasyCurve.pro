QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    plotlayout.cpp \
    sppchip.cpp

HEADERS += \
    mainwindow.h \
    plotlayout.h \
    sppchip.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    EasyCurve_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -LF:/Qt/Qwt-6.2.0/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -LF:/Qt/Qwt-6.2.0/lib/ -lqwtd
else:unix: LIBS += -LF:/Qt/Qwt-6.2.0/lib/ -lqwt

INCLUDEPATH += F:/Qt/Qwt-6.2.0/include
DEPENDPATH += F:/Qt/Qwt-6.2.0/include
