
exists(.qmake.cache) {
  message("Using .qmake.cache")
}
else {
  message("Doing automated configuration ...")

  unix {
    MY_OS += "unix"
    INCLUDEPATH += /usr/include/qt4/Qt
    exists(/usr/local/include/ta-lib) {
      INCLUDEPATH += /usr/local/include/ta-lib
    }
    exists(/usr/include/ta-lib) {
      INCLUDEPATH += /usr/include/ta-lib
    }
  }

  freebsd-g++ {
    MY_OS += "freebsd"
    INCLUDEPATH += /usr/X11R6/include
  }

  linux-g++ {
    MY_OS += "linux"
  }

  win32 {
    MY_OS += "windows"
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

  message("Using INCLUDEPATH=$$INCLUDEPATH")
  message("Using LIBS=$$LIBS")

  !exists(qtstalker.config) {
	error("Missing configuration file qtstalker.config")
  }
	include(qtstalker.config)
  
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
  win32 {
    system(echo $${LITERAL_HASH}ifndef QTSTALKER_DEFINES_H > lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define QTSTALKER_DEFINES_H >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_BIN_DIR \"$$INSTALL_BIN_DIR\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_DATA_DIR \"$$INSTALL_DATA_DIR\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_DOCS_DIR \"$$INSTALL_DOCS_DIR\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}define INSTALL_LIB_DIR \"$$INSTALL_LIB_DIR\" >> lib/qtstalker_defines.h)
    system(echo $${LITERAL_HASH}endif >> lib/qtstalker_defines.h)
  } else {
    system(echo \"$${LITERAL_HASH}ifndef QTSTALKER_DEFINES_H\" > lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define QTSTALKER_DEFINES_H\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_BIN_DIR \\\"$$INSTALL_BIN_DIR\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_DATA_DIR \\\"$$INSTALL_DATA_DIR\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_DOCS_DIR \\\"$$INSTALL_DOCS_DIR\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}define INSTALL_LIB_DIR \\\"$$INSTALL_LIB_DIR\\\"\" >> lib/qtstalker_defines.h)
    system(echo \"$${LITERAL_HASH}endif\" >> lib/qtstalker_defines.h)
  }
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

# compile CSV app
SUBDIRS += CSV

!win32 {
  # install docs and i18n
 # SUBDIRS += docs
}
