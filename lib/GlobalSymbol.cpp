#include "GlobalSymbol.h"

Symbol * g_currentSymbol = 0;
QMutex g_symbolMutex;
