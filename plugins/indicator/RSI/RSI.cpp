/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("RSI::calculate: no input");
    return;
  }

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

  if (! customFlag)
    delete in;
}

int RSI::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("RSI Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(QObject::tr("Smoothing Type"), QObject::tr("Parms"), getMATypes(), maType);
  dialog->addIntItem(QObject::tr("Smoothing"), QObject::tr("Parms"), smoothing, 0, 99999999);
  
  if (customFlag)
    dialog->addFormulaInputItem(QObject::tr("Input"), QObject::tr("Parms"), FALSE, customInput);
  else
    dialog->addComboItem(QObject::tr("Input"), QObject::tr("Parms"), inputTypeList, input);
    
  dialog->createPage (QObject::tr("Zones"));
  dialog->addColorItem(QObject::tr("Buy Zone Color"), QObject::tr("Zones"), buyColor);
  dialog->addColorItem(QObject::tr("Sell Zone Color"), QObject::tr("Zones"), sellColor);
  dialog->addIntItem(QObject::tr("Buy Zone"), QObject::tr("Zones"), buyLine, 0, 100);
  dialog->addIntItem(QObject::tr("Sell Zone"), QObject::tr("Zones"), sellLine, 0, 100);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    period = dialog->getInt(QObject::tr("Period"));
    label = dialog->getText(QObject::tr("Label"));
    maType = dialog->getComboIndex(QObject::tr("Smoothing Type"));
    smoothing = dialog->getInt(QObject::tr("Smoothing"));
    
    if (customFlag)
      customInput = dialog->getFormulaInput(QObject::tr("Input"));
    else
      input = (BarData::InputType) dialog->getComboIndex(QObject::tr("Input"));
      
    buyColor = dialog->getColor(QObject::tr("Buy Zone Color"));
    sellColor = dialog->getColor(QObject::tr("Sell Zone Color"));
    buyLine = dialog->getInt(QObject::tr("Buy Zone"));
    sellLine = dialog->getInt(QObject::tr("Sell Zone"));
    
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

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
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
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
}

PlotLine * RSI::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
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



