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

#include "MF.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

MF::MF ()
{
  pluginName = "MF";
  helpFile = "mf.html";
  
  setDefaults();
}

MF::~MF ()
{
}

void MF::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 10;
  smoothing = 10;  
  maType = 1;  
}

void MF::calculate ()
{
  PlotLine *d = new PlotLine();
  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    double pmf = 0;
    double nmf = 0;
    int loop2;
    for (loop2 = 0; loop2 < period; loop2++)
    {
      double high = data->getHigh(loop - loop2);
      double low = data->getLow(loop - loop2);
      double close = data->getClose(loop - loop2);
      double volume = data->getVolume(loop - loop2);

      double ttp = (high + low + close) / 3;
      double mf = ttp * volume;

      high = data->getHigh(loop - loop2 - 1);
      low = data->getLow(loop - loop2 - 1);
      close = data->getClose(loop - loop2 - 1);

      double ytp = (high + low + close) / 3;

      if (ttp > ytp)
        pmf = pmf + mf;

      if (ytp > ttp)
        nmf = nmf + mf;
    }

    double mfi = 100 - (100 / (1 + (pmf / nmf)));

    if (mfi > 100)
      mfi = 100;
    if (mfi < 0)
      mfi = 0;

    d->append(mfi);
  }

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(d, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete d;
  }
  else
  {
    d->setColor(color);
    d->setType(lineType);
    d->setLabel(label);
    output->addLine(d);
  }
}

int MF::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MF Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    maType = dialog->getComboIndex(stl);
    smoothing = dialog->getInt(sl);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MF::setIndicatorSettings (Setting &dict)
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
    
  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();
}

void MF::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("plugin", pluginName);
}

PlotLine * MF::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

int MF::getMinBars ()
{
  int t = minBars + period + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MF *o = new MF;
  return ((IndicatorPlugin *) o);
}






