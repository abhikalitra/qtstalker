#ifndef GLOBAL_SCRIPT_TIMER_HPP
#define GLOBAL_SCRIPT_TIMER_HPP

#include <QHash>
#include <QString>
#include <QMutex>

#include "ScriptTimer.h"

extern QMutex g_scriptTimerMutex;
extern QHash<QString, ScriptTimer *> g_scriptTimerList;

#endif
