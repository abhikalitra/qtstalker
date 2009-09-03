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
      ADOSC,
      ADD,
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
      CORREL,
      COS,
      COSH,
      DIV,
      DX,
      EXP,
      FLOOR,
      HT_DCPERIOD,
      HT_DCPHASE,
      HT_PHASOR,
      HT_SINE,
      HT_TRENDLINE,
      HT_TRENDMODE,
      LINEARREG,
      LINEARREG_ANGLE,
      LINEARREG_INTERCEPT,
      LINEARREG_SLOPE,
      LN,
      LOG10,
      MA,
      MACD,
      MACDEXT,
      MACDFIX,
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
      SQRT,
      STDDEV,
      STOCH,
      STOCHF,
      STOCHRSI,
      SUB,
      SUM,
      TAN,
      TANH,
      TRANGE,
      TRIX,
      TSF,
      TYPPRICE,
      ULTOSC,
      VAR,
      WCLPRICE,
      WILLR
    };
    
    ExScript ();
    ~ExScript ();
    void calculate (QString &command);
    void sendBarData (QStringList &l);
    int plot (QStringList &l);
    int parseIndicator (QStringList &l);
    void getLines (QList<PlotLine *> &);
    void setInput (BarData *bd);
    
    // indicators
    int getAROON (QStringList &l); // AROON and AROONOSC functions
    int getBARS (QStringList &l);
    int getBBANDS (QStringList &l);
    int getCANDLES (QStringList &l);
    int getCDL (QStringList &l, int); // candle functions
    int getCOMPARE (QStringList &l);
    int getCOLOR (QStringList &l);
    int getMACD (QStringList &l, int i);
    int getMINMAXINDEX (QStringList &l);
    int getNORMAL (QStringList &l);
    int getOBV (QStringList &l);
    int getREF (QStringList &l);
    int getSTOCH (QStringList &l, int i);
    int getSTOCHRSI (QStringList &l);
    
    int getSingleInput (QStringList &l, int i);
    int getHLCVInput (QStringList &l, int i);
    int getDoubleInput (QStringList &l, int i);
    int getHLCInput (QStringList &l, int i);
    int getOHLCInput (QStringList &l, int i);
    int getSingleInputInt (QStringList &l, int i);
    int getSingleInputInt2 (QStringList &l, int i);
    int getHLInput (QStringList &l, int i);
    int getDoubleOutput (QStringList &l, int i);
    int getOSC (QStringList &l, int i);

  public slots:
    void readFromStdout ();
    void done (int, QProcess::ExitStatus);
    
  private:
    QProcess *proc;
    BarData *data;
    QHash<QString, PlotLine *> tlines;
    QStringList plotOrder;
    QStringList opList;
    QStringList indicatorList;
    QStringList maList;
};

