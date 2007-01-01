TEMPLATE = lib
 
CONFIG += thread warn_on

system(tar zxvf talib.tar.gz)
unix:linux-g++:system(cd ta-lib/c/make/cmr/linux/g++ ; make)
unix:freebsd-g++:system(cd ta-lib/c/make/cmr/freebsd/g++ ; make)

# for macx build problem
unix:macx-g++:system(cd ta-lib/c/make/cmr/freebsd/g++ ; make)