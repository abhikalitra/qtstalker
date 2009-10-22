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

#include "PlotLine.h"
#include "BarData.h"
#include "IndicatorSettings.h"



#include <QProcess>
#include <QList>
#include <QString>
#include <QStringList>
#include <QObject>
#include <QHash>




class ExScript : public QObject
{
  Q_OBJECT
  
  signals:
    void signalDone ();
  
  public:
    enum IndicatorName
    {
      ACOS,
      AD,
      ADD,
      ADOSC,
      ADX,
      ADXR,
      APO,
      AROON,
      AROONOSC,
      ASIN,
      ATAN,
      ATR,
      AVGPRICE,
      BARS,
      BBANDS,
      BETA,
      BOP,
      CANDLES,
      CCI,
      CDL2CROWS,
      CDL3BLACKCROWS,
      CDL3INSIDE,
      CDL3LINESTRIKE,
      CDL3OUTSIDE,
      CDL3STARSINSOUTH,
      CDL3WHITESOLDIERS,
      CDLABANDONEDBABY,
      CDLADVANCEBLOCK,
      CDLBELTHOLD,
      CDLBREAKAWAY,
      CDLCLOSINGMARUBOZU,
      CDLCONCEALBABYSWALL,
      CDLCOUNTERATTACK,
      CDLDARKCLOUDCOVER,
      CDLDOJI,
      CDLDOJISTAR,
      CDLDRAGONFLYDOJI,
      CDLENGULFING,
      CDLEVENINGDOJISTAR,
      CDLEVENINGSTAR,
      CDLGAPSIDESIDEWHITE,
      CDLGRAVESTONEDOJI,
      CDLHAMMER,
      CDLHANGINGMAN,
      CDLHARAMI,
      CDLHARAMICROSS,
      CDLHIGHWAVE,
      CDLHIKKAKE,
      CDLHIKKAKEMOD,
      CDLHOMINGPIGEON,
      CDLIDENTICAL3CROWS,
      CDLINNECK,
      CDLINVERTEDHAMMER,
      CDLKICKING,
      CDLKICKINGBYLENGTH,
      CDLLADDERBOTTOM,
      CDLLONGLEGGEDDOJI,
      CDLLONGLINE,
      CDLMARUBOZU,
      CDLMATCHINGLOW,
      CDLMATHOLD,
      CDLMORNINGDOJISTAR,
      CDLMORNINGSTAR,
      CDLONNECK,
      CDLPIERCING,
      CDLRICKSHAWMAN,
      CDLRISEFALL3METHODS,
      CDLSEPARATINGLINES,
      CDLSHOOTINGSTAR,
      CDLSHORTLINE,
      CDLSPINNINGTOP,
      CDLSTALLEDPATTERN,
      CDLSTICKSANDWICH,
      CDLTAKURI,
      CDLTASUKIGAP,
      CDLTHRUSTING,
      CDLTRISTAR,
      CDLUNIQUE3RIVER,
      CDLUPSIDEGAP2CROWS,
      CDLXSIDEGAP3METHODS,
      CEIL,
      CMO,
      COLOR,
      COMPARE,
      COMPARE2,
      CORREL,
      COS,
      COSH,
      DEMA,
      DIV,
      DX,
      EMA,
      EXP,
      FLOOR,
      HT_DCPERIOD,
      HT_DCPHASE,
      HT_PHASOR,
      HT_SINE,
      HT_TRENDLINE,
      HT_TRENDMODE,
      KAMA,
      LINEARREG,
      LINEARREG_ANGLE,
      LINEARREG_INTERCEPT,
      LINEARREG_SLOPE,
      LN,
      LOG10,
      MACD,
      MACDEXT,
      MACDFIX,
      MAMA,
      MAVP,
      MAX,
      MAXINDEX,
      MEDPRICE,
      MFI,
      MIDPOINT,
      MIDPRICE,
      MIN,
      MININDEX,
      MINMAX,
      MINMAXINDEX,
      MINUS_DI,
      MINUS_DM,
      MOM,
      MULT,
      NATR,
      NORMAL,
      OBV,
      PLUS_DI,
      PLUS_DM,
      PPO,
      REF,
      ROC,
      ROCP,
      ROCR,
      ROCR100,
      RSI,
      SAR,
      SAREXT,
      SIN,
      SINH,
      SMA,
      SQRT,
      STDDEV,
      STOCH,
      STOCHF,
      STOCHRSI,
      SUB,
      SUM,
      SYMBOL,
      T3,
      TAN,
      TANH,
      TEMA,
      TRANGE,
      TRIMA,
      TRIX,
      TSF,
      TYPPRICE,
      ULTOSC,
      VAR,
      WCLPRICE,
      WILLR,
      WMA
    };
    
    ExScript ();
    ~ExScript ();
    void calculate (QString &command);
    void sendBarData (QStringList &l);
    int plot (QStringList &l);
    int parseIndicator (QStringList &l);
    void getLines (QList<PlotLine *> &);
    void setInput (BarData *bd);
    void getIndicatorList (QStringList &l);
    int getCDL (IndicatorSettings &, int); // CDL* functions
    int getInput (IndicatorSettings &, int); // single input functions
    int getInput2 (IndicatorSettings &, int); // 2 input functions
    int getFieldInput (IndicatorSettings &, int); // price field input functions
    int getBARS (IndicatorSettings &); // BARS function
    int getCANDLES (IndicatorSettings &); // CANDLES function
    int getCOLOR (IndicatorSettings &); // COLOR function
    int getCOMPARE (IndicatorSettings &); // COMPARE function
    int getCOMPARE2 (IndicatorSettings &); // COMPARE2 function
    int getNORMAL (IndicatorSettings &); // Normalize function
    int getREF (IndicatorSettings &); // REF function
    int getSYMBOL (IndicatorSettings &); // SYMBOL function

  public slots:
    void readFromStdout ();
    void readFromStderr ();
    void done (int, QProcess::ExitStatus);
    
  private:
    QProcess *proc;
    BarData *data;
    QHash<QString, PlotLine *> tlines;
    QStringList plotOrder;
    QStringList opList;
    QStringList indicatorList;
    QStringList inputFieldList;
    QStringList maList;
};

