#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <QMutex>
#include <QString>

#include "BarData.h"

extern QMutex g_mutex;
extern QString g_session;
extern BarData g_barData;

#endif
