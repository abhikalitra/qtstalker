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

#include "VFI.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>

VFI::VFI ()
{
  pluginName = "VFI";
  helpFile = "vfi.html";
  
  setDefaults();
}

VFI::~VFI ()
{
}

void VFI::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  smoothing = 3;
  maType = IndicatorPlugin::SMA;
  period = 100;
}

void VFI::calculate ()
{
  PlotLine *vfi = new PlotLine();
  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = data->getClose(loop-period);
    double high = data->getHigh(loop-period);
    double low = data->getLow(loop-period);
    double typical = (high+low+close)/3.0;
    for(i=loop-period+1; i<=loop; i++) {
      double ytypical = typical;
      close = data->getClose(i);
      high = data->getHigh(i);
      low = data->getLow(i);
      typical = (high+low+close)/3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta*delta;
      sma_vol += data->getVolume(i);
      }
    inter = 0.2*sqrt(inter/(double)period)*typical;
    sma_vol /= (double)period;

    close = data->getClose(loop-period);
    high = data->getHigh(loop-period);
    low = data->getLow(loop-period);
    typical = (high+low+close)/3.0;
    double t = 0;
    for(i=loop-period+1; i<=loop; i++) {
      double ytypical = typical;
      double volume = data->getVolume(i);
      close = data->getClose(i);
      high = data->getHigh(i);
      low = data->getLow(i);
      typical = (high+low+close)/3.0;

      if (typical > ytypical+inter)
        t = t + log(1.0 + volume/sma_vol);
      else
      {
        if (typical < ytypical-inter)
          t = t - log(1.0 + volume/sma_vol);
      }
    }

    vfi->append(t);
  }

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(vfi, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete vfi;
  }
  else
  {
    vfi->setColor(color);
    vfi->setType(lineType);
    vfi->setLabel(label);
    output->addLine(vfi);
  }
}

int VFI::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("VFI Indicator"));
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
    label = dialog->getText(QObject::tr("Label"));
    period = dialog->getInt(QObject::tr("Period"));
    smoothing = dialog->getInt(QObject::tr("Smoothing"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("Smoothing Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VFI::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();

  s = dict.getData("maType");
  if (s.length())
    maType = (IndicatorPlugin::MAType) s.toInt();

}

void VFI::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("period", QString::number(period));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("maType", QString::number(maType));
  dict.setData("plugin", pluginName);
}

PlotLine * VFI::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

int VFI::getMinBars ()
{
  int t = minBars + period + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VFI *o = new VFI;
  return ((IndicatorPlugin *) o);
}


