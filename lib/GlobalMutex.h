#ifndef GLOBAL_MUTEX_HPP
#define GLOBAL_MUTEX_HPP

#include <QHash>
#include <QString>
#include <QMutex>

extern QMutex g_mutex;
extern QHash<QString, QMutex *> g_mutexList;

extern QMutex g_dialog;
extern QHash<QString, int> g_dialogStatus;

#endif
