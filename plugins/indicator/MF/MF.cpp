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

#include "MF.h"
#include "PrefDialog.h"
#include <qdict.h>

MF::MF ()
{
  pluginName = "MF";
  plotFlag = FALSE;
  alertFlag = FALSE;
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
  maType = IndicatorPlugin::SMA;  
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
    output.append(ma);
    delete d;
  }
  else
  {
    d->setColor(color);
    d->setType(lineType);
    d->setLabel(label);
    output.append(d);
  }
}

int MF::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("MF Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addComboItem(tr("Line Type"), 1, lineTypes, lineType);
  dialog->addTextItem(tr("Label"), 1, label);
  dialog->addIntItem(tr("Period"), 1, period, 1, 99999999);
  dialog->addIntItem(tr("Smoothing"), 1, smoothing, 0, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), 1, getMATypes(), maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    smoothing = dialog->getInt(tr("Smoothing"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MF::loadIndicatorSettings (QString file)
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

  s = dict["label"];
  if (s)
    label = s->left(s->length());
      
  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
    
  s = dict["smoothing"];
  if (s)
    smoothing = s->left(s->length()).toInt();
}

void MF::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("label", new QString(label));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("smoothing", new QString(QString::number(smoothing)));

  saveFile(file, dict);
}

Plugin * create ()
{
  MF *o = new MF;
  return ((Plugin *) o);
}






