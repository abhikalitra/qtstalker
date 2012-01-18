#ifndef GLOBAL_DATA_HPP
#define GLOBAL_DATA_HPP

#include <QHash>
#include <QString>
#include <QMutex>

#include "Entity.h"

extern QMutex g_dataMutex;
extern QHash<QString, Entity> g_dataList;

#endif
