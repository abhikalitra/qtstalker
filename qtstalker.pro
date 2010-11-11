exists(.qmake.cache) {
  message("Using the existing .qmake.cache")
} else {
  message("Doing automated configuration ...")

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

  message("Initial INCLUDEPATH=$$INCLUDEPATH")
  message("Initial LIBS=$$LIBS")

  !exists(qtstalker.config) {
    error("Missing configuration file qtstalker.config")
  } else {
    message("Including qtstalker.config")
    include(qtstalker.config)
  }

  TA_LIB_VERSION = $$system(ta-lib-config --version)
  contains(TA_LIB_VERSION, 0.3.0) {
    LIBS += -lta_abstract
    LIBS += -lta_common
    LIBS += -lta_func
  } else {
    LIBS += -lta_lib
  }

  INCLUDEPATH = $$unique(INCLUDEPATH)
  LIBS = $$unique(LIBS)

  message("Writing .qmake.cache")
  system(echo "INCLUDEPATH=$$INCLUDEPATH" > .qmake.cache)
  system(echo "LIBS=$$LIBS" >> .qmake.cache)
  system(echo "MY_OS=$$MY_OS" >> .qmake.cache)
  system(echo "INSTALL_BIN_DIR=$$INSTALL_BIN_DIR" >> .qmake.cache)
  system(echo "INSTALL_DATA_DIR=$$INSTALL_DATA_DIR" >> .qmake.cache)
  system(echo "INSTALL_DOCS_DIR=$$INSTALL_DOCS_DIR" >> .qmake.cache)
  system(echo "INSTALL_LIB_DIR=$$INSTALL_LIB_DIR" >> .qmake.cache)

  message("Writing lib/qtstalker_defines.h")
  system(echo \"$${LITERAL_HASH}ifndef QTSTALKER_DEFINES_H\" > lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define QTSTALKER_DEFINES_H\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define INSTALL_BIN_DIR \\\"$$INSTALL_BIN_DIR\\\"\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define INSTALL_DATA_DIR \\\"$$INSTALL_DATA_DIR\\\"\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define INSTALL_DOCS_DIR \\\"$$INSTALL_DOCS_DIR\\\"\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define INSTALL_LIB_DIR \\\"$$INSTALL_LIB_DIR\\\"\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}endif\" >> lib/qtstalker_defines.h)
}

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")
message("Operating system: $$MY_OS")
!isEmpty(QMAKESPEC) {
  message("QMAKESPEC: $$QMAKESPEC")
}

TEMPLATE = subdirs

# compile qtstalker lib
SUBDIRS += lib

# compile app
SUBDIRS += src

# compile indicator plugins
SUBDIRS += plugins/indicators/AD
SUBDIRS += plugins/indicators/ADX
SUBDIRS += plugins/indicators/AROON
SUBDIRS += plugins/indicators/ATR
SUBDIRS += plugins/indicators/BARS
SUBDIRS += plugins/indicators/BBANDS
SUBDIRS += plugins/indicators/BETA
SUBDIRS += plugins/indicators/BOP
SUBDIRS += plugins/indicators/CANDLES
SUBDIRS += plugins/indicators/CCI
SUBDIRS += plugins/indicators/CMO
SUBDIRS += plugins/indicators/CORREL
SUBDIRS += plugins/indicators/CUS
SUBDIRS += plugins/indicators/FI
SUBDIRS += plugins/indicators/HT
SUBDIRS += plugins/indicators/LINEARREG
SUBDIRS += plugins/indicators/MA
SUBDIRS += plugins/indicators/MACD
SUBDIRS += plugins/indicators/MAMA
SUBDIRS += plugins/indicators/MAVP
SUBDIRS += plugins/indicators/MFI
SUBDIRS += plugins/indicators/MOM
SUBDIRS += plugins/indicators/PO
SUBDIRS += plugins/indicators/PP
SUBDIRS += plugins/indicators/ROC
SUBDIRS += plugins/indicators/RSI
SUBDIRS += plugins/indicators/SAR
SUBDIRS += plugins/indicators/STOCH
SUBDIRS += plugins/indicators/STOCHRSI
SUBDIRS += plugins/indicators/STOCHS
SUBDIRS += plugins/indicators/SZ
SUBDIRS += plugins/indicators/T3
SUBDIRS += plugins/indicators/THERM
SUBDIRS += plugins/indicators/TRIX
SUBDIRS += plugins/indicators/ULTOSC
SUBDIRS += plugins/indicators/VAR
SUBDIRS += plugins/indicators/VFI
SUBDIRS += plugins/indicators/VIDYA
SUBDIRS += plugins/indicators/VOL
SUBDIRS += plugins/indicators/WILLR

# compile misc plugins
SUBDIRS += plugins/misc/Alert
SUBDIRS += plugins/misc/CSV
SUBDIRS += plugins/misc/Scanner
SUBDIRS += plugins/misc/Script
SUBDIRS += plugins/misc/Tester
SUBDIRS += plugins/misc/Yahoo

# compile script plugins
SUBDIRS += plugins/script/CO
SUBDIRS += plugins/script/GROUP
SUBDIRS += plugins/script/INDICATOR
SUBDIRS += plugins/script/PLUGIN
SUBDIRS += plugins/script/PROCESS
SUBDIRS += plugins/script/SYMBOL

# compile QuoteServer
SUBDIRS += QuoteServer

# install docs and i18n
SUBDIRS += docs
