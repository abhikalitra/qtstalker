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

#ifndef INDICATORBASE_HPP
#define INDICATORBASE_HPP

#include <QString>
#include <QStringList>
#include <QList>
#include <QtDebug>
#include <QObject>
#include <QHash>

#include "BarData.h"
#include "PlotLine.h"
#include "ta_libc.h"
#include "Indicator.h"
#include "Setting.h"
#include "PrefDialog.h"


class IndicatorBase
{
  public:
    enum IndicatorName
    {
      _AD,
      _ADOSC,
      _ADX,
      _AROON,
      _ATR,
      _BARS,
      _BBANDS,
      _BETA,
      _BOP,
      _CANDLES,
      _CCI,
      _CMO,
      _COLOR,
      _COMPARE,
      _CORREL,
      _CUS,
      _FI,
      _HT,
      _LINEARREG,
      _LMS,
      _MA,
      _MACD,
      _MAMA,
      _MATH1,
      _MATH2,
      _MAVP,
      _MFI,
      _MOM,
      _PP,
      _PO,
      _RANGE,
      _REF,
      _ROC,
      _RSI,
      _SAR,
      _STDDEV,
      _STOCH,
      _STOCHRSI,
      _SYMBOL,
      _SZ,
      _T3,
      _THERM,
      _TRIX,
      _TSF,
      _ULTOSC,
      _VAR,
      _VFI,
      _VIDYA,
      _VOL,
      _WILLR
    };

    IndicatorBase ();
    virtual ~IndicatorBase ();
    virtual int getIndicator (Indicator &ind, BarData *data);
    virtual int getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data);
    virtual int dialog ();
    PlotLine * getMA (PlotLine *_in, int period, int type);
    void setSettings (Indicator &_indicator);
    void getSettings (Indicator &_indicator);
    void getIndicatorList (QStringList &, int flag);
    void getMAList (QStringList &);

  protected:
    Setting settings;
    QString name;
    QString indicator;

    QStringList indicatorList;
    QStringList maList;
    QStringList opList;
    QStringList plotList;
    QStringList inputList;

    QString colorKey;
    QString plotKey;
    QString labelKey;
    QString inputKey;
    QString periodKey;
    QString maPeriodKey;
    QString maTypeKey;
    QString maColorKey;
    QString maPlotKey;
    QString maLabelKey;
};

#endif
