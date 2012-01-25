#include "GlobalScriptTimer.h"

QMutex g_scriptTimerMutex;
QHash<QString, ScriptTimer *> g_scriptTimerList;
