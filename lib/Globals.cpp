#include "Globals.h"

QString g_session;
QString g_settingsFile;
BarData * g_barData = 0;
MiddleMan * g_middleMan = 0;
QMutex g_middleManMutex;
