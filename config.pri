win32 {
    MY_OS += "win32"
    INCLUDEPATH += "C:/Qt/Qwt-5.2.2/include"
    INCLUDEPATH += "C:/workspace/ta-lib/c/include"
    LIBS += -L"C:/Qt/Qwt-5.2.2/lib"
    INSTALL_DOCS_DIR = docs

    OTA_LIB = "../../lib/debug"
    OTA_LIB1 = "../lib/debug"
    TA_LIB = "C:/workspace/ta-lib/c/lib"

    QMAKE_CXXFLAGS += /MP
}

unix {
    MY_OS += "unix"
    exists(/usr/include/qt4/Qt) {
      INCLUDEPATH += /usr/include/qt4/Qt
    }
    exists(/usr/include/ta-lib) {
      INCLUDEPATH += /usr/include/ta-lib
    }
    exists(/usr/local/include/ta-lib) {
      INCLUDEPATH += /usr/local/include/ta-lib
    }
    exists(/usr/local/lib) {
      LIBS += -L/usr/local/lib
    }
    # Determine Qwt for Ubuntu
    exists(/usr/include/qwt-qt4) {
      INCLUDEPATH += /usr/include/qwt-qt4
    }
    exists(/usr/lib/libqwt-qt4.so) {
      LIBS += -lqwt-qt4
    } else {
      LIBS += -lqwt
    }
}

freebsd-g++ {
    MY_OS += "freebsd"
    INCLUDEPATH += /usr/X11R6/include
}

linux-g++ {
    MY_OS += "linux"
}

macx {
    MY_OS += "macx"
    # For MacPorts
    exists(/opt/local/include/ta-lib) {
      INCLUDEPATH += /opt/local/include/ta-lib
    }
    exists(/opt/local/lib) {
      LIBS += -L/opt/local/lib
    }
    # For Fink
    exists(/sw/include/ta-lib) {
      INCLUDEPATH += /sw/include/ta-lib
    }
    exists(/sw/lib) {
      LIBS += -L/sw/lib
    }
}
