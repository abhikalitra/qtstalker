#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <QString>
#include "Bars.h"

QTSTALKER_SHARED_EXPORT extern QString g_session;
QTSTALKER_SHARED_EXPORT extern QString g_settings;
QTSTALKER_SHARED_EXPORT extern QString g_title;
QTSTALKER_SHARED_EXPORT extern Bars *g_symbol;

#define MAX_SIZE 1024

#endif
