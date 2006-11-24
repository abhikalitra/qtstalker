TEMPLATE = subdirs

# compile qtstalker lib
SUBDIRS += lib

# compile app
SUBDIRS += src

# compile quote plugins
SUBDIRS += plugins/quote/CME
SUBDIRS += plugins/quote/CSV
SUBDIRS += plugins/quote/NYBOT
SUBDIRS += plugins/quote/Yahoo

# compile indicator plugins
SUBDIRS += plugins/indicator/BARS
SUBDIRS += plugins/indicator/CUS
SUBDIRS += plugins/indicator/FI
SUBDIRS += plugins/indicator/LMS
SUBDIRS += plugins/indicator/LOWPASS
SUBDIRS += plugins/indicator/PP
SUBDIRS += plugins/indicator/SINWAV
SUBDIRS += plugins/indicator/SYMBOL
SUBDIRS += plugins/indicator/SZ
SUBDIRS += plugins/indicator/TALIB
SUBDIRS += plugins/indicator/THERM
SUBDIRS += plugins/indicator/UTIL
SUBDIRS += plugins/indicator/VFI
SUBDIRS += plugins/indicator/VIDYA
SUBDIRS += plugins/indicator/VOL
SUBDIRS += plugins/indicator/VOLA

# install docs and i18n
SUBDIRS += docs
