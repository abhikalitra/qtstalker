/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

class SZ : public IndicatorPlugin
{
  public:
    SZ ();
    virtual ~SZ ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QString &, QPtrList<PlotLine> &);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    void formatDialog (QStringList &vl, QString &rv, QString &rs);
    
  private:
    QColor color;
    PlotLine::LineType lineType;
    double coefficient;
    int period;
    int no_decline_period;
    QString method;
    QStringList methodList;
    QString label;

    QString colorLabel;
    QString lineTypeLabel;
    QString periodLabel;
    QString noDeclinePeriodLabel;
    QString coefficientLabel;
    QString methodLabel;
    QString labelLabel;
    QString pluginLabel;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}


