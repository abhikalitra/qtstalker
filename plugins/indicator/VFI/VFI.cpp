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

VFI::VFI ()
{
  pluginName = "VFI";
  plotFlag = FALSE;
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
    output.append(ma);
    delete vfi;
  }
  else
  {
    vfi->setColor(color);
    vfi->setType(lineType);
    vfi->setLabel(label);
    output.append(vfi);
  }
}

int VFI::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("VFI Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addIntItem(tr("Smoothing"), tr("Parms"), smoothing, 0, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    period = dialog->getInt(tr("Period"));
    smoothing = dialog->getInt(tr("Smoothing"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VFI::loadIndicatorSettings (QString file)
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

  s = dict["label"];
  if (s)
    label = s->left(s->length());

  s = dict["period"];
  if (s)
    period = s->left(s->length()).toInt();

  s = dict["smoothing"];
  if (s)
    smoothing = s->left(s->length()).toInt();

  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();

}

void VFI::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("label", new QString(label));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("smoothing", new QString(QString::number(smoothing)));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

PlotLine * VFI::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

QString VFI::getCustomSettings ()
{
  QString s("VFI");
  s.append("," + QString::number(maType));
  s.append("," + QString::number(period));
  s.append("," + QString::number(smoothing));
  s.append("," + color.name());
  s.append("," + QString::number(lineType));
  s.append("," + label);
  return s;
}

void VFI::setCustomSettings (QString d)
{
  customFlag = TRUE;
  QStringList l = QStringList::split(",", d, FALSE);
  maType = (IndicatorPlugin::MAType) l[1].toInt();
  period = l[2].toInt();
  smoothing = l[3].toInt();
  color.setNamedColor(l[4]);
  lineType = (PlotLine::LineType) l[5].toInt();
  label = l[6];
}

Plugin * create ()
{
  VFI *o = new VFI;
  return ((Plugin *) o);
}


