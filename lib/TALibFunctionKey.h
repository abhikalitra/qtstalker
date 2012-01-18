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

// *************************************************************************************************
// *************************************************************************************************

#ifndef TALIB_FUNCTION_KEY_HPP
#define TALIB_FUNCTION_KEY_HPP

#include "Keys.h"

class TALibFunctionKey : public Keys
{
  public:
    enum Key
    {
      _2CROWS,
      _3BLACKCROWS,
      _3INSIDE,
      _3LINESTRIKE,
      _3OUTSIDE,
      _3STARSINSOUTH,
      _3WHITESOLDIERS,
      _ADVANCEBLOCK,
      _BELTHOLD,
      _BREAKAWAY,
      _CLOSINGMARUBOZU,
      _CONCEALBABYSWALL,
      _COUNTERATTACK,
      _DOJI,
      _DOJISTAR,
      _DRAGONFLYDOJI,
      _ENGULFING,
      _GAPSIDESIDEWHITE,
      _GRAVESTONEDOJI,
      _HAMMER,
      _HANGINGMAN,
      _HARAMI,
      _HARAMICROSS,
      _HIGHWAVE,
      _HIKKAKE,
      _HIKKAKEMOD,
      _HOMINGPIGEON,
      _IDENTICAL3CROWS,
      _INNECK,
      _INVERTEDHAMMER,
      _KICKING,
      _KICKINGBYLENGTH,
      _LADDERBOTTOM,
      _LONGLEGGEDDOJI,
      _LONGLINE,
      _MARUBOZU,
      _MATCHINGLOW,
      _ONNECK,
      _PIERCING,
      _RICKSHAWMAN,
      _RISEFALL3METHODS,
      _SEPARATINGLINES,
      _SHOOTINGSTAR,
      _SHORTLINE,
      _SPINNINGTOP,
      _STALLEDPATTERN,
      _STICKSANDWICH,
      _TAKURI,
      _TASUKIGAP,
      _THRUSTING,
      _TRISTAR,
      _UNIQUE3RIVER,
      _UPSIDEGAP2CROWS,
      _XSIDEGAP3METHODS,
      _ABANDONEDBABY,
      _DARKCLOUDCOVER,
      _EVENINGDOJISTAR,
      _EVENINGSTAR,
      _MORNINGDOJISTAR,
      _AD,
      _ADX,
      _ADXR,
      _APO,
      _AROON,
      _ATR,
      _BBANDS,
      _BETA,
      _BOP,
      _CCI,
      _CMO,
      _CORREL,
      _HT_DCPERIOD,
      _HT_DCPHASE,
      _HT_TRENDLINE,
      _HT_TRENDMODE,
      _LR_LINEARREG,
      _LR_ANGLE,
      _LR_INTERCEPT,
      _LR_SLOPE,
      _LR_TSF,
      _MACD,
      _MAMA,
      _MAX,
      _MDI,
      _MFI,
      _MIN,
      _MOM,
      _PDI,
      _PHASOR,
      _PPO,
      _ROC,
      _ROCP,
      _ROCR,
      _ROCR100,
      _RSI,
      _SAR,
      _SINE,
      _STDDEV,
      _STOCHF,
      _STOCHRSI,
      _STOCHS,
      _T3,
      _TRIX,
      _ULTOSC,
      _VAR,
      _WILLR
    };
    
    TALibFunctionKey ();
};

#endif
