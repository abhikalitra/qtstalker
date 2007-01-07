TEMPLATE = lib

CONFIG += thread

docs.path = /usr/share/doc/qtstalker/html
docs.files = *.html
INSTALLS += docs

pics.path = /usr/share/doc/qtstalker/html
pics.files = *.png
INSTALLS += pics

# install the translation files
i18n.path = /usr/share/qtstalker/i18n
i18n.files = ../i18n/*.qm
INSTALLS += i18n

# install the indicator files
indicator.path = /usr/share/qtstalker/indicator
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


