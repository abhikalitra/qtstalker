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

#include "CCI.h"
#include "PrefDialog.h"
#include <math.h>
#include <qdict.h>
#include <qobject.h>

CCI::CCI ()
{
  pluginName = "CCI";
  helpFile = "cci.html";
  
  setDefaults();
}

CCI::~CCI ()
{
}

void CCI::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  smoothing = 3;
  period = 20;
  maType = IndicatorPlugin::SMA;
}

void CCI::calculate ()
{
  PlotLine *cci = new PlotLine;

  PlotLine *tp = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    tp->append((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 3);
  int tpLoop = tp->getSize() - 1;

  PlotLine *sma = getMA(tp, IndicatorPlugin::SMA, period);
  int smaLoop = sma->getSize() - 1;

  while (tpLoop >= period && smaLoop >= period)
  {
    double md = 0;
    int loop;
    for (loop = 0; loop < period; loop++)
      md = md + fabs(tp->getData(tpLoop - loop) - sma->getData(smaLoop - loop));
    md = md / period;

    double t = (tp->getData(tpLoop) - sma->getData(smaLoop)) / (0.015 * md);
    cci->prepend(t);

    tpLoop--;
    smaLoop--;
  }

  delete tp;
  delete sma;

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(cci, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete cci;
  }
  else
  {
    cci->setColor(color);
    cci->setType(lineType);
    cci->setLabel(label);
    output->addLine(cci);
  }
}

int CCI::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("CCI Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("Parms"), period, 1, 99999999);
  dialog->addIntItem(QObject::tr("Smoothing"), QObject::tr("Parms"), smoothing, 0, 99999999);
  dialog->addComboItem(QObject::tr("Smoothing Type"), QObject::tr("Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    period = dialog->getInt(QObject::tr("Period"));
    label = dialog->getText(QObject::tr("Label"));
    smoothing = dialog->getInt(QObject::tr("Smoothing"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("Smoothing Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void CCI::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("maType");
  if (s.length())
    maType = (IndicatorPlugin::MAType) s.toInt();
}

void CCI::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("plugin", pluginName);
}

PlotLine * CCI::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

int CCI::getMinBars ()
{
  int t = minBars + period + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CCI *o = new CCI;
  return ((IndicatorPlugin *) o);
}





