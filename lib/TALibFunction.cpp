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

#include "TALibFunction.h"
#include "TALibFunctionKey.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

TALibFunction::TALibFunction ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "TALibFunction::TALibFunction: error on TA_Initialize";
}

int TALibFunction::run (Entity &parms, int inNum, Entity &input, Entity &input2, Entity &input3,
			Entity &input4, int outNum, Entity &line, Entity &line2, Entity &line3)
{
  ScriptVerifyCurveKeys svck;
  QList<QString> keys;
  int rc = 1;
  switch (inNum)
  {
    case 1:
      rc = svck.keys1(input, keys);
      break;
    case 2:
      rc = svck.keys2(input, input2, keys);
      break;
    case 3:
      rc = svck.keys3(input, input2, input3, keys);
      break;
    case 4:
      rc = svck.keys4(input, input2, input3, input4, keys);
      break;
    default:
      break;
  }
  if (rc)
  {
    qDebug() << "TALibFunction::run: verify keys input error";
    return 1;
  }

  int size = keys.size();
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
  TA_Integer iout[size];
  TA_Real in[size];
  TA_Real in2[size];
  TA_Real in3[size];
  TA_Real in4[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tali;
  switch (inNum)
  {
    case 1:
      size = tali.fillInput(input, keys, &in[0]);
      break;
    case 2:
      size = tali.fillInput2(input, input2, keys, &in[0], &in2[0]);
      break;
    case 3:
      size = tali.fillInput3(input, input2, input3, keys, &in[0], &in2[0], &in3[0]);
      break;
    case 4:
      size = tali.fillInput4(input, input2, input3, input4, keys, &in[0], &in2[0], &in3[0], &in4[0]);
      break;
    default:
      break;
  }
  if (! size)
  {
    qDebug() << "TALibFunction::run: fill input error";
    return 1;
  }

  Data function, period, pen;
  parms.toData(QString("FUNCTION"), function);
  parms.toData(QString("PERIOD"), period);
  parms.toData(QString("PENETRATION"), pen);

  int ioutFlag = 0;
  TA_RetCode tarc = TA_SUCCESS;
  switch ((TALibFunctionKey::Key) function.toInteger())
  {
    case TALibFunctionKey::_2CROWS:
      tarc = TA_CDL2CROWS(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_3BLACKCROWS:
      tarc = TA_CDL3BLACKCROWS(0,  size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_3INSIDE:
      tarc = TA_CDL3INSIDE(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_3LINESTRIKE:
      tarc = TA_CDL3LINESTRIKE(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_3OUTSIDE:
      tarc = TA_CDL3OUTSIDE(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_3STARSINSOUTH:
      tarc = TA_CDL3STARSINSOUTH(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_3WHITESOLDIERS:
      tarc = TA_CDL3WHITESOLDIERS(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_ADVANCEBLOCK:
      tarc = TA_CDLADVANCEBLOCK(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_BELTHOLD:
      tarc = TA_CDLBELTHOLD(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_BREAKAWAY:
      tarc = TA_CDLBREAKAWAY(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_CLOSINGMARUBOZU:
      tarc = TA_CDLCLOSINGMARUBOZU(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_CONCEALBABYSWALL:
      tarc = TA_CDLCONCEALBABYSWALL(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_COUNTERATTACK:
      tarc = TA_CDLCOUNTERATTACK(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_DOJI:
      tarc = TA_CDLDOJI(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_DOJISTAR:
      tarc = TA_CDLDOJISTAR(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_DRAGONFLYDOJI:
      tarc = TA_CDLDRAGONFLYDOJI(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_ENGULFING:
      tarc = TA_CDLENGULFING(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_GAPSIDESIDEWHITE:
      tarc = TA_CDLGAPSIDESIDEWHITE(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_GRAVESTONEDOJI:
      tarc = TA_CDLGRAVESTONEDOJI(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_HAMMER:
      tarc = TA_CDLHAMMER(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_HANGINGMAN:
      tarc = TA_CDLHANGINGMAN(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_HARAMI:
      tarc = TA_CDLHARAMI(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_HARAMICROSS:
      tarc = TA_CDLHARAMICROSS(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_HIGHWAVE:
      tarc = TA_CDLHIGHWAVE(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_HIKKAKE:
      tarc = TA_CDLHIKKAKE(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_HIKKAKEMOD:
      tarc = TA_CDLHIKKAKEMOD(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_HOMINGPIGEON:
      tarc = TA_CDLHOMINGPIGEON(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_IDENTICAL3CROWS:
      tarc = TA_CDLIDENTICAL3CROWS(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_INNECK:
      tarc = TA_CDLINNECK(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_INVERTEDHAMMER:
      tarc = TA_CDLINVERTEDHAMMER(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_KICKING:
      tarc = TA_CDLKICKING(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_KICKINGBYLENGTH:
      tarc = TA_CDLKICKINGBYLENGTH(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_LADDERBOTTOM:
      tarc = TA_CDLLADDERBOTTOM(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_LONGLEGGEDDOJI:
      tarc = TA_CDLLONGLEGGEDDOJI(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_LONGLINE:
      tarc = TA_CDLLONGLINE(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_MARUBOZU:
      tarc = TA_CDLMARUBOZU(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_MATCHINGLOW:
      tarc = TA_CDLMATCHINGLOW(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_ONNECK:
      tarc = TA_CDLONNECK(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_PIERCING:
      tarc = TA_CDLPIERCING(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_RICKSHAWMAN:
      tarc = TA_CDLRICKSHAWMAN(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_RISEFALL3METHODS:
      tarc = TA_CDLRISEFALL3METHODS(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_SEPARATINGLINES:
      tarc = TA_CDLSEPARATINGLINES(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_SHOOTINGSTAR:
      tarc = TA_CDLSHOOTINGSTAR(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_SHORTLINE:
      tarc = TA_CDLSHORTLINE(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_SPINNINGTOP:
      tarc = TA_CDLSPINNINGTOP(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_STALLEDPATTERN:
      tarc = TA_CDLSTALLEDPATTERN(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_STICKSANDWICH:
      tarc = TA_CDLSTICKSANDWICH(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_TAKURI:
      tarc = TA_CDLTAKURI(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_TASUKIGAP:
      tarc = TA_CDLTASUKIGAP(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_THRUSTING:
      tarc = TA_CDLTHRUSTING(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_TRISTAR:
      tarc = TA_CDLTRISTAR(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_UNIQUE3RIVER:
      tarc = TA_CDLUNIQUE3RIVER(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_UPSIDEGAP2CROWS:
      tarc = TA_CDLUPSIDEGAP2CROWS(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_XSIDEGAP3METHODS:
      tarc = TA_CDLXSIDEGAP3METHODS(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &iout[0]);
      break;
    case TALibFunctionKey::_ABANDONEDBABY:
      tarc = TA_CDLABANDONEDBABY(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], pen.toInteger(), &outBeg, &outNb, &iout[0]);
      ioutFlag++;
      break;
    case TALibFunctionKey::_DARKCLOUDCOVER:
      tarc = TA_CDLDARKCLOUDCOVER(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], pen.toInteger(), &outBeg, &outNb, &iout[0]);
      ioutFlag++;
      break;
    case TALibFunctionKey::_EVENINGDOJISTAR:
      tarc = TA_CDLEVENINGDOJISTAR(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], pen.toInteger(), &outBeg, &outNb, &iout[0]);
      ioutFlag++;
      break;
    case TALibFunctionKey::_EVENINGSTAR:
      tarc = TA_CDLEVENINGSTAR(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], pen.toInteger(), &outBeg, &outNb, &iout[0]);
      ioutFlag++;
      break;
    case TALibFunctionKey::_MORNINGDOJISTAR:
      tarc = TA_CDLMORNINGDOJISTAR(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], pen.toInteger(), &outBeg, &outNb, &iout[0]);
      ioutFlag++;
      break;
    case TALibFunctionKey::_AD:
      tarc = TA_AD(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_ADX:
      tarc = TA_ADX(0, size - 1, &in[0], &in2[0], &in3[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_ADXR:
      tarc = TA_ADXR(0, size - 1, &in[0], &in2[0], &in3[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_APO:
    {
      Data fp, sp, ma;
      parms.toData(QString("PERIOD_FAST"), fp);
      parms.toData(QString("PERIOD_SLOW"), sp);
      parms.toData(QString("MA"), ma);
      tarc = TA_APO(0, size - 1, &in[0], fp.toInteger(), sp.toInteger(), (TA_MAType) ma.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    }
    case TALibFunctionKey::_AROON:
      tarc = TA_AROON(0, size - 1, &in[0], &in2[0], period.toInteger(), &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case TALibFunctionKey::_ATR:
      tarc = TA_ATR(0, size - 1, &in[0], &in2[0], &in3[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_BBANDS:
    {
      Data type, udev, ldev;
      parms.toData(QString("MA_TYPE"), type);
      parms.toData(QString("DEV_UP"), udev);
      parms.toData(QString("DEV_DOWN"), ldev);
      tarc = TA_BBANDS(0, size - 1, &in[0], period.toInteger(), udev.toDouble(), ldev.toDouble(), (TA_MAType) type.toInteger(), &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
      break;
    }
    case TALibFunctionKey::_BETA:
      tarc = TA_BETA(0, size - 1, &in[0], &in2[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_BOP:
      tarc = TA_BOP(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_CCI:
      tarc = TA_CCI(0, size - 1, &in[0], &in2[0], &in3[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_CMO:
      tarc = TA_CMO(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_CORREL:
      tarc = TA_CORREL(0, size - 1, &in[0], &in2[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_HT_DCPERIOD:
      tarc = TA_HT_DCPERIOD (0, size - 1, &in[0], &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_HT_DCPHASE:
      tarc = TA_HT_DCPHASE (0, size - 1, &in[0], &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_HT_TRENDLINE:
      tarc = TA_HT_TRENDLINE (0, size - 1, &in[0], &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_HT_TRENDMODE:
      tarc = TA_HT_TRENDMODE (0, size - 1, &in[0], &outBeg, &outNb, &iout[0]);
      ioutFlag++;
      break;
    case TALibFunctionKey::_LR_LINEARREG:
      rc = TA_LINEARREG(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_LR_ANGLE:
      rc = TA_LINEARREG_ANGLE(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_LR_INTERCEPT:
      rc = TA_LINEARREG_INTERCEPT(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_LR_SLOPE:
      rc = TA_LINEARREG_SLOPE(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_LR_TSF:
      rc = TA_TSF(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_MACD:
    {
      Data fp, fma, sp, sma, sigp, sigma;
      parms.toData(QString("PERIOD_FAST"), fp);
      parms.toData(QString("MA_FAST"), fma);
      parms.toData(QString("PERIOD_SLOW"), sp);
      parms.toData(QString("MA_SLOW"), sma);
      parms.toData(QString("PERIOD_SIG"), sigp);
      parms.toData(QString("MA_SIG"), sigma);
      tarc = TA_MACDEXT(0, size - 1, &in[0], fp.toInteger(), (TA_MAType) fma.toInteger(), sp.toInteger(), (TA_MAType) sma.toInteger(), sigp.toInteger(), (TA_MAType) sigma.toInteger(), &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
      break;
    }
    case TALibFunctionKey::_MAMA:
    {
      Data fl, sl;
      parms.toData(QString("LIMIT_FAST"), fl);
      parms.toData(QString("LIMIT_SLOW"), sl);
      tarc = TA_MAMA(0, size - 1, &in[0], fl.toDouble(), sl.toDouble(), &outBeg, &outNb, &out[0], &out2[0]);
      break;
    }
    case TALibFunctionKey::_MAX:
      tarc = TA_MAX(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_MDI:
      tarc = TA_MINUS_DI(0, size - 1, &in[0], &in2[0], &in3[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_MFI:
      tarc = TA_MFI(0, size - 1, &in[0], &in2[0], &in3[0], &in4[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_MIN:
      tarc = TA_MIN(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_MOM:
      tarc = TA_MOM(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_PDI:
      tarc = TA_PLUS_DI(0, size - 1, &in[0], &in2[0], &in3[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_PHASOR:
      tarc = TA_HT_PHASOR (0, size - 1, &in[0], &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case TALibFunctionKey::_PPO:
    {
      Data fp, sp, ma;
      parms.toData(QString("PERIOD_FAST"), fp);
      parms.toData(QString("PERIOD_SLOW"), sp);
      parms.toData(QString("MA"), ma);
      tarc = TA_PPO(0, size - 1, &in[0], fp.toInteger(), sp.toInteger(), (TA_MAType) ma.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    }
    case TALibFunctionKey::_ROC:
      tarc = TA_ROC(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_ROCP:
      tarc = TA_ROCP(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_ROCR:
      tarc = TA_ROCR(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_ROCR100:
      tarc = TA_ROCR100(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_RSI:
      tarc = TA_RSI(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_SAR:
    {
      Data si, sm;
      parms.toData(QString("STEP_INIT"), si);
      parms.toData(QString("STEP_MAX"), sm);
      tarc = TA_SAR(0, size - 1, &in[0], &in2[0], si.toDouble(), sm.toDouble(), &outBeg, &outNb, &out[0]);
      break;
    }
    case TALibFunctionKey::_SINE:
      tarc = TA_HT_SINE (0, size - 1, &in[0], &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case TALibFunctionKey::_STDDEV:
    {
      Data dev;
      parms.toData(QString("DEV"), dev);
      tarc = TA_STDDEV(0, size - 1, &in[0], period.toInteger(), dev.toDouble(), &outBeg, &outNb, &out[0]);
      break;
    }
    case TALibFunctionKey::_STOCHF:
    {
      Data pk, pd, ma;
      parms.toData(QString("PERIOD_K"), pk);
      parms.toData(QString("PERIOD_D"), pd);
      parms.toData(QString("MA"), ma);
      tarc = TA_STOCHF(0, size - 1, &in[0], &in2[0], &in3[0], pk.toInteger(), pd.toInteger(), (TA_MAType) ma.toInteger(), &outBeg, &outNb, &out[0], &out2[0]);
      break;
    }
    case TALibFunctionKey::_STOCHRSI:
      tarc = TA_STOCHRSI(0, size - 1, &in[0], period.toInteger(), period.toInteger(), 1, (TA_MAType) 0, &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case TALibFunctionKey::_STOCHS:
    {
      Data fk, sk, sd, kma, dma;
      parms.toData(QString("PERIOD_FK"), fk);
      parms.toData(QString("PERIOD_K"), sk);
      parms.toData(QString("PERIOD_D"), sd);
      parms.toData(QString("KMA"), kma);
      parms.toData(QString("DMA"), dma);
      tarc = TA_STOCH(0, size - 1, &in[0], &in2[0], &in3[0], fk.toInteger(), sk.toInteger(), (TA_MAType) kma.toInteger(), sd.toInteger(), (TA_MAType) dma.toInteger(), &outBeg, &outNb, &out[0], &out2[0]);
      break;
    }
    case TALibFunctionKey::_T3:
    {
      Data vf;
      parms.toData(QString("V"), vf);
      tarc = TA_T3(0, size - 1, &in[0], period.toInteger(), vf.toDouble(), &outBeg, &outNb, &out[0]);
      break;
    }
    case TALibFunctionKey::_TRIX:
      tarc = TA_TRIX(0, size - 1, &in[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    case TALibFunctionKey::_ULTOSC:
    {
      Data sp, mp, lp;
      parms.toData(QString("PERIOD_S"), sp);
      parms.toData(QString("PERIOD_M"), mp);
      parms.toData(QString("PERIOD_L"), lp);
      tarc = TA_ULTOSC(0, size - 1, &in[0], &in2[0], &in3[0], sp.toInteger(), mp.toInteger(), lp.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    }
    case TALibFunctionKey::_VAR:
    {
      Data dev;
      parms.toData(QString("DEV"), dev);
      tarc = TA_VAR(0, size - 1, &in[0], period.toInteger(), dev.toDouble(), &outBeg, &outNb, &out[0]);
      break;
    }
    case TALibFunctionKey::_WILLR:
      tarc = TA_WILLR(0, size - 1, &in[0], &in2[0], &in3[0], period.toInteger(), &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
  
  if (tarc != TA_SUCCESS)
  {
    qDebug() << "TALibFunction::run: TA-Lib error" << tarc;
    return 1;
  }

  TALibOutput talo;
  rc = 1;
  switch (outNum)
  {
    case 1:
      if (ioutFlag)
	rc = talo.fillOutputInteger1(line, keys, outNb, &iout[0]);
      else
        rc = talo.fillOutputDouble1(line, keys, outNb, &out[0]);
      break;
    case 2:
      rc = talo.fillOutputDouble2(line, line2, keys, outNb, &out[0], &out2[0]);
      break;
    case 3:
      rc = talo.fillOutputDouble3(line, line2, line3, keys, outNb, &out[0], &out2[0], &out3[0]);
      break;
    default:
      break;
  }
  if (rc)
  {
    qDebug() << "TALibFunction::run: fill output error";
    return 1;
  }

  return 0;
}
