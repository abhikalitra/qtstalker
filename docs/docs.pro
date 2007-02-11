TEMPLATE = lib

CONFIG += thread

# install the html files
docs.path = /usr/local/share/doc/qtstalker/html
docs.files = *.html
INSTALLS += docs

# install the html pic files
pics.path = /usr/local/share/doc/qtstalker/html
pics.files = *.png
INSTALLS += pics

# install the CHANGELOG files
cl.path = /usr/local/share/doc/qtstalker/html
cl.files = CHANGELOG*
INSTALLS += cl

# install the translation files
i18n.path = /usr/local/share/qtstalker/i18n
i18n.files = ../i18n/*.qm
INSTALLS += i18n

# install the indicator files
indicator.path = /usr/local/share/qtstalker/indicator
indicator.files += ../misc/CUS_examples/B*
indicator.files += ../misc/CUS_examples/Candles
indicator.files += ../misc/CUS_examples/D*
indicator.files += ../misc/CUS_examples/O*
indicator.files += ../misc/CUS_examples/P*
indicator.files += ../misc/CUS_examples/R*
indicator.files += ../misc/CUS_examples/S*
indicator.files += ../misc/CUS_examples/V*
indicator.files += ../misc/CUS_examples/cdl-*
INSTALLS += indicator


