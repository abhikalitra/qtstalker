TEMPLATE = lib

CONFIG += thread

# install the system script files
script.path = "$${INSTALL_DATA_DIR}/qtstalker/script"
script.files = ../Scripts/*.pl
INSTALLS += script
