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

#ifndef SCINDICATOR_HPP
#define SCINDICATOR_HPP


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "PlotLine.h"
#include "BarData.h"
#include "IndicatorPlugin.h"

class SCIndicator
{
  public:
    enum Method
    {
      GET, // pass indicator data to the calling script
      GET_INDEX, // pass a single indicator value to the calling script
      GET_SIZE, // returns the size of the indicator.
      PLUGIN, // get a plugin indicator
      SET // pass indicator data from script to qtstalker
    };
    
    enum Local
    {
      _COLOR,
      _COMPARE,
      _REF,
      _NORMALIZE,
      _STDDEV,
      _ADD,
      _DIV,
      _MULT,
      _SUB
    };

    SCIndicator ();
    int calculate (QStringList &, QByteArray &, QHash<QString, PlotLine *> &, BarData *, QString &);
    int getIndicator (QStringList &, QByteArray &, QHash<QString, PlotLine *> &);
    int getIndex (QStringList &, QByteArray &, QHash<QString, PlotLine *> &);
    int setIndicator (QStringList &, QByteArray &, QHash<QString, PlotLine *> &);
    int getSize (QStringList &, QByteArray &, QHash<QString, PlotLine *> &);
    int getPlugin (QStringList &, QByteArray &, QHash<QString, PlotLine *> &, BarData *, QString &);
    IndicatorPlugin * getLocalPlugin (int);

  protected:
    QStringList methodList;
    QStringList localList;
};

#endif
