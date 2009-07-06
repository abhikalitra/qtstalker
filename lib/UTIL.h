/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef UTIL_HPP
#define UTIL_HPP



#include "IndicatorParms.h"
#include "PlotLine.h"
#include "BarData.h"
#include <QList>
#include <QHash>
#include <QString>
#include <QStringList>

class UTIL
{
  public:

    enum Operator
    {
      Equal,
      LessThan,
      LessThanEqual,
      GreaterThan,
      GreaterThanEqual,
      And,
      Or
    };

    UTIL ();
    ~UTIL ();
    void calculate (BarData *, IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculateAccum (IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculateNormal(IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculateCOMP (IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculateREF (IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculatePER (IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculateCOLOR (IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculateHL (IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculateINRANGE (IndicatorParms &, QHash<QString, PlotLine *> &);
    void calculateSYMBOL (IndicatorParms &, QHash<QString, PlotLine *> &);
    void prefDialog (IndicatorParms &, QStringList &);
    UTIL::Operator getOperator (QString &);
  
  private:
    QStringList methodList;
    QStringList opList;
    BarData *data;
};

#endif
