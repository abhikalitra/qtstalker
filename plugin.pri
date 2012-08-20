include(config.pri)

LIBS += -lqwt5
LIBS += -L$${OTA_LIB} -lOTA
LIBS += -L$${TA_LIB} -lta_func_cdr -lta_common_cdr -lta_abstract_cdr -lta_libc_cdr
