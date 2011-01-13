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

# compile script plugins
SUBDIRS += plugins/script/AD
SUBDIRS += plugins/script/ADX
SUBDIRS += plugins/script/ADXR
SUBDIRS += plugins/script/AROON
SUBDIRS += plugins/script/ATR
SUBDIRS += plugins/script/BARS
SUBDIRS += plugins/script/BBANDS
SUBDIRS += plugins/script/BETA
SUBDIRS += plugins/script/BOP
SUBDIRS += plugins/script/CANDLES
SUBDIRS += plugins/script/CCI
SUBDIRS += plugins/script/CHART_OBJECT
SUBDIRS += plugins/script/CHART_OBJECT_BUY
SUBDIRS += plugins/script/CHART_OBJECT_BUY_DIALOG
SUBDIRS += plugins/script/CHART_OBJECT_DATABASE
SUBDIRS += plugins/script/CHART_OBJECT_HLINE
SUBDIRS += plugins/script/CHART_OBJECT_HLINE_DIALOG
SUBDIRS += plugins/script/CHART_OBJECT_RETRACEMENT
SUBDIRS += plugins/script/CHART_OBJECT_RETRACEMENT_DIALOG
SUBDIRS += plugins/script/CHART_OBJECT_SELL
SUBDIRS += plugins/script/CHART_OBJECT_SELL_DIALOG
SUBDIRS += plugins/script/CHART_OBJECT_TEXT
SUBDIRS += plugins/script/CHART_OBJECT_TEXT_DIALOG
SUBDIRS += plugins/script/CHART_OBJECT_TLINE
SUBDIRS += plugins/script/CHART_OBJECT_TLINE_DIALOG
SUBDIRS += plugins/script/CHART_OBJECT_VLINE
SUBDIRS += plugins/script/CHART_OBJECT_VLINE_DIALOG
SUBDIRS += plugins/script/CHART_PANEL
SUBDIRS += plugins/script/CMO
SUBDIRS += plugins/script/COLOR_DIALOG
SUBDIRS += plugins/script/CONFIGURE_DIALOG
SUBDIRS += plugins/script/CONFIRM_DIALOG
SUBDIRS += plugins/script/CORREL
SUBDIRS += plugins/script/DATA_WINDOW
SUBDIRS += plugins/script/FI
SUBDIRS += plugins/script/FONT_DIALOG
SUBDIRS += plugins/script/GROUP_DATABASE
SUBDIRS += plugins/script/GROUP_EDIT_DIALOG
SUBDIRS += plugins/script/GROUP_PANEL
SUBDIRS += plugins/script/HT
SUBDIRS += plugins/script/HT_PHASOR
SUBDIRS += plugins/script/HT_SINE
SUBDIRS += plugins/script/INDICATOR_DATABASE
SUBDIRS += plugins/script/INDICATOR_EDIT_DIALOG
SUBDIRS += plugins/script/INDICATOR_NEW_DIALOG
SUBDIRS += plugins/script/INDICATOR_PLOT
SUBDIRS += plugins/script/INDICATOR_PLOT_COLOR
SUBDIRS += plugins/script/INDICATOR_PLOT_INDEX
SUBDIRS += plugins/script/INDICATOR_PLOT_NEW
SUBDIRS += plugins/script/INDICATOR_PLOT_STYLE
SUBDIRS += plugins/script/INDICATOR_RUN
SUBDIRS += plugins/script/LINEARREG
SUBDIRS += plugins/script/MA
SUBDIRS += plugins/script/MACD
SUBDIRS += plugins/script/MAMA
SUBDIRS += plugins/script/MAVP
SUBDIRS += plugins/script/MDI
SUBDIRS += plugins/script/MESSAGE_DIALOG
SUBDIRS += plugins/script/MFI
SUBDIRS += plugins/script/MOM
SUBDIRS += plugins/script/NEW_DIALOG
SUBDIRS += plugins/script/NORMALIZE
SUBDIRS += plugins/script/PDI
SUBDIRS += plugins/script/PLOT
SUBDIRS += plugins/script/PO
SUBDIRS += plugins/script/PROCESS
SUBDIRS += plugins/script/QUOTE_DATABASE
SUBDIRS += plugins/script/ROC
SUBDIRS += plugins/script/RSI
SUBDIRS += plugins/script/SAR
SUBDIRS += plugins/script/SCRIPT_DATABASE
SUBDIRS += plugins/script/SCRIPT_EDIT_DIALOG
SUBDIRS += plugins/script/SCRIPT_PANEL
SUBDIRS += plugins/script/SELECT_DIALOG
SUBDIRS += plugins/script/SETTINGS
SUBDIRS += plugins/script/STOCH_FAST
SUBDIRS += plugins/script/STOCH_RSI
SUBDIRS += plugins/script/STOCH_SLOW
SUBDIRS += plugins/script/SYMBOL
SUBDIRS += plugins/script/SYMBOL_DIALOG
SUBDIRS += plugins/script/SZ
SUBDIRS += plugins/script/T3
SUBDIRS += plugins/script/THERM
SUBDIRS += plugins/script/TRIX
SUBDIRS += plugins/script/ULTOSC
SUBDIRS += plugins/script/VAR
SUBDIRS += plugins/script/VFI
SUBDIRS += plugins/script/VIDYA
SUBDIRS += plugins/script/WILLR
SUBDIRS += plugins/script/YAHOO_DATABASE
SUBDIRS += plugins/script/YAHOO_DIALOG
SUBDIRS += plugins/script/YAHOO_HISTORY


# install docs and i18n
SUBDIRS += docs
