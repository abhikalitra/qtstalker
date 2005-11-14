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

#include "BARS.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <qinputdialog.h>
#include <math.h>


BARS::BARS ()
{
  pluginName = "BARS";
  helpFile = "bars.html";

  methodList.append("Bar");
  methodList.append("Candle");
  
  setDefaults();
}

BARS::~BARS ()
{
}

void BARS::setDefaults ()
{
  barUpColor.setNamedColor("green");
  barDownColor.setNamedColor("red");
  barNeutralColor.setNamedColor("blue");
  candleColor.setNamedColor("green");
  label = pluginName;

  maColor.setNamedColor("red");
  maColor2.setNamedColor("red");
  maColor3.setNamedColor("red");
  maLineType = PlotLine::Line;
  maLineType2 = PlotLine::Line;
  maLineType3 = PlotLine::Line;
  maLabel = "MA";
  maLabel2 = "MA2";
  maLabel3 = "MA3";
  maPeriod = 1;
  maPeriod2 = 1;
  maPeriod3 = 1;
  maType = 1;
  maType2 = 1;
  maType3 = 1;
  maInput = BarData::Close;
  maInput2 = BarData::Close;
  maInput3 = BarData::Close;

  longPercentage = 0.5;
  dojiPercentage = 0.1;
}

void BARS::calculate ()
{
  if (! method.compare("Bar"))
  {
    calculateBar();
    return;
  }

  if (! method.compare("Candle"))
  {
    calculateCandle();
    return;
  }

}

void BARS::calculateBar ()
{
  PlotLine *line = new PlotLine;
  int loop;
  QColor color;

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

    line->appendBar(color, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop));
  }

  line->setType(PlotLine::Bar);
  line->setLabel(label);
  output->addLine(line);

  calculateMA();
}

void BARS::calculateCandle ()
{
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

    line->appendBar(candleColor, o, data->getHigh(loop), data->getLow(loop), c, fillFlag);
  }

  line->setType(PlotLine::Candle);
  line->setLabel(label);
  output->addLine(line);

  calculateMA();
}

void BARS::calculateMA ()
{
  if (maPeriod > 1)
  {
    PlotLine *in = data->getInput(maInput);
    if (in)
    {
      PlotLine *ma = getMA(in, maType, maPeriod);
      ma->setColor(maColor);
      ma->setType(maLineType);
      ma->setLabel(maLabel);
      output->addLine(ma);
      delete in;
    }
  }

  if (maPeriod2 > 1)
  {
    PlotLine *in = data->getInput(maInput2);
    if (in)
    {
      PlotLine *ma = getMA(in, maType2, maPeriod2);
      ma->setColor(maColor2);
      ma->setType(maLineType2);
      ma->setLabel(maLabel2);
      output->addLine(ma);
      delete in;
    }
  }

  if (maPeriod3 > 1)
  {
    PlotLine *in = data->getInput(maInput3);
    if (in)
    {
      PlotLine *ma = getMA(in, maType3, maPeriod3);
      ma->setColor(maColor3);
      ma->setType(maLineType3);
      ma->setLabel(maLabel3);
      output->addLine(ma);
      delete in;
    }
  }
}

int BARS::indicatorPrefDialog (QWidget *w)
{
  bool newFlag = FALSE;
  if (! method.length())
    newFlag = TRUE;

  if (newFlag)
  {
    bool ok;
    QString s = QInputDialog::getItem(QObject::tr("BARS type Selection"),
                                      QObject::tr("Select a bar type:"),
                                      methodList,
                                      0,
                                      TRUE,
                                      &ok,
                                      w);
    if (ok)
    {
      method = s;
      label = method;
    }
    else
      return FALSE;
  }

  QString pl = QObject::tr("Parms");
  QString ucl = QObject::tr("Up Color");
  QString dcl = QObject::tr("Down Color");
  QString ncl = QObject::tr("Neutral Color");
  QString ccl = QObject::tr("Candle Color");
  QString ll = QObject::tr("Label");

  QString pl2 = QObject::tr("MA");
  QString macl = QObject::tr("MA Color");
  QString mall = QObject::tr("MA Label");
  QString maltl = QObject::tr("MA Line Type");
  QString mapl = QObject::tr("MA Period");
  QString matl = QObject::tr("MA Type");
  QString mail = QObject::tr("MA Input");

  QString pl3 = QObject::tr("MA2");
  QString ma2cl = QObject::tr("MA2 Color");
  QString ma2ll = QObject::tr("MA2 Label");
  QString ma2ltl = QObject::tr("MA2 Line Type");
  QString ma2pl = QObject::tr("MA2 Period");
  QString ma2tl = QObject::tr("MA2 Type");
  QString ma2il = QObject::tr("MA2 Input");

  QString pl4 = QObject::tr("MA3");
  QString ma3cl = QObject::tr("MA3 Color");
  QString ma3ll = QObject::tr("MA3 Label");
  QString ma3ltl = QObject::tr("MA3 Line Type");
  QString ma3pl = QObject::tr("MA3 Period");
  QString ma3tl = QObject::tr("MA3 Type");
  QString ma3il = QObject::tr("MA3 Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("BARS Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  while (1)
  {
    if (! method.compare("Bar"))
    {
      dialog->addColorItem(ucl, pl, barUpColor);
      dialog->addColorItem(dcl, pl, barDownColor);
      dialog->addColorItem(ncl, pl, barNeutralColor);
      break;
    }

    if (! method.compare("Candle"))
    {
      dialog->addColorItem(ccl, pl, candleColor);
      break;
    }

    break;
  }

  dialog->addTextItem(ll, pl, label);

  QStringList mal = getMATypes();

  dialog->createPage (pl2);
  dialog->addColorItem(macl, pl2, maColor);
  dialog->addTextItem(mall, pl2, maLabel);
  dialog->addComboItem(maltl, pl2, lineTypes, maLineType);
  dialog->addComboItem(matl, pl2, mal, maType);
  dialog->addIntItem(mapl, pl2, maPeriod, 1, 999999);
  dialog->addComboItem(mail, pl2, inputTypeList, maInput);

  dialog->createPage (pl3);
  dialog->addColorItem(ma2cl, pl3, maColor2);
  dialog->addTextItem(ma2ll, pl3, maLabel2);
  dialog->addComboItem(ma2ltl, pl3, lineTypes, maLineType2);
  dialog->addComboItem(ma2tl, pl3, mal, maType2);
  dialog->addIntItem(ma2pl, pl3, maPeriod2, 1, 999999);
  dialog->addComboItem(ma2il, pl3, inputTypeList, maInput2);

  dialog->createPage (pl4);
  dialog->addColorItem(ma3cl, pl4, maColor3);
  dialog->addTextItem(ma3ll, pl4, maLabel3);
  dialog->addComboItem(ma3ltl, pl4, lineTypes, maLineType3);
  dialog->addComboItem(ma3tl, pl4, mal, maType3);
  dialog->addIntItem(ma3pl, pl4, maPeriod3, 1, 999999);
  dialog->addComboItem(ma3il, pl4, inputTypeList, maInput3);
 
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    while (1)
    {
      if (! method.compare("Bar"))
      {
        barUpColor = dialog->getColor(ucl);
        barDownColor = dialog->getColor(dcl);
        barNeutralColor = dialog->getColor(ncl);
        lineType = PlotLine::Bar;
        break;
      }

      if (! method.compare("Candle"))
      {
        candleColor = dialog->getColor(ccl);
        lineType = PlotLine::Candle;
        break;
      }

      break;
    }

    label = dialog->getText(ll);

    maColor = dialog->getColor(macl);
    maLineType = (PlotLine::LineType) dialog->getComboIndex(maltl);
    maPeriod = dialog->getInt(mapl);
    maLabel = dialog->getText(mall);
    maType = dialog->getComboIndex(matl);
    maInput = (BarData::InputType) dialog->getComboIndex(mail);

    maColor2 = dialog->getColor(ma2cl);
    maLineType2 = (PlotLine::LineType) dialog->getComboIndex(ma2ltl);
    maPeriod2 = dialog->getInt(ma2pl);
    maLabel2 = dialog->getText(ma2ll);
    maType2 = dialog->getComboIndex(ma2tl);
    maInput2 = (BarData::InputType) dialog->getComboIndex(ma2il);

    maColor3 = dialog->getColor(ma3cl);
    maLineType3 = (PlotLine::LineType) dialog->getComboIndex(ma3ltl);
    maPeriod3 = dialog->getInt(ma3pl);
    maLabel3 = dialog->getText(ma3ll);
    maType3 = dialog->getComboIndex(ma3tl);
    maInput3 = (BarData::InputType) dialog->getComboIndex(ma3il);

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void BARS::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("label");
  if (s.length())
    label = s;
        
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("method");
  if (s.length())
    method = s;

  s = dict.getData("barUpColor");
  if (s.length())
    barUpColor.setNamedColor(s);
    
  s = dict.getData("barDownColor");
  if (s.length())
    barDownColor.setNamedColor(s);
    
  s = dict.getData("barNeutralColor");
  if (s.length())
    barNeutralColor.setNamedColor(s);

  s = dict.getData("candleColor");
  if (s.length())
    candleColor.setNamedColor(s);

  s = dict.getData("maColor");
  if (s.length())
    maColor.setNamedColor(s);
    
  s = dict.getData("maLineType");
  if (s.length())
    maLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("maPeriod");
  if (s.length())
    maPeriod = s.toInt();

  s = dict.getData("maLabel");
  if (s.length())
    maLabel = s;
      
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();
    
  s = dict.getData("maInput");
  if (s.length())
    maInput = (BarData::InputType) s.toInt();

  s = dict.getData("maColor2");
  if (s.length())
    maColor2.setNamedColor(s);
    
  s = dict.getData("maLineType2");
  if (s.length())
    maLineType2 = (PlotLine::LineType) s.toInt();

  s = dict.getData("maPeriod2");
  if (s.length())
    maPeriod2 = s.toInt();

  s = dict.getData("maLabel2");
  if (s.length())
    maLabel2 = s;
      
  s = dict.getData("maType2");
  if (s.length())
    maType2 = s.toInt();
    
  s = dict.getData("maInput2");
  if (s.length())
    maInput2 = (BarData::InputType) s.toInt();

  s = dict.getData("maColor3");
  if (s.length())
    maColor3.setNamedColor(s);
    
  s = dict.getData("maLineType3");
  if (s.length())
    maLineType3 = (PlotLine::LineType) s.toInt();

  s = dict.getData("maPeriod3");
  if (s.length())
    maPeriod3 = s.toInt();

  s = dict.getData("maLabel3");
  if (s.length())
    maLabel3 = s;
      
  s = dict.getData("maType3");
  if (s.length())
    maType3 = s.toInt();
    
  s = dict.getData("maInput3");
  if (s.length())
    maInput3 = (BarData::InputType) s.toInt();
}

void BARS::getIndicatorSettings (Setting &dict)
{
  dict.setData("barUpColor", barUpColor.name());
  dict.setData("barDownColor", barDownColor.name());
  dict.setData("barNeutralColor", barNeutralColor.name());
  dict.setData("candleColor", candleColor.name());
  dict.setData("label", label);
  dict.setData("method", method);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);

  dict.setData("maColor", maColor.name());
  dict.setData("maLineType", QString::number(maLineType));
  dict.setData("maPeriod", QString::number(maPeriod));
  dict.setData("maLabel", maLabel);
  dict.setData("maType", QString::number(maType));
  dict.setData("maInput", QString::number(maInput));

  dict.setData("maColor2", maColor2.name());
  dict.setData("maLineType2", QString::number(maLineType2));
  dict.setData("maPeriod2", QString::number(maPeriod2));
  dict.setData("maLabel2", maLabel2);
  dict.setData("maType2", QString::number(maType2));
  dict.setData("maInput2", QString::number(maInput2));

  dict.setData("maColor3", maColor3.name());
  dict.setData("maLineType3", QString::number(maLineType3));
  dict.setData("maPeriod3", QString::number(maPeriod3));
  dict.setData("maLabel3", maLabel3);
  dict.setData("maType3", QString::number(maType3));
  dict.setData("maInput3", QString::number(maInput3));
}

PlotLine * BARS::calculateCustom (QString &p, QPtrList<PlotLine> &)
{
  // format1 (BARS): TYPE

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 1)
    ;
  else
  {
    qDebug("BARS::calculateCustom: invalid parm count");
    return 0;
  }

  if (methodList.findIndex(l[0]) == -1)
  {
    qDebug("BARS::calculateCustom: invalid TYPE parm");
    return 0;
  }
  else
    method = l[0];

  clearOutput();
  calculate();
  return output->getLine(0);
}

int BARS::getMinBars ()
{
  return minBars;
}

//***************************************************************************
//********************** CANDLE FUNCTIONS ***********************************
//***************************************************************************

int BARS::getCandleColor (double open, double close)
{
  int rc = 0;
  if (close < open)
    rc = 1; // black
  else
  {
    if (close > open)
      rc = 2; //white
  }
  return rc;
}

bool BARS::getCandleLongBlackBody (double open, double high, double low, double close)
{
  bool rc = FALSE;
  if (open >= close)
    return rc;

  double shadow = high - low;
  double body = fabs(open - close);
  if (body >= shadow * longPercentage)
    rc = TRUE;
  return rc;
}
    
bool BARS::getCandleLongWhiteBody (double open, double high, double low, double close)
{
  bool rc = FALSE;
  if (close <= open)
    return rc;

  double shadow = high - low;
  double body = fabs(open - close);
  if (body >= shadow * longPercentage)
    rc = TRUE;
  return rc;
}

bool BARS::getCandleDoji (double open, double high, double low, double close)
{
  bool rc = FALSE;
  double shadow = high - low;
  double body = fabs(open - close);
  if (body <= shadow * dojiPercentage)
    rc = TRUE;
  return rc;
}

bool BARS::getBlackMarubozu (double open, double high, double low, double close)
{
  bool rc = FALSE;
  if (open == high && close == low)
    rc = TRUE;
  return rc;
}

bool BARS::getWhiteMarubozu (double open, double high, double low, double close)
{
  bool rc = FALSE;
  if (open == low && close == high)
    rc = TRUE;
  return rc;
}

bool BARS::getCandleEngulf (double fhigh, double flow, double shigh, double slow)
{
  bool rc = FALSE;
  if (shigh < fhigh && slow > flow)
    rc = TRUE;
  return rc;
}

bool BARS::getCBullishAbandonedBaby (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for a longblackbody
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongBlackBody(open, high, low, close))
    return rc;

  // 2 check for doji
  double open2 = data->getOpen(index - 1);
  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  double close2 = data->getClose(index - 1);
  if (! getCandleDoji(open2, high2, low2, close2))
    return rc;

  // 2 check if doji is gap down  
  if (high2 >= low)
    return rc;
  
  // 3 check for white
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 2)
    return rc;

  // 3 check if white is gap up  
  double low3 = data->getLow(index);
  if (low3 <= high2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCBearishAbandonedBaby (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for a long white body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongWhiteBody(open, high, low, close))
    return rc;

  // 2 check for doji
  double open2 = data->getOpen(index - 1);
  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  double close2 = data->getClose(index - 1);
  if (! getCandleDoji(open2, high2, low2, close2))
    return rc;

  // 2 check if doji is gap up  
  if (low2 <= high)
    return rc;
  
  // 3 check for black
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // 3 check if gap down  
  double high3 = data->getHigh(index);
  if (high3 >= low2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCConcealingBabySwallow (int index)
{
  bool rc = FALSE;
  if (index < 3)
    return rc;

  // 1 check for a blackmarubozu
  double open = data->getOpen(index - 3);
  double high = data->getHigh(index - 3);
  double low = data->getLow(index - 3);
  double close = data->getClose(index - 3);
  if (! getBlackMarubozu(open, high, low, close))
    return rc;

  // 2 check for a blackmarubozu
  double open2 = data->getOpen(index - 2);
  double high2 = data->getHigh(index - 2);
  double low2 = data->getLow(index - 2);
  double close2 = data->getClose(index - 2);
  if (! getBlackMarubozu(open2, high2, low2, close2))
    return rc;

  // 3 check for black
  double open3 = data->getOpen(index - 1);
  double close3 = data->getClose(index - 1);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // 3 check for o/c gap down
  if (open3 >= low2 || close3 >= low2)
    return rc;

  // 3 check for penetration on 2
  double high3 = data->getHigh(index - 1);
  if (high3 < low2)
    return rc;

  // 4 check for a blackmarubozu
  double open4 = data->getOpen(index);
  double high4 = data->getHigh(index);
  double low4 = data->getLow(index);
  double close4 = data->getClose(index);
  if (! getBlackMarubozu(open4, high4, low4, close4))
    return rc;

  double low3 = data->getLow(index - 1);
  if (high4 < high3 || low4 > low3)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCBullishKicking (int index)
{
  bool rc = FALSE;
  if (index < 1)
    return rc;

  // 1 check for a black marubozu
  double open = data->getOpen(index - 1);
  double high = data->getHigh(index - 1);
  double low = data->getLow(index - 1);
  double close = data->getClose(index - 1);
  if (! getBlackMarubozu(open, high, low, close))
    return rc;

  // 2 check for a white marubozu
  double open2 = data->getOpen(index);
  double high2 = data->getHigh(index);
  double low2 = data->getLow(index);
  double close2 = data->getClose(index);
  if (! getWhiteMarubozu(open2, high2, low2, close2))
    return rc;

  // 2 check if gap up
  if (low2 <= high)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCBearishKicking (int index)
{
  bool rc = FALSE;
  if (index < 1)
    return rc;

  // 1 check for a white marubozu
  double open = data->getOpen(index - 1);
  double high = data->getHigh(index - 1);
  double low = data->getLow(index - 1);
  double close = data->getClose(index - 1);
  if (! getWhiteMarubozu(open, high, low, close))
    return rc;

  // 2 check for a black marubozu
  double open2 = data->getOpen(index);
  double high2 = data->getHigh(index);
  double low2 = data->getLow(index);
  double close2 = data->getClose(index);
  if (! getBlackMarubozu(open2, high2, low2, close2))
    return rc;

  // 2 check if gap down
  if (high2 >= low)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCMatHold (int index)
{
  bool rc = FALSE;
  if (index < 4)
    return rc;

  // 1 check for long white body
  double open = data->getOpen(index - 4);
  double high = data->getHigh(index - 4);
  double low = data->getLow(index - 4);
  double close = data->getClose(index - 4);
  if (! getCandleLongWhiteBody(open, high, low, close))
    return rc;

  // 2 check for black
  double open2 = data->getOpen(index - 3);
  double close2 = data->getClose(index - 3);
  if (getCandleColor(open2, close2) != 1)
    return rc;

  // 2 check if within 1 range and higher than 1
  double high2 = data->getHigh(index - 3);
  double low2 = data->getLow(index - 3);
  if (high2 < high || low2 < low)
    return rc;

  // 3 check for black
  double open3 = data->getOpen(index - 2);
  double close3 = data->getClose(index - 2);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // 3 check if within 1 range
  double low3 = data->getLow(index - 2);
  if (low3 < low)
    return rc;

  // 4 check for black
  double open4 = data->getOpen(index - 1);
  double close4 = data->getClose(index - 1);
  if (getCandleColor(open4, close4) != 1)
    return rc;

  // 4 check if within 1 range
  double low4 = data->getLow(index - 1);
  if (low4 < low)
    return rc;

  // 5 check for long white body
  double open5 = data->getOpen(index);
  double high5 = data->getHigh(index);
  double low5 = data->getLow(index);
  double close5 = data->getClose(index);
  if (! getCandleLongWhiteBody(open5, high5, low5, close5))
    return rc;

  // 5 check if close > 1
  if (close5 <= close)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCMatchingLow (int index)
{
  bool rc = FALSE;
  if (index < 1)
    return rc;

  // 1 check for long black body
  double open = data->getOpen(index - 1);
  double high = data->getHigh(index - 1);
  double low = data->getLow(index - 1);
  double close = data->getClose(index - 1);
  if (! getCandleLongBlackBody(open, high, low, close))
    return rc;

  // 2 check for black
  double open2 = data->getOpen(index);
  double close2 = data->getClose(index);
  if (getCandleColor(open2, close2) != 1)
    return rc;

  // 2 check if 2 close = 1 close
  if (close2 != close)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCMorningDojiStar (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long black body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongBlackBody(open, high, low, close))
    return rc;

  // 2 check for doji
  double open2 = data->getOpen(index - 1);
  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  double close2 = data->getClose(index - 1);
  if (! getCandleDoji(open2, high2, low2, close2))
    return rc;

  // 2 check if doji is gap down  
  if (high2 >= low)
    return rc;

  // 3 check for white day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 2)
    return rc;

  // 3 check if 3 close in top half of 1
  if (close3 < ((high - low) / 2) + low)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCMorningStar (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long black body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongBlackBody(open, high, low, close))
    return rc;

  // 2 check if gap down  
  double high2 = data->getHigh(index - 1);
  if (high2 >= low)
    return rc;

  // 2 check for small day (50% of 1)
  double low2 = data->getLow(index - 1);
  if (high2 - low2 > (high - low) / 2)
    return rc;

  // 3 check for white day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 2)
    return rc;

  // 3 check if 3 close in top half of 1
  if (close3 < ((high - low) / 2) + low)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCSideBySideWhiteLines (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for white day
  double open = data->getOpen(index - 2);
  double close = data->getClose(index - 2);
  if (getCandleColor(open, close) != 2)
    return rc;

  // 2 check for white day
  double open2 = data->getOpen(index - 1);
  double close2 = data->getClose(index - 1);
  if (getCandleColor(open2, close2) != 2)
    return rc;

  // 2 check if gap up  
  double low2 = data->getLow(index - 1);
  double high = data->getHigh(index - 2);
  if (low2 <= high)
    return rc;

  // 3 check for white day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 2)
    return rc;

  // check if 3 high within 10% of 2 high
  double high3 = data->getHigh(index);
  double high2 = data->getHigh(index - 1);
  if (high3 < high2 * 0.9 || high3 > high2 * 1.1)
    return rc;

  // check if 3 low within 10% of 2 low
  double low3 = data->getLow(index);
  if (low3 < low2 * 0.9 || low3 > low2 * 1.1)
    return rc;

  // check if 3 close within 10% of 2 close
  if (close3 < close2 * 0.9 || close3 > close2 * 1.1)
    return rc;

  // check if 3 open within 10% of 2 open
  if (open3 < open2 * 0.9 || open3 > open2 * 1.1)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCThreeInsideUp (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long black body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongBlackBody(open, high, low, close))
    return rc;

  // 2 check for white day
  double open2 = data->getOpen(index - 1);
  double close2 = data->getClose(index - 1);
  if (getCandleColor(open2, close2) != 2)
    return rc;

  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  if (! getCandleEngulf(open, close, high2, low2))
    return rc;

  // 3 check for white day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 2)
    return rc;

  // 3 check if 3 close > 2 close
  if (close3 < close2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCThreeOutsideUp (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for black day
  double open = data->getOpen(index - 2);
  double close = data->getClose(index - 2);
  if (getCandleColor(open, close) != 1)
    return rc;

  // 2 check for white day
  double open2 = data->getOpen(index - 1);
  double close2 = data->getClose(index - 1);
  if (getCandleColor(open2, close2) != 2)
    return rc;

  // 2 check if 2 engulfs 1
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  if (high2 <= high || low2 >= low)
    return rc;

  // 3 check for white day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 2)
    return rc;

  // 3 check if 3 close > 2 high
  if (close3 <= high2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCThreeWhiteSoldiers (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long white body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongWhiteBody(open, high, low, close))
    return rc;

  // 2 check for long white body
  double open2 = data->getOpen(index - 1);
  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  double close2 = data->getClose(index - 1);
  if (! getCandleLongWhiteBody(open2, high2, low2, close2))
    return rc;

  // 2 check for open with body of 1
  if (open2 < open || open2 > close)
    return rc;
    
  // 2 check for close >  1 high
  if (close2 <= high)
    return rc;

  // 3 check for long white body
  double open3 = data->getOpen(index);
  double high3 = data->getHigh(index);
  double low3 = data->getLow(index);
  double close3 = data->getClose(index);
  if (! getCandleLongWhiteBody(open3, high3, low3, close3))
    return rc;

  // 3 check for open with body of 2
  if (open3 < open2 || open3 > close2)
    return rc;
    
  // 3 check for close >  2 high
  if (close3 <= high2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCDarkCloudCover (int index)
{
  bool rc = FALSE;
  if (index < 1)
    return rc;

  // 1 check for long white body
  double open = data->getOpen(index - 1);
  double high = data->getHigh(index - 1);
  double low = data->getLow(index - 1);
  double close = data->getClose(index - 1);
  if (! getCandleLongWhiteBody(open, high, low, close))
    return rc;

  // 2 check for black day
  double open2 = data->getOpen(index);
  double close2 = data->getClose(index);
  if (getCandleColor(open2, close2) != 1)
    return rc;

  // 2 check for open >  1 high
  if (open2 <= high)
    return rc;

  // 2 check for close < midpoint 1 body
  double midpoint = ((close - open) / 2) + open;
  if (close2 >= midpoint)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCEveningDojiStar (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long white body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongWhiteBody(open, high, low, close))
    return rc;

  // 2 check for doji
  double open2 = data->getOpen(index - 1);
  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  double close2 = data->getClose(index - 1);
  if (! getCandleDoji(open2, high2, low2, close2))
    return rc;

  // 2 check if doji is gap up  
  if (low2 <= high)
    return rc;

  // 3 check for black day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // 3 check if 3 close in bottom half of 1
  if (close3 > ((high - low) / 2) + low)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCEveningStar (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long white body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongWhiteBody(open, high, low, close))
    return rc;

  // 2 check if gap up  
  double low2 = data->getLow(index - 1);
  if (low2 <= high)
    return rc;

  // 2 check for small day (50% of 1)
  double high2 = data->getHigh(index - 1);
  if (high2 - low2 > (high - low) / 2)
    return rc;

  // 3 check for black day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // 3 check if 3 close in bottom half of 1
  if (close3 > ((high - low) / 2) + low)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCFallingThreeMethods (int index)
{
  bool rc = FALSE;
  if (index < 4)
    return rc;

  // 1 check for long black body
  double open = data->getOpen(index - 4);
  double high = data->getHigh(index - 4);
  double low = data->getLow(index - 4);
  double close = data->getClose(index - 4);
  if (! getCandleLongBlackBody(open, high, low, close))
    return rc;

  // 2 check for white day
  double open2 = data->getOpen(index - 3);
  double close2 = data->getClose(index - 3);
  if (getCandleColor(open2, close2) != 2)
    return rc;

  // 2 check if open/close within range of 1
  if (open2 > high || open2 < low || close2 < low || close2 > high)
    return rc;

  // 3 check for white day
  double open3 = data->getOpen(index - 2);
  double close3 = data->getClose(index - 2);
  if (getCandleColor(open3, close3) != 2)
    return rc;

  // 3 check if open/close within range of 1
  if (open3 > high || open3 < low || close3 < low || close3 > high)
    return rc;

  // 4 check for white day
  double open4 = data->getOpen(index - 1);
  double close4 = data->getClose(index - 1);
  if (getCandleColor(open4, close4) != 2)
    return rc;

  // 4 check if open/close within range of 1
  if (open4 > high || open4 < low || close4 < low || close4 > high)
    return rc;

  // 5 check for long black body
  double open5 = data->getOpen(index);
  double high5 = data->getHigh(index);
  double low5 = data->getLow(index);
  double close5 = data->getClose(index);
  if (! getCandleLongBlackBody(open5, high5, low5, close5))
    return rc;

  // 5 check if close < 1 close
  if (close5 >= close)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCIdenticalThreeCrows (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for black day
  double open = data->getOpen(index - 2);
  double close = data->getClose(index - 2);
  if (getCandleColor(open, close) != 1)
    return rc;

  // 2 check for black day
  double open2 = data->getOpen(index - 1);
  double close2 = data->getClose(index - 1);
  if (getCandleColor(open2, close2) != 1)
    return rc;

  // check if open is 1 close
  if (open2 != close)
    return rc;

  // 3 check for black day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // check if open is 2 close
  if (open3 != close2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCThreeBlackCrows (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long black body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongBlackBody(open, high, low, close))
    return rc;

  // 2 check for long black body
  double open2 = data->getOpen(index - 1);
  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  double close2 = data->getClose(index - 1);
  if (! getCandleLongBlackBody(open2, high2, low2, close2))
    return rc;

  // 2 check if open with 1 body
  if (open2 > high || open2 < low)
    return rc;

  // 2 check if close is < 1 close
  if (close2 >= close)
    return rc;

  // 3 check for long black body
  double open3 = data->getOpen(index);
  double high3 = data->getHigh(index);
  double low3 = data->getLow(index);
  double close3 = data->getClose(index);
  if (! getCandleLongBlackBody(open3, high3, low3, close3))
    return rc;

  // 3 check if open with 2 body
  if (open3 > high2 || open3 < low2)
    return rc;

  // 3 check if close is < 2 close
  if (close3 >= close2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCThreeInsideDown (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long white body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongWhiteBody(open, high, low, close))
    return rc;

  // 2 check for black day
  double open2 = data->getOpen(index - 1);
  double close2 = data->getClose(index - 1);
  if (getCandleColor(open2, close2) != 1)
    return rc;

  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  if (! getCandleEngulf(open, close, high2, low2))
    return rc;

  // 3 check for black day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // 3 check if 3 close > 2 low
  if (close3 >= low2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCThreeOutsideDown (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for white day
  double open = data->getOpen(index - 2);
  double close = data->getClose(index - 2);
  if (getCandleColor(open, close) != 2)
    return rc;

  // 2 check for black day
  double open2 = data->getOpen(index - 1);
  double close2 = data->getClose(index - 1);
  if (getCandleColor(open2, close2) != 1)
    return rc;

  // 2 check if 2 engulfs 1
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double high2 = data->getHigh(index - 1);
  double low2 = data->getLow(index - 1);
  if (high2 <= high || low2 >= low)
    return rc;

  // 3 check for black day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // 3 check if 3 close < 2 low
  if (close3 >= low2)
    return rc;

  rc = TRUE;
  return rc;
}

bool BARS::getCUpsideGapTwoCrows (int index)
{
  bool rc = FALSE;
  if (index < 2)
    return rc;

  // 1 check for long white body
  double open = data->getOpen(index - 2);
  double high = data->getHigh(index - 2);
  double low = data->getLow(index - 2);
  double close = data->getClose(index - 2);
  if (! getCandleLongWhiteBody(open, high, low, close))
    return rc;

  // 2 check for black day
  double open2 = data->getOpen(index - 1);
  double close2 = data->getClose(index - 1);
  if (getCandleColor(open2, close2) != 1)
    return rc;

  // 2 check 2 gaps up
  double low2 = data->getLow(index - 1);
  if (low2 <= high)
    return rc;

  // 3 check for black day
  double open3 = data->getOpen(index);
  double close3 = data->getClose(index);
  if (getCandleColor(open3, close3) != 1)
    return rc;

  // 3 check if engulfs 2
  double low3 = data->getLow(index);
  double high3 = data->getHigh(index);
  double high2 = data->getHigh(index - 1);
  if (low3 >= low2 || high3 <= high2)
    return rc;

  // 3 check if close within gap
  if (close3 <= close || close3 >= close2)
    return rc;

  rc = TRUE;
  return rc;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BARS *o = new BARS;
  return ((IndicatorPlugin *) o);
}




