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

#include "UO.h"
#include "PrefDialog.h"
#include <qdict.h>

UO::UO ()
{
  pluginName = "UO";
  plotFlag = FALSE;
  setDefaults();
}

UO::~UO ()
{
}

void UO::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  shortPeriod = 7;
  medPeriod = 14;
  longPeriod = 28;
}

void UO::calculate ()
{
  PlotLine *trg = getTR();

  PlotLine *atr = getMA(trg, IndicatorPlugin::SMA, shortPeriod);
  int atrLoop = atr->getSize() - 1;

  PlotLine *atr2 = getMA(trg, IndicatorPlugin::SMA, medPeriod);
  int atr2Loop = atr2->getSize() - 1;

  PlotLine *atr3 = getMA(trg, IndicatorPlugin::SMA, longPeriod);
  int atr3Loop = atr3->getSize() - 1;

  PlotLine *f = new PlotLine();

  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    f->append(data->getClose(loop) - data->getLow(loop));

  PlotLine *sma = getMA(f, IndicatorPlugin::SMA, shortPeriod);
  int smaLoop = sma->getSize() - 1;

  PlotLine *sma2 = getMA(f, IndicatorPlugin::SMA, medPeriod);
  int sma2Loop = sma2->getSize() - 1;

  PlotLine *sma3 = getMA(f, IndicatorPlugin::SMA, longPeriod);
  int sma3Loop = sma3->getSize() - 1;

  PlotLine *uo = new PlotLine();

  while (smaLoop > -1 && sma2Loop > -1 && sma3Loop > -1 && atrLoop > -1 && atr2Loop > -1 && atr3Loop > -1)
  {
    double t = (sma->getData(smaLoop) / atr->getData(atrLoop)) * 4;
    t = t + ((sma2->getData(sma2Loop) / atr2->getData(atr2Loop)) * 2);
    t = t + (sma3->getData(sma3Loop) / atr3->getData(atr3Loop));
    t = (t / 7) * 100;

    uo->prepend(t);

    smaLoop--;
    sma2Loop--;
    sma3Loop--;
    atrLoop--;
    atr2Loop--;
    atr3Loop--;
  }

  uo->setColor(color);
  uo->setType(lineType);
  uo->setLabel(label);
  output.append(uo);

  delete trg;
  delete atr;
  delete atr2;
  delete atr3;
  delete sma;
  delete sma2;
  delete sma3;
  delete f;
}

int UO::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("UO Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Short Period"), tr("Parms"), shortPeriod, 1, 99999999);
  dialog->addIntItem(tr("Medium Period"), tr("Parms"), medPeriod, 1, 99999999);
  dialog->addIntItem(tr("Long Period"), tr("Parms"), longPeriod, 1, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    shortPeriod = dialog->getInt(tr("Short Period"));
    medPeriod = dialog->getInt(tr("Medium Period"));
    longPeriod = dialog->getInt(tr("Long Period"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void UO::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void UO::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void UO::setIndicatorSettings (Setting dict)
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

  s = dict.getData("label");
  if (s.length())
    label = s;

  s = dict.getData("shortPeriod");
  if (s.length())
    shortPeriod = s.toInt();

  s = dict.getData("medPeriod");
  if (s.length())
    medPeriod = s.toInt();

  s = dict.getData("longPeriod");
  if (s.length())
    longPeriod = s.toInt();
}

Setting UO::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("shortPeriod", QString::number(shortPeriod));
  dict.setData("medPeriod", QString::number(medPeriod));
  dict.setData("longPeriod", QString::number(longPeriod));
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * UO::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

PlotLine * UO::getTR ()
{
  PlotLine *tr = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double high = data->getHigh(loop);
    double low = data->getLow(loop);
    double close;
    if (loop > 0)
      close = data->getClose(loop - 1);
    else
      close = high;

    double t = high - low;

    double t2 = fabs(high - close);
    if (t2 > t)
      t = t2;

    t2 = fabs(low - close);
    if (t2 > t)
      t = t2;

    tr->append(t);
  }
  
  return tr;
}

Plugin * create ()
{
  UO *o = new UO;
  return ((Plugin *) o);
}

