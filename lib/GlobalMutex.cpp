#include "GlobalMutex.h"

QMutex g_mutex;
QHash<QString, QMutex *> g_mutexList;

QMutex g_dialog;
QHash<QString, int> g_dialogStatus;
