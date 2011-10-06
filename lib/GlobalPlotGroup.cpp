#include "GlobalPlotGroup.h"

PlotGroup * g_plotGroup = 0;
QHash<QString, QMutex *> g_plotMutex;
