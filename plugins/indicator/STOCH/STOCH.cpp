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

#include "STOCH.h"
#include "PrefDialog.h"
#include <qdict.h>

STOCH::STOCH ()
{
  pluginName = "STOCH";
  plotFlag = FALSE;
  setDefaults();
}

STOCH::~STOCH ()
{
}

void STOCH::setDefaults ()
{
  dcolor.setNamedColor("yellow");
  dlineType = PlotLine::Dash;
  dlabel = "%D";
  dperiod = 3;
  kcolor.setNamedColor("red");
  klineType = PlotLine::Line;
  klabel = "%K";
  kperiod = 3;
  period = 14;
  buyLine = 20;
  sellLine = 80;
  maType = IndicatorPlugin::SMA;  
}

void STOCH::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
  {
    in = getInputLine(customInput);
    if (! in)
    {
      qDebug("STOCH::calculate: no input");
      return;
    }
  }
  
  PlotLine *k = new PlotLine();
  int loop;
  
  if (customFlag)
  {
    for (loop = period; loop < (int) in->getSize(); loop++)
    {
      int loop2;
      double l;
      double h;
      for (loop2 = 0, l = 9999999, h = 0; loop2 < period; loop2++)
      {
        double t = data->getData(loop - loop2);
        if (t > h)
          h = t;
        if (t < l)
          l = t;
      }

      double close = data->getData(loop);
      double t = ((close - l) / (h - l)) * 100;
      if (t > 100)
        t = 100;
      if (t < 0)
        t = 0;

      k->append(t);
    }
  }
  else
  {  
    for (loop = period; loop < (int) data->count(); loop++)
    {
      int loop2;
      double l;
      double h;
      for (loop2 = 0, l = 9999999, h = 0; loop2 < period; loop2++)
      {
        double high = data->getHigh(loop - loop2);
        double low = data->getLow(loop - loop2);

        double t = high;
        if (t > h)
          h = t;

        t = low;
        if (t < l)
          l = t;
      }

      double close = data->getClose(loop);
      double t = ((close - l) / (h - l)) * 100;
      if (t > 100)
        t = 100;
      if (t < 0)
        t = 0;

      k->append(t);
    }
  }

  if (kperiod > 1)
  {
    PlotLine *k2 = getMA(k, maType, kperiod);
    delete k;
    k = k2;
  }
  
  k->setColor(kcolor);
  k->setType(klineType);
  k->setLabel(klabel);
  output.append(k);

  if (dperiod > 1)
  {
    PlotLine *d = getMA(k, maType, dperiod);
    d->setColor(dcolor);
    d->setType(dlineType);
    d->setLabel(dlabel);
    output.append(d);
  }
}

int STOCH::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("STOCH Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("Parms"), maTypeList, maType);
  dialog->addFloatItem(tr("Buy Line"), tr("Parms"), buyLine, 0, 100);
  dialog->addFloatItem(tr("Sell Line"), tr("Parms"), sellLine, 0, 100);
  if (customFlag)
    dialog->addFormulaInputItem(tr("Input"), tr("Parms"), FALSE, customInput);
  
  dialog->createPage (tr("%K Parms"));
  dialog->addColorItem(tr("%K Color"), tr("%K Parms"), kcolor);
  dialog->addComboItem(tr("%K Line Type"), tr("%K Parms"), lineTypes, klineType);
  dialog->addTextItem(tr("%K Label"), tr("%K Parms"), klabel);
  dialog->addIntItem(tr("%K Smoothing"), tr("%K Parms"), kperiod, 0, 99999999);
  
  dialog->createPage (tr("%D Parms"));
  dialog->addColorItem(tr("%D Color"), tr("%D Parms"), dcolor);
  dialog->addComboItem(tr("%D Line Type"), tr("%D Parms"), lineTypes, dlineType);
  dialog->addTextItem(tr("%D Label"), tr("%D Parms"), dlabel);
  dialog->addIntItem(tr("%D Smoothing"), tr("%D Parms"), dperiod, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dcolor = dialog->getColor(tr("%D Color"));
    dlineType = (PlotLine::LineType) dialog->getComboIndex(tr("%D Line Type"));
    dperiod = dialog->getInt(tr("%D Smoothing"));
    dlabel = dialog->getText(tr("%D Label"));
    kcolor = dialog->getColor(tr("%K Color"));
    klineType = (PlotLine::LineType) dialog->getComboIndex(tr("%K Line Type"));
    kperiod = dialog->getInt(tr("%K Smoothing"));
    klabel = dialog->getText(tr("%K Label"));
    period = dialog->getInt(tr("Period"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    buyLine = dialog->getFloat(tr("Buy Line"));
    sellLine = dialog->getFloat(tr("Sell Line"));
    if (customFlag)
      customInput = dialog->getFormulaInput(tr("Input"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void STOCH::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void STOCH::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void STOCH::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("dcolor");
  if (s.length())
    dcolor.setNamedColor(s);
    
  s = dict.getData("kcolor");
  if (s.length())
    kcolor.setNamedColor(s);
  
  s = dict.getData("dlineType");
  if (s.length())
    dlineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("klineType");
  if (s.length())
    klineType = (PlotLine::LineType) s.toInt();
  
  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("dperiod");
  if (s.length())
    dperiod = s.toInt();
  
  s = dict.getData("kperiod");
  if (s.length())
    kperiod = s.toInt();
  
  s = dict.getData("dlabel");
  if (s.length())
    dlabel = s;
      
  s = dict.getData("klabel");
  if (s.length())
    klabel = s;
  
  s = dict.getData("maType");
  if (s.length())
    maType = (IndicatorPlugin::MAType) s.toInt();

  s = dict.getData("buyLine");
  if (s.length())
    buyLine = s.toFloat();

  s = dict.getData("sellLine");
  if (s.length())
    sellLine = s.toFloat();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

Setting STOCH::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("dcolor", dcolor.name());
  dict.setData("dlineType", QString::number(dlineType));
  dict.setData("dperiod", QString::number(dperiod));
  dict.setData("dlabel", dlabel);
  dict.setData("kcolor", kcolor.name());
  dict.setData("klineType", QString::number(klineType));
  dict.setData("kperiod", QString::number(kperiod));
  dict.setData("klabel", klabel);
  dict.setData("maType", QString::number(maType));
  dict.setData("period", QString::number(period));
  dict.setData("buyLine", QString::number(buyLine));
  dict.setData("sellLine", QString::number(sellLine));
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * STOCH::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output.at(0);
}

Plugin * create ()
{
  STOCH *o = new STOCH;
  return ((Plugin *) o);
}




