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
#include <qobject.h>

STOCH::STOCH ()
{
  pluginName = "STOCH";
  helpFile = "stoch.html";
  
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
  label = pluginName;
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
        double t = in->getData(loop - loop2);
        if (t > h)
          h = t;
        if (t < l)
          l = t;
      }

      double close = in->getData(loop);
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
  output->addLine(k);

  if (dperiod > 1)
  {
    PlotLine *d = getMA(k, maType, dperiod);
    d->setColor(dcolor);
    d->setType(dlineType);
    d->setLabel(dlabel);
    output->addLine(d);
  }
}

int STOCH::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("STOCH Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(QObject::tr("Smoothing Type"), QObject::tr("Parms"), maTypeList, maType);
  dialog->addFloatItem(QObject::tr("Buy Line"), QObject::tr("Parms"), buyLine, 0, 100);
  dialog->addFloatItem(QObject::tr("Sell Line"), QObject::tr("Parms"), sellLine, 0, 100);
  if (customFlag)
  {
    dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
    dialog->addFormulaInputItem(QObject::tr("Input"), QObject::tr("Parms"), FALSE, customInput);
  }
  
  dialog->createPage (QObject::tr("%K Parms"));
  dialog->addColorItem(QObject::tr("%K Color"), QObject::tr("%K Parms"), kcolor);
  dialog->addComboItem(QObject::tr("%K Line Type"), QObject::tr("%K Parms"), lineTypes, klineType);
  dialog->addTextItem(QObject::tr("%K Label"), QObject::tr("%K Parms"), klabel);
  dialog->addIntItem(QObject::tr("%K Smoothing"), QObject::tr("%K Parms"), kperiod, 0, 99999999);
  
  dialog->createPage (QObject::tr("%D Parms"));
  dialog->addColorItem(QObject::tr("%D Color"), QObject::tr("%D Parms"), dcolor);
  dialog->addComboItem(QObject::tr("%D Line Type"), QObject::tr("%D Parms"), lineTypes, dlineType);
  dialog->addTextItem(QObject::tr("%D Label"), QObject::tr("%D Parms"), dlabel);
  dialog->addIntItem(QObject::tr("%D Smoothing"), QObject::tr("%D Parms"), dperiod, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dcolor = dialog->getColor(QObject::tr("%D Color"));
    dlineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("%D Line Type"));
    dperiod = dialog->getInt(QObject::tr("%D Smoothing"));
    dlabel = dialog->getText(QObject::tr("%D Label"));
    kcolor = dialog->getColor(QObject::tr("%K Color"));
    klineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("%K Line Type"));
    kperiod = dialog->getInt(QObject::tr("%K Smoothing"));
    klabel = dialog->getText(QObject::tr("%K Label"));
    period = dialog->getInt(QObject::tr("Period"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("Smoothing Type"));
    buyLine = dialog->getFloat(QObject::tr("Buy Line"));
    sellLine = dialog->getFloat(QObject::tr("Sell Line"));
    if (customFlag)
    {
      label = dialog->getText(QObject::tr("Label"));
      customInput = dialog->getFormulaInput(QObject::tr("Input"));
    }
    
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

  s = dict.getData("label");
  if (s.length())
    label = s;
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
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * STOCH::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCH *o = new STOCH;
  return ((IndicatorPlugin *) o);
}




