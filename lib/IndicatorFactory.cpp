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

#include "IndicatorFactory.h"

#include "AD.h"
#include "ADOSC.h"
#include "ADX.h"
#include "AROON.h"
#include "ATR.h"
#include "BARS.h"
#include "BBANDS.h"
#include "BETA.h"
#include "BOP.h"
#include "CANDLES.h"
#include "CCI.h"
#include "CMO.h"
#include "COLOR.h"
#include "COMPARE.h"
#include "CORREL.h"
#include "CUS.h"
#include "FI.h"
#include "HT.h"
#include "LINEARREG.h"
#include "LMS.h"
#include "MA.h"
#include "MACD.h"
#include "MAMA.h"
#include "MATH1.h"
#include "MATH2.h"
#include "MAVP.h"
#include "MFI.h"
#include "MOM.h"
#include "PP.h"
#include "PO.h"
#include "RANGE.h"
#include "REF.h"
#include "ROC.h"
#include "RSI.h"
#include "SAR.h"
#include "STDDEV.h"
#include "STOCH.h"
#include "STOCHRSI.h"
#include "SYMBOL.h"
#include "SZ.h"
#include "T3.h"
#include "THERM.h"
#include "TRIX.h"
#include "ULTOSC.h"
#include "VAR.h"
#include "VFI.h"
#include "VIDYA.h"
#include "VOL.h"
#include "WILLR.h"

#include <QStringList>

IndicatorFactory::IndicatorFactory ()
{
}

IndicatorBase * IndicatorFactory::getFunction (QString &_indicator)
{
  IndicatorBase tib;
  QStringList l;
  tib.getIndicatorList(l, 0);

  IndicatorBase *ib = 0;
  int i = l.indexOf(_indicator);
  if (i < 0)
    return ib;

  switch ((IndicatorBase::IndicatorName) i)
  {
    case IndicatorBase::_AD:
    {
      ib = new AD ;
      break;
    }
    case IndicatorBase::_ADOSC:
    {
      ib = new ADOSC;
      break;
    }
    case IndicatorBase::_ADX:
    {
      ib = new ADX;
      break;
    }
    case IndicatorBase::_AROON:
    {
      ib = new AROON;
      break;
    }
    case IndicatorBase::_ATR:
    {
      ib = new ATR;
      break;
    }
    case IndicatorBase::_BARS:
    {
      ib = new BARS;
      break;
    }
    case IndicatorBase::_BBANDS:
    {
      ib = new BBANDS;
      break;
    }
    case IndicatorBase::_BETA:
    {
      ib = new BETA;
      break;
    }
    case IndicatorBase::_BOP:
    {
      ib = new BOP;
      break;
    }
    case IndicatorBase::_CANDLES:
    {
      ib = new CANDLES;
      break;
    }
    case IndicatorBase::_CCI:
    {
      ib = new CCI;
      break;
    }
    case IndicatorBase::_CMO:
    {
      ib = new CMO;
      break;
    }
    case IndicatorBase::_COLOR:
    {
      ib = new COLOR;
      break;
    }
    case IndicatorBase::_COMPARE:
    {
      ib = new COMPARE;
      break;
    }
    case IndicatorBase::_CORREL:
    {
      ib = new CORREL;
      break;
    }
    case IndicatorBase::_CUS:
    {
      ib = new CUS;
      break;
    }
    case IndicatorBase::_FI:
    {
      ib = new FI;
      break;
    }
    case IndicatorBase::_HT:
    {
      ib = new HT;
      break;
    }
    case IndicatorBase::_LINEARREG:
    {
      ib = new LINEARREG;
      break;
    }
    case IndicatorBase::_LMS:
    {
      ib = new LMS;
      break;
    }
    case IndicatorBase::_MA:
    {
      ib = new MA;
      break;
    }
    case IndicatorBase::_MACD:
    {
      ib = new MACD;
      break;
    }
    case IndicatorBase::_MAMA:
    {
      ib = new MAMA;
      break;
    }
    case IndicatorBase::_MATH1:
    {
      ib = new MATH1;
      break;
    }
    case IndicatorBase::_MATH2:
    {
      ib = new MATH2;
      break;
    }
    case IndicatorBase::_MAVP:
    {
      ib = new MAVP;
      break;
    }
    case IndicatorBase::_MFI:
    {
      ib = new MFI;
      break;
    }
    case IndicatorBase::_MOM:
    {
      ib = new MOM;
      break;
    }
    case IndicatorBase::_PP:
    {
      ib = new PP;
      break;
    }
    case IndicatorBase::_PO:
    {
      ib = new PO;
      break;
    }
    case IndicatorBase::_RANGE:
    {
      ib = new RANGE;
      break;
    }
    case IndicatorBase::_REF:
    {
      ib = new REF;
      break;
    }
    case IndicatorBase::_ROC:
    {
      ib = new ROC;
      break;
    }
    case IndicatorBase::_RSI:
    {
      ib = new RSI;
      break;
    }
    case IndicatorBase::_SAR:
    {
      ib = new SAR;
      break;
    }
    case IndicatorBase::_STDDEV:
    {
      ib = new STDDEV;
      break;
    }
    case IndicatorBase::_STOCH:
    {
      ib = new STOCH;
      break;
    }
    case IndicatorBase::_STOCHRSI:
    {
      ib = new STOCHRSI;
      break;
    }
    case IndicatorBase::_SYMBOL:
    {
      ib = new SYMBOL;
      break;
    }
    case IndicatorBase::_SZ:
    {
      ib = new SZ;
      break;
    }
    case IndicatorBase::_T3:
    {
      ib = new T3;
      break;
    }
    case IndicatorBase::_THERM:
    {
      ib = new THERM;
      break;
    }
    case IndicatorBase::_TRIX:
    {
      ib = new TRIX;
      break;
    }
    case IndicatorBase::_ULTOSC:
    {
      ib = new ULTOSC;
      break;
    }
    case IndicatorBase::_VAR:
    {
      ib = new VAR;
      break;
    }
    case IndicatorBase::_VFI:
    {
      ib = new VFI;
      break;
    }
    case IndicatorBase::_VIDYA:
    {
      ib = new VIDYA;
      break;
    }
    case IndicatorBase::_VOL:
    {
      ib = new VOL;
      break;
    }
    case IndicatorBase::_WILLR:
    {
      ib = new WILLR;
      break;
    }
    default:
      break;
  }

  return ib;
}

