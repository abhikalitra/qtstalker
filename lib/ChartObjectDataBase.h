/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

// *************************************************************************************************
// Chart object database. Used for storing/returning chart object settings for any chart objects
// created on charts.
// *************************************************************************************************


#ifndef CHART_OBJECT_DATA_BASE_HPP
#define CHART_OBJECT_DATA_BASE_HPP

#include <QStringList>
#include <QMap>

#include "DataBase.h"
#include "BarData.h"
#include "ChartObject.h"
#include "ChartObjectSettings.h"

class ChartObjectDataBase : public DataBase
{
  public:
    ChartObjectDataBase ();
    void init ();
    void deleteChartObjects (BarData *);
    void deleteChartObjectsIndicator (QString &indicator);
    void deleteChartObject (int);
    void getChartObjects (QString &exchange, QString &symbol, QString &iname, QMap<int, ChartObject *> &);
    void setChartObject (ChartObjectSettings &);
    int renameSymbol (BarData *, BarData *);

  private:
    QStringList _fieldList;
};

#endif
