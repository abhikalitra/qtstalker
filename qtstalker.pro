TEMPLATE = subdirs

#compile qtstalker lib
SUBDIRS += lib

#compile app
SUBDIRS += src

#compile quote plugins
SUBDIRS += plugins/quote/CME
SUBDIRS += plugins/quote/CSV
SUBDIRS += plugins/quote/NYBOT
SUBDIRS += plugins/quote/QtstalkerFormat
SUBDIRS += plugins/quote/Yahoo

#compile indicator plugins
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
SUBDIRS += plugins/indicator/UO
SUBDIRS += plugins/indicator/UTIL
SUBDIRS += plugins/indicator/VFI
SUBDIRS += plugins/indicator/VIDYA
SUBDIRS += plugins/indicator/VOL
SUBDIRS += plugins/indicator/VOLA

#compile db plugins
SUBDIRS += plugins/db/Stocks
SUBDIRS += plugins/db/Futures
SUBDIRS += plugins/db/Spread
SUBDIRS += plugins/db/Index
SUBDIRS += plugins/db/CC

#compile co plugins
SUBDIRS += plugins/co/BuyArrow
SUBDIRS += plugins/co/SellArrow
SUBDIRS += plugins/co/HorizontalLine
SUBDIRS += plugins/co/VerticalLine
SUBDIRS += plugins/co/Text
SUBDIRS += plugins/co/TrendLine
SUBDIRS += plugins/co/FiboLine
SUBDIRS += plugins/co/Cycle

SUBDIRS += docs
