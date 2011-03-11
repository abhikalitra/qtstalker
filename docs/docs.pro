TEMPLATE = lib

CONFIG += thread

# install the example indicator html files and pics
idocs.path = "$${INSTALL_DOCS_DIR}/qtstalker/html/indicator"
idocs.files = indicator/*.html
idocs.files += indicator/*.png
INSTALLS += idocs

# install the past CHANGELOG files
pcl.path = "$${INSTALL_DOCS_DIR}/qtstalker/html/pastchanges"
pcl.files = pastchanges/*.html
pcl.files += pastchanges/CHANGELOG*
INSTALLS += pcl

# install the html files and pics
docs.path = "$${INSTALL_DOCS_DIR}/qtstalker/html"
docs.files = *.html *.png CHANGELOG*
docs.files += doc.qhc doc.qch doc-doxygen.qch
INSTALLS += docs

# install the license file
lic.path = "$${INSTALL_DOCS_DIR}/qtstalker"
lic.files = COPYING
INSTALLS += lic

# install the translation files
i18n.path = "$${INSTALL_DATA_DIR}/qtstalker/i18n"
i18n.files = ../i18n/*.qm
INSTALLS += i18n

# install the db update files
dbUpdate.path = "$${INSTALL_DATA_DIR}/qtstalker/db"
dbUpdate.files = *.csv
INSTALLS += dbUpdate

# install the alert script files
alert.path = "$${INSTALL_DATA_DIR}/qtstalker/alert"
alert.files = ../Scripts/alert/*.pl
INSTALLS += alert

# install the indicator script files
indicator.path = "$${INSTALL_DATA_DIR}/qtstalker/indicator"
indicator.files = ../Scripts/indicator/*.pl
INSTALLS += indicator

# install the quote script files
quote.path = "$${INSTALL_DATA_DIR}/qtstalker/quote"
quote.files = ../Scripts/quote/*.pl
INSTALLS += quote

# install the scanner script files
scanner.path = "$${INSTALL_DATA_DIR}/qtstalker/scanner"
scanner.files = ../Scripts/scanner/*.pl
INSTALLS += scanner

# install the system script files
system.path = "$${INSTALL_DATA_DIR}/qtstalker/system"
system.files = ../Scripts/system/*.pl
INSTALLS += system

# install the test script files
tester.path = "$${INSTALL_DATA_DIR}/qtstalker/test"
tester.files = ../Scripts/test/*.pl
INSTALLS += tester
