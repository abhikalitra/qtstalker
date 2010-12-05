#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <QString>
#include <QHash>

#include "BarData.h"
#include "Indicator.h"

extern QString g_session;
extern BarData * g_barData;
extern QHash<QString, Indicator *> g_indicators;

#endif
