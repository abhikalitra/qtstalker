#include "Globals.h"

QString g_session;
QString g_localSettings;
QString g_globalSettings;
QMainWindow * g_parent = 0;
ControlPanel * g_controlPanel = 0;
InfoPanel * g_infoPanel = 0;
ChartPage * g_chartPanel = 0;
GroupPage * g_groupPanel = 0;
ScriptPage * g_scriptPanel = 0;
SidePanel * g_sidePanel = 0;
Data * g_currentSymbol;
PlotGroup * g_plotGroup = 0;