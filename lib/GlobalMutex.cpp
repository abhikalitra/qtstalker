#include "GlobalMutex.h"

QMutex g_mutex;
QHash<QString, QMutex *> g_mutexList;
