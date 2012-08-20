TEMPLATE = subdirs

include (config.pri)

message("Initial INCLUDEPATH=$$INCLUDEPATH")
message("Initial LIBS=$$LIBS")

!exists(qtstalker.config) {
    error("Missing configuration file qtstalker.config")
} else {
    message("Including qtstalker.config")
    include(qtstalker.config)
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

!win32 {
  system(echo \"$${LITERAL_HASH}ifndef QTSTALKER_DEFINES_H\" > lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define QTSTALKER_DEFINES_H\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define INSTALL_BIN_DIR \\\"$$INSTALL_BIN_DIR\\\"\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define INSTALL_DATA_DIR \\\"$$INSTALL_DATA_DIR\\\"\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define INSTALL_DOCS_DIR \\\"$$INSTALL_DOCS_DIR\\\"\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}define INSTALL_LIB_DIR \\\"$$INSTALL_LIB_DIR\\\"\" >> lib/qtstalker_defines.h)
  system(echo \"$${LITERAL_HASH}endif\" >> lib/qtstalker_defines.h)
}

win32 {
  system(echo $${LITERAL_HASH}ifndef QTSTALKER_DEFINES_H > lib/qtstalker_defines.h)
  system(echo $${LITERAL_HASH}define QTSTALKER_DEFINES_H >> lib/qtstalker_defines.h)
  system(echo $${LITERAL_HASH}define INSTALL_BIN_DIR \"$$INSTALL_BIN_DIR\" >> lib/qtstalker_defines.h)
  system(echo $${LITERAL_HASH}define INSTALL_DATA_DIR \"$$INSTALL_DATA_DIR\" >> lib/qtstalker_defines.h)
  system(echo $${LITERAL_HASH}define INSTALL_DOCS_DIR \"$$INSTALL_DOCS_DIR\" >> lib/qtstalker_defines.h)
  system(echo $${LITERAL_HASH}define INSTALL_LIB_DIR \"$$INSTALL_LIB_DIR\" >> lib/qtstalker_defines.h)
  system(echo $${LITERAL_HASH}endif >> lib/qtstalker_defines.h)
}


message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")
message("Operating system: $$MY_OS")
!isEmpty(QMAKESPEC) {
  message("QMAKESPEC: $$QMAKESPEC")
}



# compile lib
SUBDIRS += lib

# compile app
SUBDIRS += src

# compile plugins
SUBDIRS += plugins/ADX
SUBDIRS += plugins/AROON
SUBDIRS += plugins/BBANDS
SUBDIRS += plugins/CCI
SUBDIRS += plugins/CDL
SUBDIRS += plugins/CSV
SUBDIRS += plugins/CurveHistogram
SUBDIRS += plugins/CurveLine
SUBDIRS += plugins/CurveOHLC
SUBDIRS += plugins/DBStock
SUBDIRS += plugins/DBSymbol
SUBDIRS += plugins/Group
SUBDIRS += plugins/Indicator
SUBDIRS += plugins/MACD
SUBDIRS += plugins/MarkerBuy
SUBDIRS += plugins/MarkerHLine
SUBDIRS += plugins/MarkerRetracement
SUBDIRS += plugins/MarkerSell
SUBDIRS += plugins/MarkerText
SUBDIRS += plugins/MarkerTLine
SUBDIRS += plugins/MarkerVLine
SUBDIRS += plugins/MOM
SUBDIRS += plugins/OHLC
SUBDIRS += plugins/ROC
SUBDIRS += plugins/RSI
SUBDIRS += plugins/SAR
#SUBDIRS += plugins/Script
#SUBDIRS += plugins/SCRIPT_ADD
#SUBDIRS += plugins/SCRIPT_DIV
#SUBDIRS += plugins/SCRIPT_MA
#SUBDIRS += plugins/SCRIPT_MULT
#SUBDIRS += plugins/SCRIPT_PLOT_HISTOGRAM
#SUBDIRS += plugins/SCRIPT_PLOT_LINE
#SUBDIRS += plugins/SCRIPT_PLOT_OHLC
#SUBDIRS += plugins/SCRIPT_SUB
SUBDIRS += plugins/STOCHF
SUBDIRS += plugins/STOCHS
SUBDIRS += plugins/Volume
SUBDIRS += plugins/YahooHistory

# install docs and i18n
#SUBDIRS += docs
