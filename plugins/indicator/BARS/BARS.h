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

class BARS : public IndicatorPlugin
{
  public:
    BARS ();
    virtual ~BARS ();
    void calculate ();
    void calculateBar ();
    void calculateCandle ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QString &, QPtrList<PlotLine> &);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    void calculateMA ();
    void formatDialog (QStringList &vl, QString &rv, QString &rs);

  private:
    QColor barUpColor;
    QColor barDownColor;
    QColor barNeutralColor;
    QColor candleColor;
    PlotLine::LineType lineType;
    QString label;
    QString method;
    QStringList methodList;

    QColor maColor;
    QColor maColor2;
    QColor maColor3;
    PlotLine::LineType maLineType;
    PlotLine::LineType maLineType2;
    PlotLine::LineType maLineType3;
    QString maLabel;
    QString maLabel2;
    QString maLabel3;
    int maPeriod;
    int maPeriod2;
    int maPeriod3;
    int maType;
    int maType2;
    int maType3;
    BarData::InputType maInput;
    BarData::InputType maInput2;
    BarData::InputType maInput3;

    QString barUpColorLabel;
    QString barDownColorLabel;
    QString barNeutralColorLabel;
    QString candleColorLabel;
    QString labelLabel;
    QString methodLabel;
    QString lineTypeLabel;
    QString pluginLabel;

    QString maColorLabel;
    QString maLineTypeLabel;
    QString maPeriodLabel;
    QString maLabelLabel;
    QString maTypeLabel;
    QString maInputLabel;

    QString maColor2Label;
    QString maLineType2Label;
    QString maPeriod2Label;
    QString maLabel2Label;
    QString maType2Label;
    QString maInput2Label;

    QString maColor3Label;
    QString maLineType3Label;
    QString maPeriod3Label;
    QString maLabel3Label;
    QString maType3Label;
    QString maInput3Label;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}


