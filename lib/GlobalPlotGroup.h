#ifndef GLOBAL_PLOT_GROUP_HPP
#define GLOBAL_PLOT_GROUP_HPP

#include <QHash>
#include <QString>
#include <QMutex>

#include "PlotGroup.h"

extern PlotGroup * g_plotGroup;
extern QHash<QString, QMutex *> g_plotMutex;

#endif
