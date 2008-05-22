TEMPLATE = lib

CONFIG += thread

# install the example indicator html files and pics
idocs.path = /usr/local/share/doc/qtstalker/html/indicator
idocs.files = indicator/*.html
idocs.files += indicator/*.png
INSTALLS += idocs

# install the past CHANGELOG files
pcl.path = /usr/local/share/doc/qtstalker/html/pastchanges
pcl.files = pastchanges/*.html
pcl.files += pastchanges/CHANGELOG*
INSTALLS += pcl

# install the html files and pics
docs.path = /usr/local/share/doc/qtstalker/html
docs.files = *.html *.png CHANGELOG*
INSTALLS += docs

# install the license file
lic.path = /usr/local/share/doc/qtstalker
lic.files = COPYING
INSTALLS += lic

# install the translation files
i18n.path = /usr/local/share/qtstalker/i18n
i18n.files = ../i18n/*.qm
INSTALLS += i18n

# install the indicator files
indicator.path = /usr/local/share/qtstalker/indicator
# only install some explicit examples
indicator.files = ../misc/CUS_examples/bar
indicator.files += ../misc/CUS_examples/cdl-rel
indicator.files += ../misc/CUS_examples/cdl-rel-ma
indicator.files += ../misc/CUS_examples/RSI
indicator.files += ../misc/CUS_examples/STOCH
indicator.files += ../misc/CUS_examples/VOL
INSTALLS += indicator


