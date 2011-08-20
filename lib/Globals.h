#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <QString>
#include <QMainWindow>
#include <QSharedMemory>

#include "ControlPanel.h"
#include "InfoPanel.h"
#include "ChartPage.h"
#include "GroupPage.h"
#include "ScriptPage.h"
#include "SidePanel.h"
#include "BarData.h"
#include "PlotGroup.h"

extern QString g_session;
extern QString g_localSettings;
extern QString g_globalSettings;
extern QMainWindow *g_parent;
extern ControlPanel * g_controlPanel;
extern InfoPanel * g_infoPanel;
extern ChartPage * g_chartPanel;
extern GroupPage * g_groupPanel;
extern ScriptPage * g_scriptPanel;
extern SidePanel * g_sidePanel;
extern BarData g_currentSymbol;
extern QSharedMemory g_sharedCurrentSymbol;
extern PlotGroup * g_plotGroup;

#endif
