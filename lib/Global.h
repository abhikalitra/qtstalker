#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <QString>
#include <QMutex>

extern QString g_session;
extern QString g_localSettings;
extern QString g_globalSettings;
extern QMutex g_chartRefreshMutex;

#endif
