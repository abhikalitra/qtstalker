TEMPLATE = subdirs

#compile qtstalker lib
SUBDIRS += lib

#compile app
SUBDIRS += src

#compile chart plugins
SUBDIRS += plugins/chart/Bar
SUBDIRS += plugins/chart/Candle
SUBDIRS += plugins/chart/EP
SUBDIRS += plugins/chart/Line
SUBDIRS += plugins/chart/PF
SUBDIRS += plugins/chart/Swing

#compile quote plugins
SUBDIRS += plugins/quote/CME
SUBDIRS += plugins/quote/CSV
SUBDIRS += plugins/quote/NYBOT
SUBDIRS += plugins/quote/QtstalkerFormat
SUBDIRS += plugins/quote/Yahoo

#compile indicator plugins
SUBDIRS += plugins/indicator/AD
SUBDIRS += plugins/indicator/AROON
SUBDIRS += plugins/indicator/ATR
SUBDIRS += plugins/indicator/BB
SUBDIRS += plugins/indicator/CCI
SUBDIRS += plugins/indicator/CMO
SUBDIRS += plugins/indicator/CUS
SUBDIRS += plugins/indicator/DMI
SUBDIRS += plugins/indicator/DPO
SUBDIRS += plugins/indicator/FI
SUBDIRS += plugins/indicator/LMS
SUBDIRS += plugins/indicator/MA
SUBDIRS += plugins/indicator/MACD
SUBDIRS += plugins/indicator/MF
SUBDIRS += plugins/indicator/MINMAX
SUBDIRS += plugins/indicator/MOM
SUBDIRS += plugins/indicator/PP
SUBDIRS += plugins/indicator/RSI
SUBDIRS += plugins/indicator/SAR
SUBDIRS += plugins/indicator/SINWAV
SUBDIRS += plugins/indicator/STOCH
SUBDIRS += plugins/indicator/SYMBOL
SUBDIRS += plugins/indicator/SZ
SUBDIRS += plugins/indicator/THERM
SUBDIRS += plugins/indicator/TRIX
SUBDIRS += plugins/indicator/UO
SUBDIRS += plugins/indicator/UTIL
SUBDIRS += plugins/indicator/VFI
SUBDIRS += plugins/indicator/VIDYA
SUBDIRS += plugins/indicator/VOL
SUBDIRS += plugins/indicator/VOLA
SUBDIRS += plugins/indicator/VT
SUBDIRS += plugins/indicator/WILLR

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
