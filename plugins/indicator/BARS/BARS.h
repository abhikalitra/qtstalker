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
    int getMinBars ();
    void calculateMA ();

    int getCandleColor (double open, double close); // 0 = none, 1=black, 2=white
    bool getCandleLongBlackBody (double open, double high, double low, double close);
    bool getCandleLongWhiteBody (double open, double high, double low, double close);
    bool getCandleDoji (double open, double high, double low, double close);
    bool getBlackMarubozu (double open, double high, double low, double close);
    bool getWhiteMarubozu (double open, double high, double low, double close);
    bool getCandleEngulf (double fhigh, double flow, double shigh, double slow);

    bool getCBullishAbandonedBaby (int index);
    bool getCBearishAbandonedBaby (int index);
    bool getCConcealingBabySwallow (int index);
    bool getCBullishKicking (int index);
    bool getCBearishKicking (int index);
    bool getCMatHold (int index);
    bool getCMatchingLow (int index);
    bool getCMorningDojiStar (int index);
    bool getCMorningStar (int index);
    bool getCSideBySideWhiteLines (int index);
    bool getCThreeInsideUp (int index);
    bool getCThreeOutsideUp (int index);
    bool getCThreeWhiteSoldiers (int index);
    bool getCDarkCloudCover (int index);
    bool getCEveningDojiStar (int index);
    bool getCEveningStar (int index);
    bool getCFallingThreeMethods (int index);
    bool getCIdenticalThreeCrows (int index);
    bool getCThreeBlackCrows (int index);
    bool getCThreeInsideDown (int index);
    bool getCThreeOutsideDown (int index);
    bool getCUpsideGapTwoCrows (int index);
    
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

    // candle function stuff
    double longPercentage;
    double dojiPercentage;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}


