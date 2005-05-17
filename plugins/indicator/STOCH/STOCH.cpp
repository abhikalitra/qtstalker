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
  kcolor.setNamedColor("red");
  buyColor.setNamedColor("gray");
  sellColor.setNamedColor("gray");
  dlineType = PlotLine::Dash;
  dlabel = "%D";
  dperiod = 3;
  klineType = PlotLine::Line;
  klabel = "%K";
  kperiod = 3;
  period = 14;
  buyLine = 20;
  sellLine = 80;
  maType = 1;
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

int STOCH::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString ll = QObject::tr("Label");
  QString perl = QObject::tr("Period");
  QString stl = QObject::tr("Smoothing Type");
  QString il = QObject::tr("Input");
  QString bzc = QObject::tr("Buy Zone Color");
  QString szc = QObject::tr("Sell Zone Color");
  QString bz = QObject::tr("Buy Zone");
  QString sz = QObject::tr("Sell Zone");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("STOCH Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  
  if (customFlag)
  {
    dialog->addTextItem(ll, pl, label);
    dialog->addFormulaInputItem(il, pl, FALSE, customInput);
  }
  
  pl = QObject::tr("%K Parms");
  dialog->createPage (pl);
  QString t = QObject::tr("%K Color");
  dialog->addColorItem(t, pl, kcolor);
  t = QObject::tr("%K Line Type");
  dialog->addComboItem(t, pl, lineTypes, klineType);
  t = QObject::tr("%K Label");
  dialog->addTextItem(t, pl, klabel);
  t = QObject::tr("%K Smoothing");
  dialog->addIntItem(t, pl, kperiod, 0, 99999999);
  
  pl = QObject::tr("%D Parms");
  dialog->createPage (pl);
  t = QObject::tr("%D Color");
  dialog->addColorItem(t, pl, dcolor);
  t = QObject::tr("%D Line Type");
  dialog->addComboItem(t, pl, lineTypes, dlineType);
  t = QObject::tr("%D Label");
  dialog->addTextItem(t, pl, dlabel);
  t = QObject::tr("%D Smoothing");
  dialog->addIntItem(t, pl, dperiod, 0, 99999999);
  
  pl = QObject::tr("Zones");
  dialog->createPage (pl);
  dialog->addColorItem(bzc, pl, buyColor);
  dialog->addColorItem(szc, pl, sellColor);
  dialog->addIntItem(bz, pl, buyLine, 0, 100);
  dialog->addIntItem(sz, pl, sellLine, 0, 100);
  
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    t = QObject::tr("%D Color");
    dcolor = dialog->getColor(t);
    t = QObject::tr("%D Line Type");
    dlineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("%D Smoothing");
    dperiod = dialog->getInt(t);
    t = QObject::tr("%D Label");
    dlabel = dialog->getText(t);
    t = QObject::tr("%K Color");
    kcolor = dialog->getColor(t);
    t = QObject::tr("%K Line Type");
    klineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("%K Smoothing");
    kperiod = dialog->getInt(t);
    t = QObject::tr("%K Label");
    klabel = dialog->getText(t);
    period = dialog->getInt(perl);
    maType = dialog->getComboIndex(stl);
    
    if (customFlag)
    {
      label = dialog->getText(ll);
      customInput = dialog->getFormulaInput(il);
    }
    
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

void STOCH::setIndicatorSettings (Setting &dict)
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
  
  s = dict.getData("buyColor");
  if (s.length())
    buyColor.setNamedColor(s);
  
  s = dict.getData("sellColor");
  if (s.length())
    sellColor.setNamedColor(s);
  
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
    maType = s.toInt();

  s = dict.getData("buyLine");
  if (s.length())
    buyLine = s.toInt();

  s = dict.getData("sellLine");
  if (s.length())
    sellLine = s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;

  s = dict.getData("label");
  if (s.length())
    label = s;
}

void STOCH::getIndicatorSettings (Setting &dict)
{
  dict.setData("dcolor", dcolor.name());
  dict.setData("buyColor", buyColor.name());
  dict.setData("sellColor", sellColor.name());
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
}

PlotLine * STOCH::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

int STOCH::getMinBars ()
{
  int t = minBars + period + dperiod + kperiod;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCH *o = new STOCH;
  return ((IndicatorPlugin *) o);
}




