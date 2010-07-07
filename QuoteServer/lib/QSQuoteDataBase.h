/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#ifndef QSQUOTE_DATA_BASE_HPP
#define QSQUOTE_DATA_BASE_HPP

#include <QString>
#include <QList>

#include "QSDataBase.h"
#include "QSSymbol.h"
#include "QSBar.h"

class QSQuoteDataBase : public QSDataBase
{
  public:
    QSQuoteDataBase (QString &dbPath);
    int deleteSymbol (QSSymbol &);
    int search (QString &ex, QString &pat, QString &);
    int symbol (QSSymbol &);
    int addSymbol (QSSymbol &);
    void exchanges (QString &);
    int rename (QSSymbol &oldSymbol, QSSymbol &newSymbol);
    int getBars (QSSymbol &);
    int setBars (QSSymbol &, QList<QSBar> &);
};

#endif
