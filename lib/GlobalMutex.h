#ifndef GLOBAL_MUTEX_HPP
#define GLOBAL_MUTEX_HPP

#include <QHash>
#include <QString>
#include <QMutex>

extern QMutex g_mutex;
extern QHash<QString, QMutex *> g_mutexList;

#endif
