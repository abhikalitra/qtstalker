#include "GlobalData.h"

QMutex g_dataMutex;
QHash<QString, Entity> g_dataList;
