/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "IndicatorPlugin.h"

class COMP : public IndicatorPlugin
{
  public:
  
    enum Operator
    {
      NoOp,
      Equal,
      LessThan,
      LessThanEqual,
      GreaterThan,
      GreaterThanEqual,
      And,
      Or
    };
  
    COMP ();
    virtual ~COMP ();
    void calculate ();
    void loadIndicatorSettings (QString);
    void saveIndicatorSettings (QString);
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *);
    Setting getIndicatorSettings ();
    void setIndicatorSettings (Setting);
    COMP::Operator getOperator (QString);
  
  private:
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    QString data1;
    QString data2;
    QString method;
    QStringList opList;
    QString delay1;
    QString delay2;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

