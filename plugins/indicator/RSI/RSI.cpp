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

#include "RSI.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

RSI::RSI ()
{
  pluginName = "RSI";
  helpFile = "rsi.html";
  
  setDefaults();
}

RSI::~RSI ()
{
}

void RSI::setDefaults ()
{
  color.setNamedColor("red");
  buyColor.setNamedColor("gray");
  sellColor.setNamedColor("gray");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 14;
  smoothing = 10;  
  maType = 1;  
  input = BarData::Close;
  buyLine = 30;
  sellLine = 70;
}

void RSI::calculate ()
{
  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("RSI::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void RSI::calculate2 (PlotLine *in)
{
  PlotLine *rsi = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
  {
    double loss = 0;
    double gain = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double t = in->getData(loop - loop2) - in->getData(loop - loop2 - 1);
      if (t > 0)
        gain = gain + t;
      if (t < 0)
        loss = loss + fabs(t);
    }

    double again = gain / period;
    double aloss = loss / period;
    double rs = again / aloss;
    double t = 100 - (100 / (1 + rs));
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    rsi->append(t);
  }
  
  if (smoothing > 1)
  {
    PlotLine *ma = getMA(rsi, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete rsi;
  }
  else
  {
    rsi->setColor(color);
    rsi->setType(lineType);
    rsi->setLabel(label);
    output->addLine(rsi);
  }
  
  if (buyLine)
  {
    PlotLine *bline = new PlotLine();
    bline->setColor(buyColor);
    bline->setType(PlotLine::Horizontal);
    bline->append(buyLine);
    output->addLine(bline);
  }
  
  if (sellLine)
  {
    PlotLine *sline = new PlotLine();
    sline->setColor(sellColor);
    sline->setType(PlotLine::Horizontal);
    sline->append(sellLine);
    output->addLine(sline);
  }
}

int RSI::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");
  QString il = QObject::tr("Input");
  QString bzc = QObject::tr("Buy Zone Color");
  QString szc = QObject::tr("Sell Zone Color");
  QString bz = QObject::tr("Buy Zone");
  QString sz = QObject::tr("Sell Zone");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("RSI Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  dialog->addComboItem(il, pl, inputTypeList, input);
    
  pl = QObject::tr("Zones");
  dialog->createPage (pl);
  dialog->addColorItem(bzc, pl, buyColor);
  dialog->addColorItem(szc, pl, sellColor);
  dialog->addIntItem(bz, pl, buyLine, 0, 100);
  dialog->addIntItem(sz, pl, sellLine, 0, 100);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    maType = dialog->getComboIndex(stl);
    smoothing = dialog->getInt(sl);
    input = (BarData::InputType) dialog->getComboIndex(il);
      
    buyColor = dialog->getColor(bzc);
    sellColor = dialog->getColor(szc);
    buyLine = dialog->getInt(bz);
    sellLine = dialog->getInt(sz);
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void RSI::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("buyColor");
  if (s.length())
    buyColor.setNamedColor(s);
  
  s = dict.getData("sellColor");
  if (s.length())
    sellColor.setNamedColor(s);
  
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();
    
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
    
  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();

  s = dict.getData("buyLine");
  if (s.length())
    buyLine = s.toInt();

  s = dict.getData("sellLine");
  if (s.length())
    sellLine = s.toInt();
}

void RSI::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("buyColor", buyColor.name());
  dict.setData("sellColor", sellColor.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("input", QString::number(input));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("buyLine", QString::number(buyLine));
  dict.setData("sellLine", QString::number(sellLine));
  dict.setData("plugin", pluginName);
}

PlotLine * RSI::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, MA_TYPE, PERIOD, SMOOTHING

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 4)
    ;
  else
  {
    qDebug("RSI::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("RSI::calculateCustom: no input");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[1]) == -1)
  {
    qDebug("RSI::calculateCustom: invalid MA_TYPE parm");
    return 0;
  }
  else
    maType = mal.findIndex(l[1]);

  bool ok;
  int t = l[2].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("RSI::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  t = l[3].toInt(&ok);
  if (ok)
    smoothing = t;
  else
  {
    qDebug("RSI::calculateCustom: invalid SMOOTHING parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

int RSI::getMinBars ()
{
  int t = minBars + period + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  RSI *o = new RSI;
  return ((IndicatorPlugin *) o);
}



