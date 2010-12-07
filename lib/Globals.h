#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <QString>
#include <QHash>

#include "BarData.h"
#include "Indicator.h"
#include "ChartPage.h"
#include "GroupPage.h"
#include "ScriptPage.h"

extern QString g_session;
extern BarData * g_barData;
extern QHash<QString, Indicator *> g_indicators;
extern ChartPage * g_chartPanel;
extern GroupPage * g_groupPanel;
extern ScriptPage * g_scriptPanel;

#endif
