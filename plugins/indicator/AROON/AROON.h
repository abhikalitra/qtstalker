/*
 *  Qtstalker stock charter Aroon Indicator
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
 *  Adaptation to AROON algorithm humbly copyright (C) 2005 Jean "Flynn" Flinois
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

class AROON : public IndicatorPlugin
{
  public:
    AROON ();
    virtual ~AROON ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QString &, QPtrList<PlotLine> &);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    void getAROON (int period);
    void getOSC ();
    PlotLine * getTR ();
    int getMinBars ();
    
  private:
    QColor downColor;
    QColor upColor;
    QColor oscColor;
    QColor buyColor;
    QColor sellColor;
    PlotLine::LineType downLineType;
    PlotLine::LineType upLineType;
    PlotLine::LineType oscLineType;
    QString downLabel;
    QString upLabel;
    QString oscLabel;
    int period;
    int buyLine;
    int sellLine;
    QString lineRequest;
    QStringList lineList;
    QString label;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}
