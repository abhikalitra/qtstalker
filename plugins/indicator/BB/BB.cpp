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

#include "BB.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>

BB::BB ()
{
  pluginName = "BB";
  plotFlag = TRUE;
  alertFlag = TRUE;
  setDefaults();
}

BB::~BB ()
{
}

void BB::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  deviation = 2;
  period = 20;
  maType = IndicatorPlugin::SMA;  
}

void BB::calculate ()
{
  PlotLine *in = getTP();

  PlotLine *sma = getMA(in, maType, period);
  int smaLoop = sma->getSize() - 1;

  if ((int) sma->getSize() < period * 2)
    return;

  PlotLine *bbu = new PlotLine();
  PlotLine *bbl = new PlotLine();

  int inputLoop = in->getSize() - 1;

  while (inputLoop >= period && smaLoop >= period)
  {
    int count;
    double t2 = 0;
    for (count = 0, t2 = 0; count < period; count++)
    {
      double t = in->getData(inputLoop - count) - sma->getData(smaLoop - count);
      t2 = t2 + (t * t);
    }

    double t = sqrt(t2 / period);

    bbu->prepend(sma->getData(smaLoop) + (deviation * t));
    bbl->prepend(sma->getData(smaLoop) - (deviation * t));

    inputLoop--;
    smaLoop--;
  }

  delete in;

  bbu->setColor(color);
  bbu->setType(lineType);
  bbu->setLabel(tr("BBU"));
  output.append(bbu);

  sma->setColor(color);
  sma->setType(lineType);
  sma->setLabel(tr("BBM"));
  output.append(sma);

  bbl->setColor(color);
  bbl->setType(lineType);
  bbl->setLabel(tr("BBL"));
  output.append(bbl);
}

QMemArray<int> BB::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 3)
    return alerts;

  PlotLine *bbu = output.at(0);
  PlotLine *bbl = output.at(2);

  int listLoop = data->count() - bbu->getSize() + 9;
  int bbLoop = 9;
  int status = 0;

  for (; listLoop < (int) data->count(); listLoop++, bbLoop++)
  {
    double close = data->getClose(listLoop);
    double t = close - bbl->getData(bbLoop);
    double t2 = bbu->getData(bbLoop) - bbl->getData(bbLoop);
    double t3 = t / t2;

    int loop;
    double h = 0;
    double l = 99999999;
    for (loop = 1; loop <= 9; loop++)
    {
      double high = data->getHigh(listLoop - loop);
      double low = data->getLow(listLoop - loop);

      if (high > h)
	h = high;

      if (low < l)
	l = low;
    }

    double high = data->getHigh(listLoop);
    double low = data->getLow(listLoop);
    double yclose = data->getClose(listLoop - 1);

    switch (status)
    {
      case -1:
        if ((t3 < .5) && (low < l) && (close > yclose))
	  status = 1;
	break;
      case 1:
        if ((t3 > .5) && (high > h) && (close < yclose))
	  status = -1;
	break;
      default:
        if ((t3 < .5) && (low < l) && (close > yclose))
	  status = 1;
	else
	{
          if ((t3 > .5) && (high > h) && (close < yclose))
	    status = -1;
	}
	break;
    }

    alerts[listLoop] = status;
  }

  return alerts;
}

int BB::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("BB Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addComboItem(tr("Line Type"), 1, lineTypes, lineType);
  dialog->addIntItem(tr("Period"), 1, period, 1, 99999999);
  dialog->addFloatItem(tr("Deviation"), 1, deviation, 0, 99999999);
  dialog->addComboItem(tr("MA Type"), 1, getMATypes(), maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    deviation = dialog->getFloat(tr("Deviation"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void BB::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["color"];
  if (s)
    color.setNamedColor(s->left(s->length()));
    
  s = dict["lineType"];
  if (s)
    lineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["period"];
  if (s)
    period = s->left(s->length()).toInt();

  s = dict["deviation"];
  if (s)
    deviation = s->left(s->length()).toFloat();
  
  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
}

void BB::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("deviation", new QString(QString::number(deviation)));
  dict.replace("maType", new QString(QString::number(maType)));

  saveFile(file, dict);
}

Plugin * create ()
{
  BB *o = new BB;
  return ((Plugin *) o);
}


