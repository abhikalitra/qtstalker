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
#include "DataBase.h"

#include <QByteArray>
#include <QtDebug>
#include <math.h> // for fabs()



ExScript::ExScript ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");

  proc = 0;
  data = 0;

  opList << "=" << "<" << "<=" << ">" << ">=" << "AND" << "OR";
  
  maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA" << "MAMA" << "T3";

  indicatorList << "ACOS" << "AD" << "ADD" << "ADOSC" << "ADX" << "ADXR" << "APO" << "AROON" << "AROONOSC" << "ASIN";
  indicatorList << "ATAN" << "ATR" << "AVGPRICE" << "BARS" << "BBANDS" << "BETA" << "BOP" << "CANDLES" << "CCI";
  indicatorList << "CDL2CROWS" << "CDL3BLACKCROWS" << "CDL3INSIDE" << "CDL3LINESTRIKE" << "CDL3OUTSIDE";
  indicatorList << "CDL3STARSINSOUTH" << "CDL3WHITESOLDIERS" << "CDLABANDONEDBABY" << "CDLADVANCEBLOCK" << "CDLBELTHOLD";
  indicatorList << "CDLBREAKAWAY" << "CDLCLOSINGMARUBOZU" << "CDLCONCEALBABYSWALL" << "CDLCOUNTERATTACK" << "CDLDARKCLOUDCOVER";
  indicatorList << "CDLDOJI" << "CDLDOJISTAR" << "CDLDRAGONFLYDOJI" << "CDLENGULFING" << "CDLEVENINGDOJISTAR" << "CDLEVENINGSTAR";
  indicatorList << "CDLGAPSIDESIDEWHITE" << "CDLGRAVESTONEDOJI" << "CDLHAMMER" << "CDLHANGINGMAN" << "CDLHARAMI" << "CDLHARAMICROSS";
  indicatorList << "CDLHIGHWAVE" << "CDLHIKKAKE" << "CDLHIKKAKEMOD" << "CDLHOMINGPIGEON" << "CDLIDENTICAL3CROWS" << "CDLINNECK";
  indicatorList << "CDLINVERTEDHAMMER" << "CDLKICKING" << "CDLKICKINGBYLENGTH" << "CDLLADDERBOTTOM" << "CDLLONGLEGGEDDOJI";
  indicatorList << "CDLLONGLINE" << "CDLMARUBOZU" << "CDLMATCHINGLOW" << "CDLMATHOLD" << "CDLMORNINGDOJISTAR";
  indicatorList << "CDLMORNINGSTAR" << "CDLONNECK" << "CDLPIERCING" << "CDLRICKSHAWMAN" << "CDLRISEFALL3METHODS";
  indicatorList << "CDLSEPARATINGLINES" << "CDLSHOOTINGSTAR" << "CDLSHORTLINE" << "CDLSPINNINGTOP" << "CDLSTALLEDPATTERN";
  indicatorList << "CDLSTICKSANDWICH" << "CDLTAKURI" << "CDLTASUKIGAP" << "CDLTHRUSTING" << "CDLTRISTAR" << "CDLUNIQUE3RIVER";
  indicatorList << "CDLUPSIDEGAP2CROWS" << "CDLXSIDEGAP3METHODS" << "CEIL" << "CMO" << "COLOR" << "COMPARE" << "COMPARE2";
  indicatorList << "CORREL" << "COS" << "COSH" << "DEMA" << "DIV" << "DX" << "EMA" << "EXP" << "FLOOR" << "HT_DCPERIOD";
  indicatorList << "HT_DCPHASE" << "HT_PHASOR" << "HT_SINE" << "HT_TRENDLINE" << "HT_TRENDMODE" << "KAMA" << "LINEARREG";
  indicatorList << "LINEARREG_ANGLE" << "LINEARREG_INTERCEPT" << "LINEARREG_SLOPE" << "LN" << "LOG10" << "MACD" << "MACDEXT";
  indicatorList << "MACDFIX" << "MAMA" << "MAVP" << "MAX" << "MAXINDEX" << "MEDPRICE" << "MFI" << "MIDPOINT" << "MIDPRICE";
  indicatorList << "MIN" << "MININDEX" << "MINMAX" << "MINMAXINDEX" << "MINUS_DI" << "MINUS_DM" << "MOM" << "MULT" << "NATR";
  indicatorList << "NORMAL" << "OBV" << "PLUS_DI" << "PLUS_DM" << "PPO" << "REF" << "ROC" << "ROCP" << "ROCR" << "ROCR100";
  indicatorList << "RSI" << "SAR" << "SAREXT" << "SIN" << "SINH" << "SMA" << "SQRT" << "STDDEV" << "STOCH" << "STOCHF";
  indicatorList << "STOCHRSI" << "SUB" << "SUM" << "SYMBOL" << "T3" << "TAN" << "TANH" << "TEMA" << "TRANGE" << "TRIMA";
  indicatorList << "TRIX" << "TSF" << "TYPPRICE" << "ULTOSC" << "VAR" << "WCLPRICE" << "WILLR" << "WMA";
}

ExScript::~ExScript ()
{
  TA_Shutdown();  
}

void ExScript::getIndicatorList (QStringList &l)
{
  l = indicatorList;
}

void ExScript::setInput (BarData *bd)
{
  if (! data)
    bd->getInputFields(inputFieldList);
    
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
  connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
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

void ExScript::readFromStderr ()
{
  QByteArray ba = proc->readAllStandardError();
  QString s(ba);
  qDebug() << "ExScript::readFromStderr:" << s;
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
  if (i < 0)
    return 1;
  
  IndicatorSettings set;
  set.setIndicator(l[0]);
  
  DataBase db;
  db.getIndicatorSettings(set);
  
  if (l.count() - 1 != set.count())
  {
    qDebug() << "ExScript::parseIndicator: item count" << l.count() << set.count();
    return 1;
  }
  
  set.load(l);

  int loop;
  for (loop = 0; loop < set.count(); loop++)
  {
    QString name, val;
    int type;
    set.getItem(loop, name, type, val);
    
    switch ((IndicatorSettings::Type) type)
    {
      case IndicatorSettings::TypeInput:
      case IndicatorSettings::TypeInput2:
        if (! tlines.contains(val))
        {
          if (inputFieldList.indexOf(val) == -1)
          {
            qDebug() << "ExScript::parseIndicator: cannot create input" << val;
            return 1;
          }
        }
        break;
      case IndicatorSettings::TypeOutput:
      case IndicatorSettings::TypeOutput2:
      case IndicatorSettings::TypeOutput3:
        if (val.isEmpty())
        {
          qDebug() << "ExScript::parseIndicator: output" << name << "is empty";
          return 1;
        }
        break;
      case IndicatorSettings::TypeInt:
      {
        bool ok;
        val.toInt(&ok);
        if (! ok)
        {
          qDebug() << "ExScript::parseIndicator:" << val << "is not a integer";
          return 1;
        }
        break;
      }
      case IndicatorSettings::TypeDouble:
      {
        bool ok;
        val.toDouble(&ok);
        if (! ok)
        {
          qDebug() << "ExScript::parseIndicator:" << val << "is not valid";
          return 1;
        }
        break;
      }
      case IndicatorSettings::TypeColor:
      {
        QColor c(val);
        if (! c.isValid())
        {
          qDebug() << "ExScript::parseIndicator:" << val << "is not a color";
          return 1;
        }
        break;
      }
      case IndicatorSettings::TypeOperator:
        if (opList.indexOf(val) == -1)
        {
          qDebug() << "ExScript::parseIndicator:" << val << "is not valid";
          return 1;
        }
        break;
      case IndicatorSettings::TypeSymbol:
        if (val.isEmpty())
        {
          qDebug() << "ExScript::parseIndicator: symbol" << name << "is empty";
          return 1;
        }
        break;
      default:
        break;
    }
  }

  switch ((IndicatorName) i)
  {
    case ACOS:
    case APO:
    case ASIN:
    case ATAN:
    case BBANDS:
    case CEIL:
    case CMO:
    case COS:
    case COSH:
    case DEMA:
    case EMA:
    case EXP:
    case FLOOR:
    case HT_DCPERIOD:
    case HT_DCPHASE:
    case HT_PHASOR:
    case HT_SINE:
    case HT_TRENDLINE:
    case HT_TRENDMODE:
    case KAMA:
    case LINEARREG:
    case LINEARREG_ANGLE:
    case LINEARREG_INTERCEPT:
    case LINEARREG_SLOPE:
    case LN:
    case LOG10:
    case MACD:
    case MACDEXT:
    case MACDFIX:
    case MAMA:
    case MAX:
    case MAXINDEX:
    case MIDPOINT:
    case MIN:
    case MININDEX:
    case MINMAX:
    case MINMAXINDEX:
    case MOM:
    case PPO:
    case ROC:
    case ROCP:
    case ROCR:
    case ROCR100:
    case RSI:
    case SIN:
    case SINH:
    case SMA:
    case SQRT:
    case STDDEV:
    case STOCHRSI:
    case SUM:
    case T3:
    case TAN:
    case TANH:
    case TEMA:
    case TRIMA:
    case TSF:
    case VAR:
    case WMA:
      rc = getInput(set, i);
      break;
    case AD:
    case ADOSC:
    case ADX:
    case ADXR:
    case AROON:
    case AROONOSC:
    case ATR:
    case AVGPRICE:
    case BOP:
    case CCI:
    case DX:
    case MEDPRICE:
    case MFI:
    case MIDPRICE:
    case MINUS_DI:
    case MINUS_DM:
    case NATR:
    case PLUS_DI:
    case PLUS_DM:
    case SAR:
    case SAREXT:
    case STOCH:
    case STOCHF:
    case TRANGE:
    case TRIX:
    case TYPPRICE:
    case ULTOSC:
    case WCLPRICE:
    case WILLR:
      rc = getFieldInput(set, i);
      break;
    case ADD:
    case BETA:
    case CORREL:
    case DIV:
    case MAVP:
    case MULT:
    case OBV:
    case SUB:
      rc = getInput2(set, i);
      break;
    case BARS:
      rc = getBARS(set);
      break;
    case CANDLES:
      rc = getCANDLES(set);
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
      rc = getCDL(set, i);
      break;
    case COLOR:
      rc = getCOLOR(set);
      break;
    case COMPARE:
      rc = getCOMPARE(set);
      break;
    case COMPARE2:
      rc = getCOMPARE2(set);
      break;
    case NORMAL:
      rc = getNORMAL(set);
      break;
    case REF:
      rc = getREF(set);
      break;
    case SYMBOL:
      rc = getSYMBOL(set);
      break;
    default:
      break;
  }
  
  return rc;
}  

//**********************************************************************************
//************************ Single Input Functions **********************************
//**********************************************************************************
int ExScript::getInput (IndicatorSettings &set, int i)
{
  QString s;
  int flag = FALSE;
  TA_RetCode rc = TA_SUCCESS;
  TA_Integer outBeg;
  TA_Integer outNb;
  
  set.getItemInput(s);
  PlotLine *in = tlines.value(s);
  if (! in)
  {
    in = data->getInput(data->getInputType(s));
    flag = TRUE;
  }

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);
  
  switch ((IndicatorName) i)
  {
    case ACOS:
      rc = TA_ACOS(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case APO:
    {
      QString ma;
      set.getItemVal(4, ma);
      rc = TA_APO(0, in->getSize() - 1, &input[0], set.getItemInt(2), set.getItemInt(3), (TA_MAType) maList.indexOf(ma), &outBeg, &outNb, &out[0]);
      break;
    }
    case ASIN:
      rc = TA_ASIN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case ATAN:
      rc = TA_ATAN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case BBANDS:
    {
      TA_Real middle[in->getSize()];
      TA_Real lower[in->getSize()];
      QString ma;
      set.getItemVal(7, ma);
      rc = TA_BBANDS(0, in->getSize() - 1, &input[0], set.getItemInt(4), set.getItemDouble(5), set.getItemDouble(6), (TA_MAType) maList.indexOf(ma), &outBeg, &outNb, &out[0], &middle[0], &lower[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *m = new PlotLine;
        PlotLine *lw = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
        {
          m->append(middle[loop]);
          lw->append(lower[loop]);
        }
        set.getItemOutput2(s);
        tlines.insert(s, m);
        set.getItemOutput3(s);
        tlines.insert(s, lw);
      }
      break;
    }
    case CEIL:
      rc = TA_CEIL(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case CMO:
      rc = TA_CMO(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case COS:
      rc = TA_COS(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case COSH:
      rc = TA_COSH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case DEMA:
      rc = TA_DEMA(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case EMA:
      rc = TA_EMA(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
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
    case HT_PHASOR:
    {
      TA_Real out2[in->getSize()];
      rc = TA_HT_PHASOR(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *quad = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
          quad->append(out2[loop]);
        set.getItemOutput2(s);
        tlines.insert(s, quad);
      }
      break;
    }
    case HT_SINE:
    {
      TA_Real out2[in->getSize()];
      rc = TA_HT_SINE(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *lead = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
          lead->append(out2[loop]);
        set.getItemOutput2(s);
        tlines.insert(s, lead);
      }
      break;
    }
    case HT_TRENDLINE:
      rc = TA_HT_TRENDLINE(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case HT_TRENDMODE:
    {
      TA_Integer iout[in->getSize()];
      rc = TA_HT_TRENDMODE(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &iout[0]);
      if (rc == TA_SUCCESS)
      {
        for (loop = 0; loop < outNb; loop++)
          out[loop] = iout[loop];
      }
      break;
    }
    case KAMA:
      rc = TA_KAMA(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case LINEARREG:
      rc = TA_LINEARREG(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case LINEARREG_ANGLE:
      rc = TA_LINEARREG_ANGLE(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case LINEARREG_INTERCEPT:
      rc = TA_LINEARREG_INTERCEPT(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case LINEARREG_SLOPE:
      rc = TA_LINEARREG_SLOPE(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case LN:
      rc = TA_LN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case LOG10:
      rc = TA_LOG10(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case MACD:
    {
      TA_Real out2[in->getSize()];
      TA_Real out3[in->getSize()];
      rc = TA_MACD(0, in->getSize() - 1, &input[0], set.getItemInt(4), set.getItemDouble(5), set.getItemDouble(6), &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *sig = new PlotLine;
        PlotLine *hist = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
        {
          sig->append(out2[loop]);
          hist->append(out3[loop]);
        }
        set.getItemOutput2(s);
        tlines.insert(s, sig);
        set.getItemOutput3(s);
        tlines.insert(s, hist);
      }
      break;
    }
    case MACDEXT:
    {
      TA_Real out2[in->getSize()];
      TA_Real out3[in->getSize()];
      QString fma;
      set.getItemVal(5, fma);
      QString sma;
      set.getItemVal(7, sma);
      QString sigma;
      set.getItemVal(9, sigma);
      rc = TA_MACDEXT(0, in->getSize() - 1, &input[0], set.getItemInt(4), (TA_MAType) maList.indexOf(fma), set.getItemDouble(6), (TA_MAType) maList.indexOf(sma), set.getItemDouble(8), (TA_MAType) maList.indexOf(sigma), &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *sig = new PlotLine;
        PlotLine *hist = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
        {
          sig->append(out2[loop]);
          hist->append(out3[loop]);
        }
        set.getItemOutput2(s);
        tlines.insert(s, sig);
        set.getItemOutput3(s);
        tlines.insert(s, hist);
      }
      break;
    }
    case MACDFIX:
    {
      TA_Real out2[in->getSize()];
      TA_Real out3[in->getSize()];
      rc = TA_MACDFIX(0, in->getSize() - 1, &input[0], set.getItemInt(4), &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *sig = new PlotLine;
        PlotLine *hist = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
        {
          sig->append(out2[loop]);
          hist->append(out3[loop]);
        }
        set.getItemOutput2(s);
        tlines.insert(s, sig);
        set.getItemOutput3(s);
        tlines.insert(s, hist);
      }
      break;
    }
    case MAMA:
    {
      TA_Real out2[in->getSize()];
      rc = TA_MAMA(0, in->getSize() - 1, &input[0], set.getItemDouble(3), set.getItemDouble(4), &outBeg, &outNb, &out[0], &out2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *fama = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
          fama->append(out2[loop]);
        set.getItemOutput2(s);
        tlines.insert(s, fama);
      }
      break;
    }
    case MAX:
      rc = TA_MAX(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case MAXINDEX:
    {
      TA_Integer iout[in->getSize()];
      rc = TA_MAXINDEX(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &iout[0]);
      if (rc == TA_SUCCESS)
      {
        for (loop = 0; loop < outNb; loop++)
          out[loop] = iout[loop];
      }
      break;
    }
    case MIDPOINT:
      rc = TA_MIDPOINT(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case MIN:
      rc = TA_MIN(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case MININDEX:
    {
      TA_Integer iout[in->getSize()];
      rc = TA_MININDEX(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &iout[0]);
      if (rc == TA_SUCCESS)
      {
        for (loop = 0; loop < outNb; loop++)
          out[loop] = iout[loop];
      }
      break;
    }
    case MINMAX:
    {
      TA_Real out2[in->getSize()];
      rc = TA_MINMAX(0, in->getSize() - 1, &input[0], set.getItemInt(3), &outBeg, &outNb, &out[0], &out2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *fama = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
          fama->append(out2[loop]);
        set.getItemOutput2(s);
        tlines.insert(s, fama);
      }
      break;
    }
    case MINMAXINDEX:
    {
      TA_Integer iout[in->getSize()];
      TA_Integer iout2[in->getSize()];
      rc = TA_MINMAXINDEX(0, in->getSize() - 1, &input[0], set.getItemInt(3), &outBeg, &outNb, &iout[0], &iout2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *line2 = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
        {
          out[loop] = iout[loop];
          line2->append(iout2[loop]);
        }
        set.getItemOutput2(s);
        tlines.insert(s, line2);
      }
      break;
    }
    case MOM:
      rc = TA_MOM(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case PPO:
    {
      QString ma;
      set.getItemVal(4, ma);
      rc = TA_PPO(0, in->getSize() - 1, &input[0], set.getItemInt(2), set.getItemInt(3), (TA_MAType) maList.indexOf(ma), &outBeg, &outNb, &out[0]);
      break;
    }
    case ROC:
      rc = TA_ROC(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case ROCP:
      rc = TA_ROCP(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case ROCR:
      rc = TA_ROCR(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case ROCR100:
      rc = TA_ROCR100(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case RSI:
      rc = TA_RSI(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case SIN:
      rc = TA_SIN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case SINH:
      rc = TA_SINH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case SMA:
      rc = TA_SMA(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case SQRT:
      rc = TA_SQRT(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case STDDEV:
      rc = TA_STDDEV(0, in->getSize() - 1, &input[0], set.getItemInt(2), set.getItemDouble(3), &outBeg, &outNb, &out[0]);
      break;
    case STOCHRSI:
    {
      TA_Real out2[in->getSize()];
      QString ma;
      set.getItemVal(5, ma);
      rc = TA_STOCHRSI(0, in->getSize() - 1, &input[0], set.getItemInt(2), set.getItemInt(3), set.getItemInt(4), (TA_MAType) maList.indexOf(ma), &outBeg, &outNb, &out[0], &out2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *line2 = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
          line2->append(out2[loop]);
        set.getItemOutput2(s);
        tlines.insert(s, line2);
      }
      break;
    }
    case SUM:
      rc = TA_SUM(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case T3:
      rc = TA_T3(0, in->getSize() - 1, &input[0], set.getItemInt(2), set.getItemDouble(3), &outBeg, &outNb, &out[0]);
      break;
    case TAN:
      rc = TA_TAN(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case TANH:
      rc = TA_TANH(0, in->getSize() - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case TEMA:
      rc = TA_TEMA(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case TRIMA:
      rc = TA_TRIMA(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case TRIX:
      rc = TA_TRIX(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case TSF:
      rc = TA_TSF(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case VAR:
      rc = TA_VAR(0, in->getSize() - 1, &input[0], set.getItemInt(2), set.getItemDouble(3), &outBeg, &outNb, &out[0]);
      break;
    case WMA:
      rc = TA_WMA(0, in->getSize() - 1, &input[0], set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (flag)
    delete in;

  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  set.getItemOutput(s);
  tlines.insert(s, line);
  return 0;
}

//***************************************************
//********** Field Input Functions ******************
//***************************************************
int ExScript::getFieldInput (IndicatorSettings &set, int i)
{
  TA_Real open[data->count()];
  TA_Real high[data->count()];
  TA_Real low[data->count()];
  TA_Real close[data->count()];
  TA_Real volume[data->count()];
  TA_Real out[data->count()];
  int loop;
  for (loop = 0; loop < data->count(); loop++)
  {
    open[loop] = (TA_Real) data->getOpen(loop);
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Real) data->getVolume(loop);
  }

  QString s;
  TA_RetCode rc = TA_SUCCESS;
  TA_Integer outBeg;
  TA_Integer outNb;
  switch ((IndicatorName) i)
  {
    case AD:
      rc = TA_AD(0, data->count() - 1, &high[0], &low[0], &close[0], &volume[0], &outBeg, &outNb, &out[0]);
      break;
    case ADOSC:
      rc = TA_ADOSC(0, data->count() - 1, &high[0], &low[0], &close[0], &volume[0], set.getItemInt(1), set.getItemInt(2), &outBeg, &outNb, &out[0]);
      break;
    case ADX:
      rc = TA_ADX(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case ADXR:
      rc = TA_ADXR(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case AROON:
    {
      TA_Real out2[data->count()];
      rc = TA_AROON(0, data->count() - 1, &high[0], &low[0], set.getItemInt(2), &outBeg, &outNb, &out[0], &out2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *line = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
          line->append(out2[loop]);
        set.getItemOutput2(s);
        tlines.insert(s, line);
      }
      break;
    }
    case AROONOSC:
      rc = TA_AROONOSC(0, data->count() - 1, &high[0], &low[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case ATR:
      rc = TA_ATR(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case AVGPRICE:
      rc = TA_AVGPRICE(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case BOP:
      rc = TA_BOP(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CCI:
      rc = TA_CCI(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case DX:
      rc = TA_DX(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case MEDPRICE:
      rc = TA_MEDPRICE(0, data->count() - 1, &high[0], &low[0], &outBeg, &outNb, &out[0]);
      break;
    case MFI:
      rc = TA_MFI(0, data->count() - 1, &high[0], &low[0], &close[0], &volume[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case MIDPRICE:
      rc = TA_MIDPRICE(0, data->count() - 1, &high[0], &low[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case MINUS_DI:
      rc = TA_MINUS_DI(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case MINUS_DM:
      rc = TA_MINUS_DM(0, data->count() - 1, &high[0], &low[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case NATR:
      rc = TA_NATR(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case PLUS_DI:
      rc = TA_PLUS_DI(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case PLUS_DM:
      rc = TA_PLUS_DM(0, data->count() - 1, &high[0], &low[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    case SAR:
      rc = TA_SAR(0, data->count() - 1, &high[0], &low[0], set.getItemDouble(1), set.getItemDouble(2), &outBeg, &outNb, &out[0]);
      break;
    case SAREXT:
      rc = TA_SAREXT(0, data->count() - 1, &high[0], &low[0], set.getItemDouble(1), set.getItemDouble(2), set.getItemDouble(3), set.getItemDouble(4), set.getItemDouble(5), set.getItemDouble(6), set.getItemDouble(7), set.getItemDouble(8), &outBeg, &outNb, &out[0]);
      break;
    case STOCH:
    {
      TA_Real out2[data->count()];
      QString ma, ma2;
      set.getItemVal(4, ma);
      set.getItemVal(6, ma2);
      rc = TA_STOCH(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(2), set.getItemInt(3), (TA_MAType) maList.indexOf(ma), set.getItemInt(5), (TA_MAType) maList.indexOf(ma2), &outBeg, &outNb, &out[0], &out2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *line = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
          line->append(out2[loop]);
        set.getItemOutput2(s);
        tlines.insert(s, line);
      }
      break;
    }
    case STOCHF:
    {
      TA_Real out2[data->count()];
      QString ma;
      set.getItemVal(4, ma);
      rc = TA_STOCHF(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(2), set.getItemInt(3), (TA_MAType) maList.indexOf(ma), &outBeg, &outNb, &out[0], &out2[0]);
      if (rc == TA_SUCCESS)
      {
        PlotLine *line = new PlotLine;
        for (loop = 0; loop < outNb; loop++)
          line->append(out2[loop]);
        set.getItemOutput2(s);
        tlines.insert(s, line);
      }
      break;
    }
    case TRANGE:
      rc = TA_TRANGE(0, data->count() - 1, &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case TYPPRICE:
      rc = TA_TYPPRICE(0, data->count() - 1, &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case ULTOSC:
      rc = TA_ULTOSC(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), set.getItemInt(2), set.getItemInt(3), &outBeg, &outNb, &out[0]);
      break;
    case WCLPRICE:
      rc = TA_WCLPRICE(0, data->count() - 1, &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case WILLR:
      rc = TA_WILLR(0, data->count() - 1, &high[0], &low[0], &close[0], set.getItemInt(1), &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getFieldInput: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  set.getItemOutput(s);
  tlines.insert(s, line);
  return 0;
}

//***************************************************
//********** Double Input Functions *****************
//***************************************************
int ExScript::getInput2 (IndicatorSettings &set, int i)
{
  QString s;
  int flag = FALSE;
  int flag2 = FALSE;
  
  set.getItemInput(s);
  PlotLine *in = tlines.value(s);
  if (! in)
  {
    in = data->getInput(data->getInputType(s));
    flag = TRUE;
  }
  
  set.getItemInput2(s);
  PlotLine *in2 = tlines.value(s);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(s));
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
  
  if (flag)
    delete in;
  if (flag2)
    delete in2;
  
  TA_RetCode rc = TA_SUCCESS;
  switch (i)
  {
    case ADD:
      rc = TA_ADD(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case BETA:
      rc = TA_BETA(0, size - 1, &input[0], &input2[0], set.getItemInt(3), &outBeg, &outNb, &out[0]);
      break;
    case CORREL:
      rc = TA_CORREL(0, size - 1, &input[0], &input2[0], set.getItemInt(3), &outBeg, &outNb, &out[0]);
      break;
    case DIV:
      rc = TA_DIV(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case MAVP:
    {
      QString ma;
      set.getItemVal(5, ma);
      rc = TA_MAVP(0, size - 1, &input[0], &input2[0], set.getItemInt(3), set.getItemInt(4), (TA_MAType) maList.indexOf(ma), &outBeg, &outNb, &out[0]);
      break;
    }
    case MULT:
      rc = TA_MULT(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case OBV:
      rc = TA_OBV(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    case SUB:
      rc = TA_SUB(0, size - 1, &input[0], &input2[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
      
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ExScript::getInput2: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  set.getItemOutput(s);
  tlines.insert(s, line);
  return 0;
}

//***************************************************
//********** CDL  ***********************************
//***************************************************
int ExScript::getCDL (IndicatorSettings &set, int i)
{
  // format 'CDL,NAME'
  // format 'CDL,NAME,PENETRATION'

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
  
  switch ((IndicatorName) i)
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
      rc = TA_CDLABANDONEDBABY(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], set.getItemDouble(1), &outBeg, &outNb, &out[0]);
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
      rc = TA_CDLDARKCLOUDCOVER(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], set.getItemDouble(1), &outBeg, &outNb, &out[0]);
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
      rc = TA_CDLEVENINGDOJISTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], set.getItemDouble(1), &outBeg, &outNb, &out[0]);
      break;
    case CDLEVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], set.getItemDouble(1), &outBeg, &outNb, &out[0]);
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
      rc = TA_CDLMATHOLD(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], set.getItemDouble(1), &outBeg, &outNb, &out[0]);
      break;
    case CDLMORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], set.getItemDouble(1), &outBeg, &outNb, &out[0]);
      break;
    case CDLMORNINGSTAR:
      rc = TA_CDLMORNINGSTAR(0, data->count() - 1, &open[0], &high[0], &low[0], &close[0], set.getItemDouble(1), &outBeg, &outNb, &out[0]);
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
    qDebug() << "ExScript::getCDL: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  QString s;
  set.getItemVal(0, s);
  tlines.insert(s, line);

  return 0;
}

//***************************************************
//********** BARS ***********************************
//***************************************************
int ExScript::getBARS (IndicatorSettings &set)
{
  // format 'BARS,NAME,BAR_UP_COLOR,BAR_DOWN_COLOR,BAR_NEUTRAL_COLOR'
  
  QColor color;
  QString s;
  set.getItemVal(1, s);
  QColor barUpColor(s);

  set.getItemVal(2, s);
  QColor barDownColor(s);
  
  set.getItemVal(3, s);
  QColor barNeutralColor(s);

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

  set.getItemOutput(s);
  tlines.insert(s, line);
  
  return 0;
}

//***************************************************
//********** CANDLES ********************************
//***************************************************
int ExScript::getCANDLES (IndicatorSettings &set)
{
  // format 'CANDLES,NAME,CANDLE_COLOR'

  QString s;
  set.getItemVal(1, s);
  QColor color(s);

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

  set.getItemOutput(s);
  tlines.insert(s, line);
  
  return 0;
}

//***************************************************
//********** REF ************************************
//***************************************************
int ExScript::getREF (IndicatorSettings &set)
{
  // format 'REF,NAME,INPUT,PERIOD'

  int period = set.getItemInt(2);
  int flag = FALSE;
  QString s;
  set.getItemInput(s);
  PlotLine *in = tlines.value(s);
  if (! in)
  {
    in = data->getInput(data->getInputType(s));
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

  set.getItemOutput(s);
  tlines.insert(s, line);

  if (flag)
    delete in;
  
  return 0;
}

//***************************************************
//********** COMPARE ********************************
//***************************************************
int ExScript::getCOMPARE (IndicatorSettings &set)
{
  // format 'COMPARE,NAME,INPUT_1,INPUT_2,OPERATOR'

  QString s;
  set.getItemVal(3, s);
  int op = opList.indexOf(s);

  int flag = FALSE;
  int flag2 = FALSE;
  set.getItemInput(s);
  PlotLine *in = tlines.value(s);
  if (! in)
  {
    in = data->getInput(data->getInputType(s));
    flag = TRUE;
  }

  set.getItemInput2(s);
  PlotLine *in2 = tlines.value(s);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(s));
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

  set.getItemOutput(s);
  tlines.insert(s, line);

  if (flag)
    delete in;
  if (flag2)
    delete in2;
  
  return 0;
}

//***************************************************
//********** COMPARE2 ********************************
//***************************************************
int ExScript::getCOMPARE2 (IndicatorSettings &set)
{
  // format 'COMPARE,NAME,INPUT_1,VALUE,OPERATOR'

  QString s;
  set.getItemVal(3, s);
  int op = opList.indexOf(s);

  double value = set.getItemDouble(2);

  int flag = FALSE;
  set.getItemInput(s);
  PlotLine *in = tlines.value(s);
  if (! in)
  {
    in = data->getInput(data->getInputType(s));
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

  set.getItemOutput(s);
  tlines.insert(s, line);

  if (flag)
    delete in;
  
  return 0;
}

//***************************************************
//********** COLOR ********************************
//***************************************************
int ExScript::getCOLOR (IndicatorSettings &set)
{
  // format 'COLOR,INPUT,INPUT_2,VALUE,COLOR'

  double value = set.getItemDouble(2);

  QString s;
  set.getItemVal(3, s);
  QColor c(s);

  set.getItemInput(s);
  PlotLine *in = tlines.value(s);
  if (! in)
    return 1;

  set.getItemInput2(s);
  PlotLine *in2 = tlines.value(s);
  if (! in2)
    return 1;

  int inboolLoop = in->getSize() - 1;
  in2->setColorFlag(TRUE);
  int incolLoop = in2->getSize() - 1;

  while (inboolLoop > -1 && incolLoop > -1)
  {
    if (in->getData(inboolLoop) == value)
      in2->setColorBar(incolLoop, c);

    inboolLoop--;
    incolLoop--;
  }

  return 0;
}

//***************************************************
//********** NORMAL *********************************
//***************************************************
int ExScript::getNORMAL (IndicatorSettings &set)
{
  // format 'NORMAL,NAME,INPUT'

  int flag = FALSE;
  QString s;
  set.getItemInput(s);
  PlotLine *in = tlines.value(s);
  if (! in)
  {
    in = data->getInput(data->getInputType(s));
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

  set.getItemOutput(s);
  tlines.insert(s, line);

  if (flag)
    delete in;
  
  return 0;
}

//***************************************************
//********** SYMBOL *********************************
//***************************************************
int ExScript::getSYMBOL (IndicatorSettings &set)
{
  // format 'SYMBOL,NAME,TICKER'

  DataBase db;
  QString s;
  set.getItemVal(1, s);
  BarData *symbol = new BarData(s);
  symbol->setBarLength(data->getBarLength());
  symbol->setBarsRequested(data->count());
  db.getChart(symbol);

  PlotLine *line = new PlotLine;
  line->setScaleFlag(TRUE);
  int loop;
  for (loop = 0; loop < symbol->count(); loop++)
    line->append(symbol->getClose(loop));
  
/*
  QHash<QString, double> dict;
  int loop;
  QString ts = "Close";
  QString ts2;
  QDateTime dt;
  double val = 0;
  for (loop = 0; loop < (int) symbol->count(); loop++)
  {
    symbol->getDate(loop, dt);
    ts = dt.toString("yyyyMMddHHmmss");
    dict.insert(ts, symbol->getClose(loop));
  }

  PlotLine *line = new PlotLine;
  line->setScaleFlag(TRUE);
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    data->getDate(loop, dt);
    ts = dt.toString("yyyyMMddHHmmss");
    if (dict.contains(ts))
      line->append(dict.value(ts));
  }
*/  

  delete symbol;

  set.getItemOutput(s);
  tlines.insert(s, line);

  return 0;
}

