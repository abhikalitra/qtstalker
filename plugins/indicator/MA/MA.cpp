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

#include "MA.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

MA::MA ()
{
  pluginName = "MA";
  helpFile = "ma.html";
  
  setDefaults();
}

MA::~MA ()
{
}

void MA::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 10;
  maType = (int) SMA;  
  input = BarData::Close;
  customInput = "1";
  maTypeList = getMATypes();
}

void MA::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("MA::calculate: no input");
    return;
  }
  
  PlotLine *ma = getMA(in, maType, period);
  
  if (! customFlag)
    delete in;

  ma->setColor(color);
  ma->setType(lineType);
  ma->setLabel(label);
  output->addLine(ma);
}

int MA::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString stl = QObject::tr("MA Type");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MA Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addComboItem(stl, pl, maTypeList, maType);
  if (customFlag)
    dialog->addFormulaInputItem(il, pl, FALSE, customInput);
  else
    dialog->addComboItem(il, pl, inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    maType = dialog->getComboIndex(stl);
    if (customFlag)
      customInput = dialog->getFormulaInput(il);
    else
      input = (BarData::InputType) dialog->getComboIndex(il);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MA::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
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

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

void MA::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("input", QString::number(input));
  dict.setData("plugin", pluginName);
  dict.setData("customInput", customInput);
}

PlotLine * MA::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  if (output->getLines())
    return output->getLine(0);
  else
    return 0;
}

int MA::getMinBars ()
{
  int t = minBars + period;
  return t;
}

PlotLine * MA::getEMA (PlotLine *d, int period)
{
  PlotLine *ema = new PlotLine;

  if (period >= (int) d->getSize())
    return ema;

  if (period < 1)
    return ema;

  double smoother = 2.0 / (period + 1);

  double t = 0;
  int loop;
  for (loop = 0; loop < period; loop++)
    t = t + d->getData(loop);

  double yesterday = t / period;
  ema->append(yesterday);

  for (; loop < (int) d->getSize(); loop++)
  {
    double t  = (smoother * (d->getData(loop) - yesterday)) + yesterday;
    yesterday = t;
    ema->append(t);
  }

  return ema;
}

// NEW CODE - SINGLE PASS SMA
PlotLine * MA::getSMA (PlotLine *d, int period)
{
  PlotLine *sma = new PlotLine;

  int size = d->getSize();

  // weed out degenerate cases
  
  if (period < 1 || period >= size)	// STEVE: should be period > size
    return sma;				// left this way to keep behaviour

  // create the circular buffer and its running total
  
  double *values = new double[period];
  double total = 0.0;
  
  // fill buffer first time around, keeping its running total

  int loop = -1;
  while (++loop < period) {
    double val = d->getData(loop);
    total += val;
    values[loop] = val;
  }

  // buffer filled with first period values, output first sma value
  
  sma->append(total / period);

  // loop over the rest, each time replacing oldest value in buffer
 
  --loop;
  while (++loop < size) 
  {
    int index = loop % period;
    double newval = d->getData(loop);
    
    total += newval;
    total -= values[index];
    values[index] = newval;

    sma->append(total / period);
  }
 
  // clean up 
  
  delete values;
	
  return sma;
}

PlotLine * MA::getWMA (PlotLine *d, int period)
{
  PlotLine *wma = new PlotLine;

  if (period >= (int) d->getSize())
    return wma;

  if (period < 1)
    return wma;

  int loop;
  for (loop = period - 1; loop < (int) d->getSize(); loop++)
  {
    int loop2;
    int weight;
    int divider;
    double total;
    for (loop2 = period - 1, weight = 1, divider = 0, total = 0; loop2 >= 0; loop2--, weight++)
    {
      total = total + (d->getData(loop - loop2) * weight);
      divider = divider + weight;
    }

    wma->append(total / divider);
  }

  return wma;
}

PlotLine * MA::getWilderMA (PlotLine *d, int period)
{
  PlotLine *wilderma = new PlotLine;

  if (period >= (int) d->getSize())
    return wilderma;

  if (period < 1)
    return wilderma;

  double t = 0;
  int loop;
  for (loop = 0; loop < period; loop++)
    t = t + d->getData(loop);

  double yesterday = t / period;

  wilderma->append(yesterday);

  for (; loop < (int) d->getSize(); loop++)
  {
    double t  = (yesterday * (period - 1) + d->getData(loop))/period;
    yesterday = t;
    wilderma->append(t);
  }

  return wilderma;
}

PlotLine * MA::getMA (PlotLine *in, int type, int period)
{
  PlotLine *ma = 0;
  
  switch (type)
  {
    case SMA:
      ma = getSMA(in, period);
      break;
    case EMA:
      ma = getEMA(in, period);
      break;
    case WMA:
      ma = getWMA(in, period);
      break;
    case Wilder:
      ma = getWilderMA(in, period);
      break;
    default:
      break;    
  }
  
  return ma;  
}

QStringList MA::getMATypes ()
{
  QStringList l;
  l.append("EMA");
  l.append("SMA");
  l.append("WMA");
  l.append("Wilder");
  return l;
}

int MA::getMAType (QString d)
{
  int type = (int) SMA;
  
  while (1)
  {
    if (! d.compare("EMA"))
    {
      type = (int) EMA;
      break;
    }
    
    if (! d.compare("WMA"))
    {
      type = (int) WMA;
      break;
    }
  
    if (! d.compare("Wilder"))
      type = (int) Wilder;
      
    break;
  }
  
  return type;
}

//*************************************************************************
//*************************************************************************
//*************************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MA *o = new MA;
  return ((IndicatorPlugin *) o);
}


