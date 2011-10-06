#ifndef GLOBAL_SYMBOL_HPP
#define GLOBAL_SYMBOL_HPP

#include <QMutex>

#include "Symbol.h"

extern Symbol *g_currentSymbol;
extern QMutex g_symbolMutex;
#endif
