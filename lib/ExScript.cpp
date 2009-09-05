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

#include "ExScript.h"
#include "ta_libc.h"

#include <QByteArray>
#include <QtDebug>
#include <math.h> // for fabs()



ExScript::ExScript ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");

  proc = 0;

  opList << "=" << "<" << "<=" << ">" << ">=" << "AND" << "OR";

  maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA" << "MAMA" << "T3";
  
  indicatorList << "ACOS" << "AD" << "ADOSC" << "ADD" << "ADX" << "ADXR" << "APO" << "AROON" << "AROONOSC" << "ASIN";
  indicatorList << "ATAN" << "ATR" << "AVGPRICE" << "BARS" << "BBANDS" << "BETA" << "BOP" << "CANDLES" << "CCI" << "CDL2CROWS" << "CDL3BLACKCROWS";
  indicatorList << "CDL3INSIDE" << "CDL3LINESTRIKE" << "CDL3OUTSIDE" << "CDL3STARSINSOUTH" << "CDL3WHITESOLDIERS";
  indicatorList << "CDLABANDONEDBABY" << "CDLADVANCEBLOCK" << "CDLBELTHOLD" << "CDLBREAKAWAY" << "CDLCLOSINGMARUBOZU";
  indicatorList << "CDLCONCEALBABYSWALL" << "CDLCOUNTERATTACK" << "CDLDARKCLOUDCOVER" << "CDLDOJI" << "CDLDOJISTAR";
  indicatorList << "CDLDRAGONFLYDOJI" << "CDLENGULFING" << "CDLEVENINGDOJISTAR" << "CDLEVENINGSTAR" << "CDLGAPSIDESIDEWHITE";
  indicatorList << "CDLGRAVESTONEDOJI" << "CDLHAMMER" << "CDLHANGINGMAN" << "CDLHARAMI" << "CDLHARAMICROSS" << "CDLHIGHWAVE";
  indicatorList << "CDLHIKKAKE" << "CDLHIKKAKEMOD" << "CDLHOMINGPIGEON" << "CDLIDENTICAL3CROWS" << "CDLINNECK";
  indicatorList << "CDLINVERTEDHAMMER" << "CDLKICKING" << "CDLKICKINGBYLENGTH" << "CDLLADDERBOTTOM" << "CDLLONGLEGGEDDOJI";
  indicatorList << "CDLLONGLINE" << "CDLMARUBOZU" << "CDLMATCHINGLOW" << "CDLMATHOLD" << "CDLMORNINGDOJISTAR";
  indicatorList << "CDLMORNINGSTAR" << "CDLONNECK" << "CDLPIERCING" << "CDLRICKSHAWMAN" << "CDLRISEFALL3METHODS";
  indicatorList << "CDLSEPARATINGLINES" << "CDLSHOOTINGSTAR" << "CDLSHORTLINE" << "CDLSPINNINGTOP" << "CDLSTALLEDPATTERN";
  indicatorList << "CDLSTICKSANDWICH" << "CDLTAKURI" << "CDLTASUKIGAP" << "CDLTHRUSTING" << "CDLTRISTAR" << "CDLUNIQUE3RIVER";
  indicatorList << "CDLUPSIDEGAP2CROWS" << "CDLXSIDEGAP3METHODS" << "CEIL" << "CMO" << "COLOR" << "COMPARE" << "COMPARE2";
  indicatorList << "CORREL" << "COS" << "COSH" << "DIV" << "DX" << "EXP" << "FLOOR" << "HT_DCPERIOD" << "HT_DCPHASE" << "HT_PHASOR";
  indicatorList << "HT_SINE" << "HT_TRENDLINE" << "HT_TRENDMODE" << "LINEARREG" << "LINEARREG_ANGLE" << "LINEARREG_INTERCEPT";
  indicatorList << "LINEARREG_SLOPE" << "LN" << "LOG10" << "MA" << "MACD" << "MACDEXT" << "MACDFIX" << "MAVP";
  indicatorList << "MAX" << "MAXINDEX" << "MEDPRICE" << "MFI" << "MIDPOINT" << "MIDPRICE" << "MIN" << "MININDEX" << "MINMAX";
  indicatorList << "MINMAXINDEX" << "MINUS_DI" << "MINUS_DM" << "MOM" << "MULT" << "NATR" << "NORMAL" << "OBV" << "PLUS_DI" << "PLUS_DM";
  indicatorList << "PPO" << "REF" << "ROC" << "ROCP" << "ROCR" << "ROCR100" << "RSI" << "SAR" << "SAREXT" << "SIN" << "SINH";
  indicatorList << "SQRT" << "STDDEV" << "STOCH" << "STOCHF" << "STOCHRSI" << "SUB" << "SUM" << "TAN" << "TANH";
  indicatorList << "TRANGE" << "TRIX" << "TSF" << "TYPPRICE" << "ULTOSC" << "VAR" << "WCLPRICE" << "WILLR";
}

ExScript::~ExScript ()
{
  TA_Shutdown();  
}

void ExScript::setInput (BarData *bd)
{
  data = bd;
}

void ExScript::calculate (QString &command)
{
  // clean up if needed
  if (proc)
    delete proc;
  qDeleteAll(tlines);
  tlines.clear();
  plotOrder.clear();
  
  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));

  // start the process
  proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! proc->waitForStarted(30000))
  {
    qDebug() << "ExScript::calculate: error starting script...timed out";
    delete proc;
    proc = 0;
    return;
  }
}

void ExScript::done (int, QProcess::ExitStatus)
{
  delete proc;
  proc = 0;
  emit signalDone();
}

void ExScript::sendBarData (QStringList &l)
{
  // format 'GET_INDICATOR,VARIABLE' returns the requested data in a CSV string
  // format 'SET_INDICATOR,VARIABLE,CSV,DATA,FROM,NOW,ON' - will create a new line using the provided data

  if (l[0] == "GET_INDICATOR")
  {
    if (l.count() != 2)
    {
      qDebug() << "ExScript::sendBarData: parm error " << l;
      proc->write("ERROR\n");
      return;
    }
    
    int flag = FALSE;
    PlotLine *in = tlines.value(l[1]);
    if (! in)
    {
      in = data->getInput(data->getInputType(l[1]));
      if (! in)
      {
        qDebug() << "ExScript::sendBarData: cannot create input_1 " << l[1];
        proc->write("ERROR\n");
        return;
      }
    
      flag = TRUE;
    }

    int loop;
    QStringList l2;
    for (loop = 0; loop < in->getSize(); loop++)
      l2.append(QString::number(in->getData(loop)));
  
    QByteArray ba;
    ba.append(l2.join(","));
    ba.append('\n');
    proc->write(ba);
    
    if (flag)
      delete in;
    
    return;
  }
  
  if (l[0] == "SET_INDICATOR")
  {
    if (l.count() < 3)
    {
      qDebug() << "ExScript::sendBarData: parm error " << l;
      proc->write("1\n");
      return;
    }
    
    PlotLine *line = tlines.value(l[1]);
    if (line)
    {
      // variable already used, abort
      qDebug() << "ExScript::sendBarData: duplicate variable name" << l[1];
      proc->write("1\n");
      return;
    }
    
    line = new PlotLine;
    int loop;
    for (loop = 2; loop < l.count(); loop++)
      line->append(l[loop].toDouble());
    
    tlines.insert(l[1], line);

    proc->write("0\n");
  }  
}

void ExScript::readFromStdout ()
{
  QByteArray ba = proc->readAllStandardOutput();
  QString s(ba);
  QStringList l =  s.split(",");
  if (! l.count())
  {
    qDebug() << "ExScript::readFromStdout: invalid request string " << l;
    return;
  }
  
  if (l[0] == "GET_INDICATOR" || l[0] == "SET_INDICATOR")
  {
    sendBarData(l);
    return;
  }

  if (l[0] == ("PLOT"))
  {
    int rc = plot(l);
    if (rc)
      proc->write("1\n");
    else
      proc->write("0\n");
    return;
  }

  int rc = parseIndicator(l);
  if (rc)
    proc->write("1\n");
  else
    proc->write("0\n");
}

int ExScript::plot (QStringList &l)
{
  // format 'PLOT,NAME,LABEL,COLOR,LINE_TYPE'
  if (l.count() != 5)
  {
    qDebug() << "ExScript::plot: parm error" << l;
    return 1;
  }

  PlotLine *line = tlines[l[1]];
  if (! line)
  {
    qDebug() << "ExScript::plot: bad line name" << l[1];
    return 1;
  }
  
  line->setLabel(l[2]);
  line->setColor(l[3]);
  line->setType(l[4]);
  line->setPlotFlag(TRUE);
  
  plotOrder.append(l[1]);
  
  return 0;
}

void ExScript::getLines (QList<PlotLine *> &lines)
{
  int loop;
  for (loop = 0; loop < plotOrder.count(); loop++)
  {
    lines.append(tlines[plotOrder[loop]]);
    tlines.remove(plotOrder[loop]);
  }
}

int ExScript::parseIndicator (QStringList &l)
{
  int rc = FALSE;
  int i = indicatorList.indexOf(l[0]);
  switch (i)
  {
    case ACOS:
    case ASIN:
    case ATAN:
    case CEIL:
    case COS:
    case COSH:
    case EXP:
    case FLOOR:
    case HT_DCPERIOD:
    case HT_DCPHASE:
    case HT_TRENDLINE:
    case LN:
    case LOG10:
    case SIN:
    case SINH:
    case SQRT:
    case TAN:
    case TANH:
      rc = getSingleInput(l, i);
      break;
    case AD:
    case ADOSC:
    case MFI:
      rc = getHLCVInput(l, i);
      break;
    case ADD:
    case BETA:
    case CORREL:
    case DIV:
    case MULT:
    case SUB:
    case MAVP:
      rc = getDoubleInput(l, i);
      break;
    case ADX:
    case ADXR:
    case ATR:
    case CCI:
    case DX:
    case MINUS_DI:
    case NATR:
    case PLUS_DI:
    case TRANGE:
    case TYPPRICE:
    case ULTOSC:
    case WCLPRICE:
    case WILLR:
      rc = getHLCInput(l, i);
      break;
    case AVGPRICE:
    case BOP:
      rc = getOHLCInput(l, i);
      break;
    case APO:
    case PPO:
      rc = getOSC(l, i);
      break;
    case AROON:
    case AROONOSC:
      rc = getAROON(l);
      break;
    case BARS:
      rc = getBARS(l);
      break;
    case BBANDS:
      rc = getBBANDS(l);
      break;
    case CDL2CROWS:
    case CDL3BLACKCROWS:
    case CDL3INSIDE:
    case CDL3LINESTRIKE:
    case CDL3OUTSIDE:
    case CDL3STARSINSOUTH:
    case CDL3WHITESOLDIERS:
    case CDLABANDONEDBABY:
    case CDLADVANCEBLOCK:
    case CDLBELTHOLD:
    case CDLBREAKAWAY:
    case CDLCLOSINGMARUBOZU:
    case CDLCONCEALBABYSWALL:
    case CDLCOUNTERATTACK:
    case CDLDARKCLOUDCOVER:
    case CDLDOJI:
    case CDLDOJISTAR:
    case CDLDRAGONFLYDOJI:
    case CDLENGULFING:
    case CDLEVENINGDOJISTAR:
    case CDLEVENINGSTAR:
    case CDLGAPSIDESIDEWHITE:
    case CDLGRAVESTONEDOJI:
    case CDLHAMMER:
    case CDLHANGINGMAN:
    case CDLHARAMI:
    case CDLHARAMICROSS:
    case CDLHIGHWAVE:
    case CDLHIKKAKE:
    case CDLHIKKAKEMOD:
    case CDLHOMINGPIGEON:
    case CDLIDENTICAL3CROWS:
    case CDLINNECK:
    case CDLINVERTEDHAMMER:
    case CDLKICKING:
    case CDLKICKINGBYLENGTH:
    case CDLLADDERBOTTOM:
    case CDLLONGLEGGEDDOJI:
    case CDLLONGLINE:
    case CDLMARUBOZU:
    case CDLMATCHINGLOW:
    case CDLMATHOLD:
    case CDLMORNINGDOJISTAR:
    case CDLMORNINGSTAR:
    case CDLONNECK:
    case CDLPIERCING:
    case CDLRICKSHAWMAN:
    case CDLRISEFALL3METHODS:
    case CDLSEPARATINGLINES:
    case CDLSHOOTINGSTAR:
    case CDLSHORTLINE:
    case CDLSPINNINGTOP:
    case CDLSTALLEDPATTERN:
    case CDLSTICKSANDWICH:
    case CDLTAKURI:
    case CDLTASUKIGAP:
    case CDLTHRUSTING:
    case CDLTRISTAR:
    case CDLUNIQUE3RIVER:
    case CDLUPSIDEGAP2CROWS:
    case CDLXSIDEGAP3METHODS:
      rc = getCDL(l, i);
      break;
    case CMO:
    case LINEARREG:
    case LINEARREG_ANGLE:
    case LINEARREG_INTERCEPT:
    case LINEARREG_SLOPE:
    case MA:
    case MAX:
    case MIDPOINT:
    case MIN:
    case MOM:
    case ROC:
    case ROCP:
    case ROCR:
    case ROCR100:
    case RSI:
    case STDDEV:
    case SUM:
    case TRIX:
    case TSF:
    case VAR:
      rc = getSingleInputInt(l, i);
      break;
    case HT_PHASOR:
    case HT_SINE:
    case MINMAX:
      rc = getDoubleOutput(l, i);
      break;
    case MACD:
    case MACDEXT:
    case MACDFIX:
      rc = getMACD(l, i);
      break;
    case MAXINDEX:
    case MININDEX:
    case HT_TRENDMODE:
      rc = getSingleInputInt2(l, i);
      break;
    case MEDPRICE:
    case MIDPRICE:
    case MINUS_DM:
    case PLUS_DM:
    case SAR:
    case SAREXT:
      rc = getHLInput(l, i);
      break;
    case MINMAXINDEX:
      rc = getMINMAXINDEX(l);
      break;
    case OBV:
      rc = getOBV(l);
      break;
    case REF:
      rc = getREF(l);
      break;
    case STOCH:
    case STOCHF:
      rc = getSTOCH(l, i);
      break;
    case STOCHRSI:
      rc = getSTOCHRSI(l);
      break;
    case COMPARE:
      rc = getCOMPARE(l);
      break;
    case COLOR:
      rc = getCOLOR(l);
      break;
    case NORMAL:
      rc = getNORMAL(l);
      break;
    case CANDLES:
      rc = getCANDLES(l);
      break;
    case COMPARE2:
      rc = getCOMPARE2(l);
      break;
    default:
      break;
  }
  
  return rc;
}  

//**********************************************************************************
//************************ INDICATORS **********************************************
//**********************************************************************************

//***************************************************
//********** HLCV Input Functions *******************
//***************************************************
int ExScript::getHLCVInput (QStringList &l, int i)
{
  // format 'AD,NAME'
  // format 'ADOSC,NAME,FAST_PERIOD,SLOW_PERIOD'
  // format 'MFI,NAME,PERIOD'

  bool ok;
  int fast = 0;
  int slow = 0;
  int period = 0;
  
  switch (i)
  {
    case ADOSC:
      if (l.count() != 4)
      {
        qDebug() << "ExScript::getHLCVInput: parm error" << l;
        return 1;
      }

      fast = l[2].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLCVInput: parm error" << l[2];
        return 1;
      }
  
      slow = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLCVInput: parm error" << l[3];
        return 1;
      }
      break;
    case MFI:
      if (l.count() != 3)
      {
        qDebug() << "ExScript::getHLCVInput: parm error" << l;
        return 1;
      }
    
      period = l[2].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLCVInput: parm error" << l[2];
        return 1;
      }
      break;
    case AD:
      if (l.count() != 2)
      {
        qDebug() << "ExScript::getHLCVInput: parm error" << l;
        return 1;
      }
      break;
    default:
      break;
  }
  
  TA_Real high[data->count()];
  TA_Real low[data->count()];
  TA_Real close[data->count()];
  TA_Real volume[data->count()];
  TA_Real out[data->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Real) data->getVolume(loop);
  }

  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case AD:
      rc = TA_AD(0, data->count() - 1, &high[0], &low[0], &close[0], &volume[0], &outBeg, &outNb, &out[0]);
      break;
    case ADOSC:
      rc = TA_ADOSC(0, data->count() - 1, &high[0], &low[0], &close[0], &volume[0], fast, slow, &outBeg, &outNb, &out[0]);
      break;
    case MFI:
      rc = TA_MFI(0, data->count() - 1, &high[0], &low[0], &close[0], &volume[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
      
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getHLCVInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  return 0;
}

//***************************************************
//********** Double Input Functions *****************
//***************************************************
int ExScript::getDoubleInput (QStringList &l, int i)
{
  // format 'FUNCTION,NAME,INPUT_1,INPUT_2'
  // format 'BETA,NAME,INPUT_1,INPUT_2,PERIOD'
  // format 'CORREL,NAME,INPUT_1,INPUT_2,PERIOD'
  // format 'MAVP,NAME,INPUT_1,INPUT_2,MIN,MAX,MA_TYPE'

  bool ok;
  int period = 0;
  int min = 0;
  int max = 0;
  switch (i)
  {
    case BETA:
    case CORREL:
      if (l.count() != 5)
      {
        qDebug() << "ExScript::getDoubleInput: parm error" << l;
        return 1;
      }
  
      period = l[4].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getDoubleInput: parm error" << l[4];
        return 1;
      }
      break;
    case MAVP:
      if (l.count() != 7)
      {
        qDebug() << "ExScript::getDoubleInput: parm error" << l;
        return 1;
      }
  
      min = l[4].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getDoubleInput: parm error" << l[4];
        return 1;
      }

      max = l[5].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getDoubleInput: parm error" << l[5];
        return 1;
      }
      break;
    default:
      if (l.count() != 4)
      {
        qDebug() << "ExScript::getDoubleInput: parm error" << l;
        return 1;
      }
      break;
  }

  int flag = FALSE;
  int flag2 = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getDoubleInput: cannot create input_1 " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  PlotLine *in2 = tlines.value(l[3]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(l[3]));
    if (! in2)
    {
      qDebug() << "ExScript::getDoubleInput: cannot create input_2 " << l[3];
      if (flag)
	delete in;
      return 1;
    }
    
    flag2 = TRUE;
  }

  int size = in->getSize();
  if (in2->getSize() < size)
    size = in2->getSize();
  
  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = in->getSize() - 1;
  int loop2 = in2->getSize() - 1;
  int count = size - 1;
  for (; count > -1; loop--, loop2--, count--)
  {
    input[loop] = (TA_Real) in->getData(loop);
    input2[loop2] = (TA_Real) in2->getData(loop2);
  }
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case ADD:
      rc = TA_ADD(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case BETA:
      rc = TA_BETA(0, size - 1, &input[0], &input2[0], period, &outBeg, &outNb, &out[0]);
      break;
    case CORREL:
      rc = TA_CORREL(0, size - 1, &input[0], &input2[0], period, &outBeg, &outNb, &out[0]);
      break;
    case DIV:
      rc = TA_DIV(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case MAVP:
      rc = TA_MAVP(0, size - 1, &input[0], &input2[0], min, max, (TA_MAType) maList.indexOf(l[6]), &outBeg, &outNb, &out[0]);
      break;
    case MULT:
      rc = TA_MULT(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case SUB:
      rc = TA_SUB(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
      
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getDoubleInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  if (flag)
    delete in;
  if (flag2)
    delete in2;
  
  return 0;
}

//***************************************************
//********** HLC Input *****************************
//***************************************************
int ExScript::getHLCInput (QStringList &l, int i)
{
  // format 'ADX,NAME,PERIOD'
  // format 'ADXR,NAME,PERIOD'

  bool ok;
  int period = 0;
  int period2 = 0;
  int period3 = 0;
  switch (i)
  {
    case ADX:
    case ADXR:
    case ATR:
    case CCI:
    case DX:
    case MINUS_DI:
    case NATR:
    case PLUS_DI:
    case WILLR:
      if (l.count() != 3)
      {
        qDebug() << "ExScript::getHLCInput: parm error" << l;
        return 1;
      }

      period = l[2].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLCInput: parm error" << l[2];
        return 1;
      }
      break;
    case TRANGE:
    case TYPPRICE:
    case WCLPRICE:
      if (l.count() != 2)
      {
        qDebug() << "ExScript::getHLCInput: parm error" << l;
        return 1;
      }
      break;      
    case ULTOSC:
      if (l.count() != 5)
      {
        qDebug() << "ExScript::getHLCInput: parm error" << l;
        return 1;
      }

      period = l[2].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLCInput: parm error" << l[2];
        return 1;
      }

      period2 = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLCInput: parm error" << l[3];
        return 1;
      }
      
      period3 = l[4].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLCInput: parm error" << l[4];
        return 1;
      }
      break;
    default:
      break;
  }
  
  TA_Real high[data->count()];
  TA_Real low[data->count()];
  TA_Real close[data->count()];
  TA_Real out[data->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case ADX:
      rc = TA_ADX(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ADXR:
      rc = TA_ADXR(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ATR:
      rc = TA_ATR(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case CCI:
      rc = TA_CCI(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case DX:
      rc = TA_DX(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case MINUS_DI:
      rc = TA_MINUS_DI(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case NATR:
      rc = TA_NATR(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case PLUS_DI:
      rc = TA_PLUS_DI(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case TRANGE:
      rc = TA_TRANGE(0, data->count() - 1, &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TYPPRICE:
      rc = TA_TYPPRICE(0, data->count() - 1, &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case ULTOSC:
      rc = TA_ULTOSC(0, data->count() - 1, &high[0], &low[0], &close[0], period, period2, period3, &outBeg, &outNb, &out[0]);
      break;
    case WCLPRICE:
      rc = TA_WCLPRICE(0, data->count() - 1, &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case WILLR:
      rc = TA_WILLR(0, data->count() - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getHLCInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  return 0;
}

//***************************************************
//********** OSCILLATORS ****************************
//***************************************************
int ExScript::getOSC (QStringList &l, int i)
{
  // format 'APO,NAME,INPUT,FAST_PERIOD,SLOW_PERIOD,MA_TYPE'

  bool ok;
  int fast = 0;
  int slow = 0;
  switch (i)
  {
    default:
      if (l.count() != 6)
      {
        qDebug() << "ExScript::getOSC: parm error" << l;
        return 1;
      }

      fast = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getOSC: parm error" << l[3];
        return 1;
      }
  
      slow = l[4].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getOSC: parm error" << l[4];
        return 1;
      }
      break;
  }
  
  int flag = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getOSC: cannot create input " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case APO:
      rc = TA_APO(0, in->getSize() - 1, &input[0], fast, slow, (TA_MAType) maList.indexOf(l[5]), &outBeg, &outNb, &out[0]);
      break;
    case PPO:
      rc = TA_PPO(0, in->getSize() - 1, &input[0], fast, slow, (TA_MAType) maList.indexOf(l[5]), &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getOSC: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  if (flag)
    delete in;
  
  return 0;
}

//***************************************************
//********** AROON **********************************
//***************************************************
int ExScript::getAROON (QStringList &l)
{
  // format 'AROON,NAME_DOWN,NAME_UP,PERIOD'
  // format 'AROONOSC,NAME,PERIOD'

  bool ok;
  int period;
  if (l[0] == "AROON")
  {
    if (l.count() != 4)
    {
      qDebug() << "ExScript::AROON: parm error" << l;
      return 1;
    }

    period = l[3].toInt(&ok);
    if (! ok)
    {
      qDebug() << "ExScript::AROON: parm error" << l[3];
      return 1;
    }
  }
  else
  {
    if (l.count() != 3)
    {
      qDebug() << "ExScript::AROONOSC: parm error" << l;
      return 1;
    }

    period = l[2].toInt(&ok);
    if (! ok)
    {
      qDebug() << "ExScript::AROONOSC: parm error" << l[2];
      return 1;
    }
  }
  
  TA_Real high[data->count()];
  TA_Real low[data->count()];
  TA_Real out[data->count()];
  TA_Real out2[data->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
  }
  
  TA_RetCode rc;
  if (l[0] == "AROON")
    rc = TA_AROON(0, data->count() - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0], &out2[0]);
  else
    rc = TA_AROONOSC(0, data->count() - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::AROON: TA-Lib error" << rc;
    return 1;
  }

  if (l[0] == "AROON")
  {
    PlotLine *line = new PlotLine;
    PlotLine *line2 = new PlotLine;
    for (loop = 0; loop < outNb; loop++)
    {
      line->append(out[loop]);
      line2->append(out2[loop]);
    }

    tlines.insert(l[1], line);
    tlines.insert(l[2], line2);
  }
  else
  {
    PlotLine *line = new PlotLine;
    for (loop = 0; loop < outNb; loop++)
      line->append(out[loop]);

    tlines.insert(l[1], line);
  }

  return 0;
}

//***************************************************
//********** OHLC Input *******************************
//***************************************************
int ExScript::getOHLCInput (QStringList &l, int i)
{
  // format 'AVGPRICE,NAME'

  switch (i)
  {
    case AVGPRICE:
    case BOP:
      if (l.count() != 2)
      {
        qDebug() << "ExScript::getOHLCInput: parm error" << l;
        return 1;
      }
      break;
    default:
      break;
  }

  TA_Real high[data->count()];
  TA_Real low[data->count()];
  TA_Real close[data->count()];
  TA_Real open[data->count()];
  TA_Real out[data->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    open[loop] = (TA_Real) data->getOpen(loop);
  }
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case AVGPRICE:
      rc = TA_AVGPRICE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case BOP:
      rc = TA_BOP(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getOHLCInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  return 0;
}

//***************************************************
//********** BARS ***********************************
//***************************************************
int ExScript::getBARS (QStringList &l)
{
  // format 'BARS,NAME,BAR_UP_COLOR,BAR_DOWN_COLOR,BAR_NEUTRAL_COLOR'
  
  if (l.count() != 5)
  {
    qDebug() << "ExScript::BARS: parm error" << l;
    return 1;
  }

  QColor color;
  QColor barUpColor(l[2]);
  if (! barUpColor.isValid())
  {
    qDebug() << "ExScript::CANDLES: parm error" << l[2];
    return 1;
  }

  QColor barDownColor(l[3]);
  if (! barDownColor.isValid())
  {
    qDebug() << "ExScript::CANDLES: parm error" << l[3];
    return 1;
  }
  
  QColor barNeutralColor(l[4]);
  if (! barNeutralColor.isValid())
  {
    qDebug() << "ExScript::CANDLES: parm error" << l[4];
    return 1;
  }

  PlotLine *line = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    if (loop > 0)
    {  
      if (data->getClose(loop) > data->getClose(loop - 1))
        color = barUpColor;
      else
      {
        if (data->getClose(loop) < data->getClose(loop - 1))
          color = barDownColor;
        else
          color = barNeutralColor;
      }
    }
    else
      color = barNeutralColor;

    line->append(color, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);

    QDateTime dt;
    data->getDate(loop, dt);
    line->append(dt);
  }

  tlines.insert(l[1], line);
  
  return 0;
}

//***************************************************
//********** CANDLES ********************************
//***************************************************
int ExScript::getCANDLES (QStringList &l)
{
  // format 'CANDLES,NAME,CANDLE_COLOR'
  
  if (l.count() != 3)
  {
    qDebug() << "ExScript::CANDLES: parm error" << l;
    return 1;
  }
    
  QColor color(l[2]);
  if (! color.isValid())
  {
    qDebug() << "ExScript::CANDLES: parm error" << l[2];
    return 1;
  }

  PlotLine *line = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double c = data->getClose(loop);
    double o = data->getOpen(loop);
    bool fillFlag = FALSE;

    if (o != 0)
    {
      if (c < o)
        fillFlag = TRUE;
    }

    line->append(color, o, data->getHigh(loop), data->getLow(loop), c, fillFlag);

    QDateTime dt;
    data->getDate(loop, dt);
    line->append(dt);
  }

  tlines.insert(l[1], line);
  
  return 0;
}

//***************************************************
//********** BBANDS *********************************
//***************************************************
int ExScript::getBBANDS (QStringList &l)
{
  // format 'BBANDS,INPUT,NAME_UPPER,NAME_MIDDLE,NAME_LOWER,PERIOD,UPPER_DEVIATION,LOWER_DEVIATION,MA_TYPE'

  if (l.count() != 9)
  {
    qDebug() << "ExScript::BBANDS: parm error" << l;
    return 1;
  }

  bool ok;
  int period = l[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::BBANDS: parm error" << l[5];
    return 1;
  }

  double devUp = l[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::BBANDS: parm error" << l[6];
    return 1;
  }

  double devDown = l[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::BBANDS: parm error" << l[7];
    return 1;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[1]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[1]));
    if (! in)
    {
      qDebug() << "ExScript::BBANDS: cannot create input " << l[1];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Real upper[in->getSize()];
  TA_Real lower[in->getSize()];
  TA_Real middle[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  TA_RetCode rc = TA_BBANDS(0, in->getSize() - 1, &input[0], period, devUp, devDown, (TA_MAType) maList.indexOf(l[8]), &outBeg, &outNb, &upper[0], &middle[0], &lower[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::BBANDS: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *u = new PlotLine;
  PlotLine *m = new PlotLine;
  PlotLine *lw = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    u->append(upper[loop]);
    m->append(middle[loop]);
    lw->append(lower[loop]);
  }

  tlines.insert(l[2], u);
  tlines.insert(l[3], m);
  tlines.insert(l[4], lw);

  if (flag)
    delete in;
  
  return 0;
}

//***************************************************
//********** CDL  ***********************************
//***************************************************
int ExScript::getCDL (QStringList &l, int i)
{
  // format 'CDL,NAME'
  // format 'CDL,NAME,PENETRATION'

  if (l.count() == 2 || l.count() == 3)
  {
    qDebug() << "ExScript::CDL: parm error" << l;
    return 1;
  }
  
  double penetration = 0;
  if (l.count() == 3)
  {
    bool ok;
    penetration = l[2].toDouble(&ok);
    if (! ok)
    {
      qDebug() << "ExScript::CDL: parm error" << l[2];
      return 1;
    }
  }

  TA_Real high[data->count()];
  TA_Real low[data->count()];
  TA_Real close[data->count()];
  TA_Real open[data->count()];
  TA_Integer out[data->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    open[loop] = (TA_Real) data->getOpen(loop);
  }

  TA_RetCode rc = TA_SUCCESS;
  
  switch (i)
  {
    case CDL2CROWS:
      rc = TA_CDL2CROWS(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDL3BLACKCROWS:
      rc = TA_CDL3BLACKCROWS(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDL3INSIDE:
      rc = TA_CDL3INSIDE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDL3LINESTRIKE:
      rc = TA_CDL3LINESTRIKE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDL3OUTSIDE:
      rc = TA_CDL3OUTSIDE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDL3STARSINSOUTH:
      rc = TA_CDL3STARSINSOUTH(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDL3WHITESOLDIERS:
      rc = TA_CDL3WHITESOLDIERS(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLABANDONEDBABY:
      rc = TA_CDLABANDONEDBABY(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], penetration, &outBeg, &outNb, &out[0]);
      break;
    case CDLADVANCEBLOCK:
      rc = TA_CDLADVANCEBLOCK(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLBELTHOLD:
      rc = TA_CDLBELTHOLD(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLBREAKAWAY:
      rc = TA_CDLBREAKAWAY(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLCLOSINGMARUBOZU:
      rc = TA_CDLCLOSINGMARUBOZU(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLCONCEALBABYSWALL:
      rc = TA_CDLCONCEALBABYSWALL(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLCOUNTERATTACK:
      rc = TA_CDLCOUNTERATTACK(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLDARKCLOUDCOVER:
      rc = TA_CDLDARKCLOUDCOVER(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], penetration, &outBeg, &outNb, &out[0]);
      break;
    case CDLDOJI:
      rc = TA_CDLDOJI(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLDOJISTAR:
      rc = TA_CDLDOJISTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLDRAGONFLYDOJI:
      rc = TA_CDLDRAGONFLYDOJI(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLENGULFING:
      rc = TA_CDLENGULFING(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLEVENINGDOJISTAR:
      rc = TA_CDLEVENINGDOJISTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], penetration, &outBeg, &outNb, &out[0]);
      break;
    case CDLEVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], penetration, &outBeg, &outNb, &out[0]);
      break;
    case CDLGAPSIDESIDEWHITE:
      rc = TA_CDLGAPSIDESIDEWHITE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLGRAVESTONEDOJI:
      rc = TA_CDLGRAVESTONEDOJI(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLHAMMER:
      rc = TA_CDLHAMMER(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLHANGINGMAN:
      rc = TA_CDLHANGINGMAN(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLHARAMI:
      rc = TA_CDLHARAMI(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLHARAMICROSS:
      rc = TA_CDLHARAMICROSS(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLHIGHWAVE:
      rc = TA_CDLHIGHWAVE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLHIKKAKE:
      rc = TA_CDLHIKKAKE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLHIKKAKEMOD:
      rc = TA_CDLHIKKAKEMOD(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLHOMINGPIGEON:
      rc = TA_CDLHOMINGPIGEON(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLIDENTICAL3CROWS:
      rc = TA_CDLIDENTICAL3CROWS(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLINNECK:
      rc = TA_CDLINNECK(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLINVERTEDHAMMER:
      rc = TA_CDLINVERTEDHAMMER(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLKICKING:
      rc = TA_CDLKICKING(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLKICKINGBYLENGTH:
      rc = TA_CDLKICKINGBYLENGTH(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLLADDERBOTTOM:
      rc = TA_CDLLADDERBOTTOM(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLLONGLEGGEDDOJI:
      rc = TA_CDLLONGLEGGEDDOJI(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLLONGLINE:
      rc = TA_CDLLONGLINE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLMARUBOZU:
      rc = TA_CDLMARUBOZU(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLMATCHINGLOW:
      rc = TA_CDLMATCHINGLOW(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLMATHOLD:
      rc = TA_CDLMATHOLD(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], penetration, &outBeg, &outNb, &out[0]);
      break;
    case CDLMORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], penetration, &outBeg, &outNb, &out[0]);
      break;
    case CDLMORNINGSTAR:
      rc = TA_CDLMORNINGSTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], penetration, &outBeg, &outNb, &out[0]);
      break;
    case CDLONNECK:
      rc = TA_CDLONNECK(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLPIERCING:
      rc = TA_CDLPIERCING(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLRICKSHAWMAN:
      rc = TA_CDLRICKSHAWMAN(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLRISEFALL3METHODS:
      rc = TA_CDLRISEFALL3METHODS(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLSEPARATINGLINES:
      rc = TA_CDLSEPARATINGLINES(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLSHOOTINGSTAR:
      rc = TA_CDLSHOOTINGSTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLSHORTLINE:
      rc = TA_CDLSHORTLINE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLSPINNINGTOP:
      rc = TA_CDLSPINNINGTOP(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLSTALLEDPATTERN:
      rc = TA_CDLSTALLEDPATTERN(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLSTICKSANDWICH:
      rc = TA_CDLSTICKSANDWICH(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLTAKURI:
      rc = TA_CDLTAKURI(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLTASUKIGAP:
      rc = TA_CDLTASUKIGAP(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLTHRUSTING:
      rc = TA_CDLTHRUSTING(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLTRISTAR:
      rc = TA_CDLTRISTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLUNIQUE3RIVER:
      rc = TA_CDLUNIQUE3RIVER(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLUPSIDEGAP2CROWS:
      rc = TA_CDLUPSIDEGAP2CROWS(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CDLXSIDEGAP3METHODS:
      rc = TA_CDLXSIDEGAP3METHODS(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::CDL: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  return 0;
}

//********************************************************
//********** MACD FUNCTIONS ******************************
//********************************************************
int ExScript::getMACD (QStringList &l, int i)
{
  // format 'MACD,INPUT,OUTPUT_MACD,OUTPUT_SIGNAL,OUTPUT_HIST,FAST,SLOW,SIGNAL'
  // format 'MACDEXT,INPUT,OUTPUT_MACD,OUTPUT_SIGNAL,OUTPUT_HIST,FAST,FAST_MA_TYPE,SLOW,SLOW_MA_TYPE,SIGNAL,SIGNAL_MA_TYPE'
  // format 'MACDFIX,INPUT,OUTPUT_MACD,OUTPUT_SIGNAL,OUTPUT_HIST,SIGNAL'

  bool ok;
  int fast = 0;
  int slow = 0;
  int signal = 0;
  switch (i)
  {
    case MACD:
      if (l.count() != 8)
      {
        qDebug() << "ExScript::getMACD: parm error" << l;
        return 1;
      }
  
      fast = l[5].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getMACD: parm error" << l[5];
        return 1;
      }
      
      slow = l[6].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getMACD: parm error" << l[6];
        return 1;
      }

      signal = l[7].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getMACD: parm error" << l[7];
        return 1;
      }
      break;
    case MACDEXT:
      if (l.count() != 11)
      {
        qDebug() << "ExScript::getMACD: parm error" << l;
        return 1;
      }
  
      fast = l[5].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getMACD: parm error" << l[5];
        return 1;
      }
      
      slow = l[7].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getMACD: parm error" << l[7];
        return 1;
      }

      signal = l[9].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getMACD: parm error" << l[9];
        return 1;
      }
      break;
    case MACDFIX:
      if (l.count() != 6)
      {
        qDebug() << "ExScript::getMACD: parm error" << l;
        return 1;
      }
  
      signal = l[5].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getMACD: parm error" << l[5];
        return 1;
      }
      break;      
    default:
      break;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[1]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[1]));
    if (! in)
    {
      qDebug() << "ExScript::getMACD: cannot create input " << l[1];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Real macd[in->getSize()];
  TA_Real sig[in->getSize()];
  TA_Real hist[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case MACD:
      rc = TA_MACD(0, in->getSize() - 1, &input[0], fast, slow, signal, &outBeg, &outNb, &macd[0], &sig[0], &hist[0]);
      break;
    case MACDEXT:
      rc = TA_MACDEXT(0, in->getSize() - 1, &input[0], fast, (TA_MAType) maList.indexOf(l[6]), slow, (TA_MAType) maList.indexOf(l[8]),
		      signal, (TA_MAType) maList.indexOf(l[10]), &outBeg, &outNb, &macd[0], &sig[0], &hist[0]);
      break;
    case MACDFIX:
      rc = TA_MACDFIX(0, in->getSize() - 1, &input[0], signal, &outBeg, &outNb, &macd[0], &sig[0], &hist[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getMACD: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *m = new PlotLine;
  PlotLine *s = new PlotLine;
  PlotLine *h = new PlotLine;
  for (loop = outNb - 1; loop > -1; loop--)
  {
    m->prepend(macd[loop]);
    s->prepend(sig[loop]);
    h->prepend(hist[loop]);
  }

  tlines.insert(l[2], m);
  tlines.insert(l[3], s);
  tlines.insert(l[4], h);

  if (flag)
    delete in;
  
  return 0;
}

//********************************************************
//********** MINMAXINDEX *********************************
//********************************************************
int ExScript::getMINMAXINDEX (QStringList &l)
{
  // format 'FUNCTION,INPUT,OUTPUT_1,OUTPUT_2,INTEGER'

  if (l.count() != 5)
  {
    qDebug() << "ExScript::getMINMAXINDEX: parm error" << l;
    return 1;
  }
  
  bool ok;
  int period = l[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::getMINMAXINDEX: parm error" << l[4];
    return 1;
  }
  
  int flag = FALSE;
  PlotLine *in = tlines.value(l[1]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[1]));
    if (! in)
    {
      qDebug() << "ExScript::getMINMAXINDEX: cannot create input " << l[1];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Integer out[in->getSize()];
  TA_Integer out2[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  TA_RetCode rc = TA_MINMAXINDEX(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getMINMAXINDEX: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *line2 = new PlotLine;
  for (loop = outNb - 1; loop > -1; loop--)
  {
    line->prepend(out[loop]);
    line2->prepend(out2[loop]);
  }

  tlines.insert(l[2], line);
  tlines.insert(l[3], line2);

  if (flag)
    delete in;
  
  return 0;
}

//***************************************************
//********** OBV Function ***************************
//***************************************************
int ExScript::getOBV (QStringList &l)
{
  // format 'OBV,NAME,INPUT'

  if (l.count() != 3)
  {
    qDebug() << "ExScript::getOBV: parm error" << l;
    return 1;
  }
  
  int flag = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getOBV: cannot create input_1 " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  int size = in->getSize();
  if (data->count() < size)
    size = data->count();
  
  TA_Real input[size];
  TA_Real volume[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = in->getSize() - 1;
  int loop2 = data->count() - 1;
  int count = size - 1;
  for (; count > -1; loop--, loop2--, count--)
  {
    input[loop] = (TA_Real) in->getData(loop);
    volume[loop2] = (TA_Real) data->getVolume(loop2);
  }
  
  TA_RetCode rc = TA_OBV(0, size - 1, &input[0], &volume[0], &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getOBV: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  if (flag)
    delete in;

  return 0;
}


//***************************************************
//********** REF ************************************
//***************************************************
int ExScript::getREF (QStringList &l)
{
  // format 'REF,NAME,INPUT,PERIOD'

  if (l.count() != 4)
  {
    qDebug() << "ExScript::REF: parm error" << l;
    return 1;
  }

  bool ok;
  int period = l[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::REF: parm error" << l[3];
    return 1;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::REF: cannot create input " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  PlotLine *line = new PlotLine;
  int loop = 0;
  for (loop = 0; loop < in->getSize(); loop++)
  {
    if (loop - period < 0)
      continue;
      
    line->append(in->getData(loop - period));
  }

  tlines.insert(l[1], line);

  if (flag)
    delete in;
  
  return 0;
}

//********************************************************
//********** STOCH FUNCTIONS ******************************
//********************************************************
int ExScript::getSTOCH (QStringList &l, int i)
{
  // format 'STOCH,OUTPUT_SLOWK,OUTPUT_SLOWD,FASTK,SLOWK,SLOWK_MA_TYPE,SLOWD,SLOWD_MA_TYPE'
  // format 'STOCHF,OUTPUT_FASTK,OUTPUT_FASTD,FASTK,FASTD,FASTD_MA_TYPE'

  bool ok;
  int fastk = 0;
  int fastd = 0;
  int slowk = 0;
  int slowd = 0;
  switch (i)
  {
    case STOCH:
      if (l.count() != 8)
      {
        qDebug() << "ExScript::getSTOCH: parm error" << l;
        return 1;
      }
  
      fastk = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSTOCH: parm error" << l[3];
        return 1;
      }
      
      slowk = l[4].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSTOCH: parm error" << l[4];
        return 1;
      }

      slowd = l[6].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSTOCH: parm error" << l[6];
        return 1;
      }
      break;
    case STOCHF:
      if (l.count() != 6)
      {
        qDebug() << "ExScript::getSTOCH: parm error" << l;
        return 1;
      }
  
      fastk = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSTOCH: parm error" << l[3];
        return 1;
      }
      
      fastd = l[4].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSTOCH: parm error" << l[4];
        return 1;
      }

      break;
    default:
      break;
  }

  TA_Real high[data->count()];
  TA_Real low[data->count()];
  TA_Real close[data->count()];
  TA_Real out[data->count()];
  TA_Real out2[data->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case STOCH:
      rc = TA_STOCH(0, data->count() - 1, &high[0], &low[0], &close[0], fastk, slowk, (TA_MAType) maList.indexOf(l[5]),
		    slowd, (TA_MAType) maList.indexOf(l[7]), &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case STOCHF:
      rc = TA_STOCHF(0, data->count() - 1, &high[0], &low[0], &close[0], fastk, fastd, (TA_MAType) maList.indexOf(l[5]),
		    &outBeg, &outNb, &out[0], &out2[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getSTOCH: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *k = new PlotLine;
  PlotLine *d = new PlotLine;
  for (loop = outNb - 1; loop > -1; loop--)
  {
    k->prepend(out[loop]);
    d->prepend(out2[loop]);
  }

  tlines.insert(l[1], k);
  tlines.insert(l[2], d);

  return 0;
}

//********************************************************
//********** STOCHRSI FUNCTION ***************************
//********************************************************
int ExScript::getSTOCHRSI (QStringList &l)
{
  // format 'STOCHRSI,INPUT,OUTPUT_FASTK,OUTPUT_FASTD,PERIOD,FASTK,FASTD,FASTD_MA_TYPE'

  bool ok;
  if (l.count() != 8)
  {
    qDebug() << "ExScript::getSTOCHRSI: parm error" << l;
    return 1;
  }
  
  int period = l[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::getSTOCHRSI: parm error" << l[4];
    return 1;
  }
      
  int fastk = l[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::getSTOCHRSI: parm error" << l[5];
    return 1;
  }

  int fastd = l[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::getSTOCHRSI: parm error" << l[6];
    return 1;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[1]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[1]));
    if (! in)
    {
      qDebug() << "ExScript::getSTOCHRSI: cannot create input " << l[1];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Real out2[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_STOCHRSI(0, in->getSize() - 1, &input[0], period, fastk, fastd, (TA_MAType) maList.indexOf(l[7]),
		              &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getSTOCHRSI: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *k = new PlotLine;
  PlotLine *d = new PlotLine;
  for (loop = outNb - 1; loop > -1; loop--)
  {
    k->prepend(out[loop]);
    d->prepend(out2[loop]);
  }

  tlines.insert(l[1], k);
  tlines.insert(l[2], d);

  if (flag)
    delete in;
  
  return 0;
}

//********************************************************
//********** SINGLE INPUT FUNCTIONS **********************
//********************************************************
int ExScript::getSingleInput (QStringList &l, int i)
{
  // format 'FUNCTION,NAME,INPUT'

  if (l.count() != 3)
  {
    qDebug() << "ExScript::getSingleInput: parm error" << l;
    return 1;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getSingleInput: cannot create input " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case ACOS:
      rc = TA_ACOS(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case ASIN:
      rc = TA_SIN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case ATAN:
      rc = TA_ATAN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case CEIL:
      rc = TA_CEIL(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case COS:
      rc = TA_COS(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case COSH:
      rc = TA_COSH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case EXP:
      rc = TA_EXP(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case FLOOR:
      rc = TA_FLOOR(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case HT_DCPERIOD:
      rc = TA_HT_DCPERIOD(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case HT_DCPHASE:
      rc = TA_HT_DCPHASE(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case HT_TRENDLINE:
      rc = TA_HT_TRENDLINE(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case LN:
      rc = TA_LN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case LOG10:
      rc = TA_LOG10(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case SIN:
      rc = TA_SIN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case SINH:
      rc = TA_SINH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case SQRT:
      rc = TA_SQRT(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case TAN:
      rc = TA_TAN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case TANH:
      rc = TA_TANH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getSingleInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  if (flag)
    delete in;
  
  return 0;
}

//********************************************************
//********** SINGLE INPUT AND INT FUNCTIONS **********************
//********************************************************
int ExScript::getSingleInputInt (QStringList &l, int i)
{
  // format 'FUNCTION,NAME,INPUT,INTEGER'
  // format 'FUNCTION,NAME,INPUT,INTEGER,DOUBLE'
  // format 'FUNCTION,NAME,INPUT,INTEGER,MA_TYPE'
  
  bool ok;
  int period = 0;
  double dev = 0;
  switch (i)
  {
    case STDDEV:
    case VAR:
      if (l.count() != 5)
      {
        qDebug() << "ExScript::getSingleInputInt: parm error" << l;
        return 1;
      }
  
      period = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSingleInputInt: parm error" << l[3];
        return 1;
      }
      
      dev = l[4].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSingleInputInt: parm error" << l[4];
        return 1;
      }
      break;
    case MA:
      if (l.count() != 5)
      {
        qDebug() << "ExScript::getSingleInputInt: parm error" << l;
        return 1;
      }
  
      period = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSingleInputInt: parm error" << l[3];
        return 1;
      }
      break;
    default:
      if (l.count() != 4)
      {
        qDebug() << "ExScript::getSingleInputInt: parm error" << l;
        return 1;
      }
  
      period = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSingleInputInt: parm error" << l[3];
        return 1;
      }
      break;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getSingleInput: cannot create input " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case CMO:
      rc = TA_CMO(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case LINEARREG:
      rc = TA_LINEARREG(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case LINEARREG_ANGLE:
      rc = TA_LINEARREG_ANGLE(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case LINEARREG_INTERCEPT:
      rc = TA_LINEARREG_INTERCEPT(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case LINEARREG_SLOPE:
      rc = TA_LINEARREG_SLOPE(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case MA:
      rc = TA_MA(0, in->getSize() - 1, &input[0], period, (TA_MAType) maList.indexOf(l[4]), &outBeg, &outNb, &out[0]);
      break;
    case MAX:
      rc = TA_MAX(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case MIDPOINT:
      rc = TA_MIDPOINT(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case MIN:
      rc = TA_MIN(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case MOM:
      rc = TA_MOM(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ROC:
      rc = TA_ROC(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ROCP:
      rc = TA_ROCP(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ROCR:
      rc = TA_ROCR(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ROCR100:
      rc = TA_ROCR100(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case RSI:
      rc = TA_RSI(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case STDDEV:
      rc = TA_STDDEV(0, in->getSize() - 1, &input[0], period, dev, &outBeg, &outNb, &out[0]);
      break;
    case SUM:
      rc = TA_SUM(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case TRIX:
      rc = TA_TRIX(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case TSF:
      rc = TA_TSF(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case VAR:
      rc = TA_VAR(0, in->getSize() - 1, &input[0], period, dev, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getSingleInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  if (flag)
    delete in;
  
  return 0;
}

//********************************************************
//********** SINGLE INPUT AND INT FUNCTIONS **************
//********** output is integer ***************************
//********************************************************
int ExScript::getSingleInputInt2 (QStringList &l, int i)
{
  // format 'FUNCTION,NAME,INPUT,INTEGER'
  
  bool ok;
  int period = 0;
  switch (i)
  {
    case MAXINDEX:
    case MININDEX:
      if (l.count() != 4)
      {
        qDebug() << "ExScript::getSingleInputInt2: parm error" << l;
        return 1;
      }
  
      period = l[3].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getSingleInputInt2: parm error" << l[3];
        return 1;
      }
      break;
    default:
      if (l.count() != 2)
      {
        qDebug() << "ExScript::getSingleInputInt2: parm error" << l;
        return 1;
      }
      break;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getSingleInput: cannot create input " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Integer out[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case MAXINDEX:
      rc = TA_MAXINDEX(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case MININDEX:
      rc = TA_MININDEX(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case HT_TRENDMODE:
      rc = TA_HT_TRENDMODE(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getSingleInput2: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  if (flag)
    delete in;
  
  return 0;
}


//***************************************************
//********** HL Input *******************************
//***************************************************
int ExScript::getHLInput (QStringList &l, int i)
{
  // format 'FUNCTION,NAME'
  // format 'FUNCTION,NAME,INTEGER'
  // format 'SAR,NAME,ACCEL,MAX'
  // format 'SAREXT,NAME,START,OFFSET,AIL,AL,AML,AIS,AS,AMS' 10

  bool ok;
  int period = 0;
  double accel = 0;
  double max = 0;
  double start = 0;
  double offset = 0;
  double ail = 0;
  double al = 0;
  double aml = 0;
  double ais = 0;
  double as = 0;
  double ams = 0;
  switch (i)
  {
    case MIDPRICE:
    case MINUS_DM:
    case PLUS_DM:
      if (l.count() != 3)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l;
        return 1;
      }

      period = l[2].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[2];
        return 1;
      }
      break;
    case SAR:
      if (l.count() != 4)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l;
        return 1;
      }
      
      accel = l[2].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[2];
        return 1;
      }
      
      max = l[3].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[3];
        return 1;
      }
      break;
    case SAREXT:
      if (l.count() != 10)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l;
        return 1;
      }
      
      start = l[2].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[2];
        return 1;
      }
      
      offset = l[3].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[3];
        return 1;
      }
      
      ail = l[4].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[4];
        return 1;
      }
      
      al = l[5].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[5];
        return 1;
      }
      
      aml = l[6].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[6];
        return 1;
      }
      
      ais = l[7].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[7];
        return 1;
      }
      
      as = l[8].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[8];
        return 1;
      }
      
      ams = l[9].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l[9];
        return 1;
      }
      break;
    default:
      if (l.count() != 2)
      {
        qDebug() << "ExScript::getHLInput: parm error" << l;
        return 1;
      }
      break;
  }
  
  TA_Real high[data->count()];
  TA_Real low[data->count()];
  TA_Real out[data->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
  }
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case MINUS_DM:
      rc = TA_MINUS_DM(0, data->count() - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
      break;
    case MEDPRICE:
      rc = TA_MEDPRICE(0, data->count() - 1, &high[0], &low[0], &outBeg, &outNb, &out[0]);
      break;
    case MIDPRICE:
      rc = TA_MIDPRICE(0, data->count() - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
      break;
    case PLUS_DM:
      rc = TA_PLUS_DM(0, data->count() - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
      break;
    case SAR:
      rc = TA_SAR(0, data->count() - 1, &high[0], &low[0], accel, max, &outBeg, &outNb, &out[0]);
      break;
    case SAREXT:
      rc = TA_SAREXT(0, data->count() - 1, &high[0], &low[0], start, offset, ail, al, aml, ais, as, ams, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getHLInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  tlines.insert(l[1], line);

  return 0;
}

//********************************************************
//********** DOUBLE OUTPUT  ******************************
//********************************************************
int ExScript::getDoubleOutput (QStringList &l, int i)
{
  // format 'FUNCTION,INPUT,OUTPUT_1,OUTPUT_2'
  // format 'FUNCTION,INPUT,OUTPUT_1,OUTPUT_2,INTEGER'
  
  bool ok;
  int period = 0;
  switch (i)
  {
    case HT_PHASOR:
    case HT_SINE:
      if (l.count() != 4)
      {
        qDebug() << "ExScript::getDoubleOutput: parm error" << l;
        return 1;
      }

      break;
    case MINMAX:
      if (l.count() != 5)
      {
        qDebug() << "ExScript::getDoubleOutput: parm error" << l;
        return 1;
      }

      period = l[4].toInt(&ok);
      if (! ok)
      {
        qDebug() << "ExScript::getDoubleOutput: parm error" << l[4];
        return 1;
      }
      break;
    default:
      break;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[1]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[1]));
    if (! in)
    {
      qDebug() << "ExScript::getDoubleOutput: cannot create input " << l[1];
      return 1;
    }
    
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Real out2[in->getSize()];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case HT_PHASOR:
      rc = TA_HT_PHASOR(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case HT_SINE:
      rc = TA_HT_SINE(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case MINMAX:
      rc = TA_MINMAX(0, in->getSize() - 1, &input[0], period, &outBeg, &outNb, &out[0], &out2[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getDoubleOutput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *line2 = new PlotLine;
  for (loop = outNb - 1; loop > -1; loop--)
  {
    line->prepend(out[loop]);
    line2->prepend(out2[loop]);
  }

  tlines.insert(l[2], line);
  tlines.insert(l[3], line2);

  if (flag)
    delete in;
  
  return 0;
}

//***************************************************
//********** COMPARE ********************************
//***************************************************
int ExScript::getCOMPARE (QStringList &l)
{
  // format 'COMPARE,NAME,INPUT_1,INPUT_2,OPERATOR'

  if (l.count() != 5)
  {
    qDebug() << "ExScript::getCOMPARE: parm error" << l;
    return 1;
  }

  int op = opList.indexOf(l[4]);
  if (op == -1)
  {
    qDebug() << "ExScript::getCOMPARE: parm error" << l[4];
    return 1;
  }

  int flag = FALSE;
  int flag2 = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getCOMPARE: cannot create input_1 " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  PlotLine *in2 = tlines.value(l[3]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(l[3]));
    if (! in2)
    {
      qDebug() << "ExScript::getCOMPARE: cannot create input_2 " << l[3];
      if (flag)
        delete in;
      return 1;
    }
    
    flag2 = TRUE;
  }

  int loop = in->getSize() - 1;
  int loop2 = in2->getSize() - 1;

  PlotLine *line = new PlotLine;

  while (loop > -1 && loop2 > -1)
  {
    double t = in->getData(loop);
    double t2 = in2->getData(loop2);
      
    switch (op)
    {
      case 0: // equal
        if (t == t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 1: // less then
        if (t < t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 2: // less than equal
        if (t <= t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 3: // greater than
        if (t > t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 4: // greater than equal
        if (t >= t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 5: // AND
        if (t && t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 6: // OR
        if (t || t2)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }
      
    loop--;
    loop2--;
  }
  
  tlines.insert(l[1], line);

  if (flag)
    delete in;
  if (flag2)
    delete in2;
  
  return 0;
}

//***************************************************
//********** COMPARE2 ********************************
//***************************************************
int ExScript::getCOMPARE2 (QStringList &l)
{
  // format 'COMPARE,NAME,INPUT_1,VALUE,OPERATOR'

  if (l.count() != 5)
  {
    qDebug() << "ExScript::getCOMPARE2: parm error" << l;
    return 1;
  }

  int op = opList.indexOf(l[4]);
  if (op == -1)
  {
    qDebug() << "ExScript::getCOMPARE2: parm error" << l[4];
    return 1;
  }

  bool ok;
  double value = l[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::getCOMPARE2: parm error" << l[3];
    return 1;
  }

  int flag = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getCOMPARE2: cannot create input_1 " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  int loop = in->getSize() - 1;

  PlotLine *line = new PlotLine;

  while (loop > -1)
  {
    double t = in->getData(loop);
      
    switch (op)
    {
      case 0: // equal
        if (t == value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 1: // less then
        if (t < value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 2: // less than equal
        if (t <= value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 3: // greater than
        if (t > value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 4: // greater than equal
        if (t >= value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 5: // AND
        if (t && value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case 6: // OR
        if (t || value)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }
      
    loop--;
  }
  
  tlines.insert(l[1], line);

  if (flag)
    delete in;
  
  return 0;
}

//***************************************************
//********** COLOR ********************************
//***************************************************
int ExScript::getCOLOR (QStringList &l)
{
  // format 'COLOR,NAME,INPUT,INPUT_2,VALUE,COLOR'

  if (l.count() != 6)
  {
    qDebug() << "ExScript::getCOLOR: parm error" << l;
    return 1;
  }
  
  bool ok;
  double value = l[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ExScript::getCOLOR: parm error" << l[4];
    return 1;
  }
  
  QColor c(l[5]);
  if (! c.isValid())
  {
    qDebug() << "ExScript::getCOLOR: parm error" << l[5];
    return 1;
  }

  int flag = FALSE;
  int flag2 = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getCOLOR: cannot create input_1 " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  PlotLine *in2 = tlines.value(l[3]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(l[3]));
    if (! in2)
    {
      qDebug() << "ExScript::getCOLOR: cannot create input_2 " << l[3];
      if (flag)
	delete in;
      return 1;
    }
    
    flag2 = TRUE;
  }

  int inboolLoop = in->getSize() - 1;
  in2->setColorFlag(TRUE);
  int incolLoop = in2->getSize() - 1;

  PlotLine *line = new PlotLine;
  
  while (inboolLoop > -1 && incolLoop > -1)
  {
    if (in->getData(inboolLoop) == value)
      in2->setColorBar(incolLoop, c);

    inboolLoop--;
    incolLoop--;
  }

  tlines.insert(l[1], line);

  if (flag)
    delete in;
  if (flag2)
    delete in2;
  
  return 0;
}

//***************************************************
//********** NORMAL *********************************
//***************************************************
int ExScript::getNORMAL (QStringList &l)
{
  // format 'NORMAL,NAME,INPUT'

  if (l.count() != 3)
  {
    qDebug() << "ExScript::getNORMAL: parm error" << l;
    return 1;
  }
  
  int flag = FALSE;
  PlotLine *in = tlines.value(l[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(l[2]));
    if (! in)
    {
      qDebug() << "ExScript::getNORMAL: cannot create input_1 " << l[2];
      return 1;
    }
    
    flag = TRUE;
  }

  int loop = 0;
  double range = 0;
  double max = -99999999.0;
  double min = 99999999.0;
  double norm = 0;
  for (loop = 0; loop < in->getSize(); loop++)
  {
    if (in->getData(loop) > max)
      max = in->getData(loop);

    if (in->getData(loop) < min)
      min = in->getData(loop);
  }
	
  range = fabs(max) + fabs(min);

  PlotLine *line = new PlotLine;
  
  for (loop = 0; loop < in->getSize(); loop++)
  {	
    norm = ((in->getData(loop) - min) / range) * 100;
    line->append(norm);
  }

  tlines.insert(l[1], line);

  if (flag)
    delete in;
  
  return 0;
}

