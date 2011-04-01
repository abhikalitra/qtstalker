#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <QString>
#include <QWidget>

#include "BarData.h"
#include "MiddleMan.h"

extern QString g_session;
extern QString g_localSettings;
extern QString g_globalSettings;
extern BarData * g_barData;
extern MiddleMan * g_middleMan;
extern QWidget *g_parent;

#endif
