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
  alertFlag = TRUE;
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
  maType = QSMath::SMA;
  period = 100;
}

void VFI::calculate ()
{
  QSMath *t = new QSMath(data);
  PlotLine *vfi = t->getVFI(period);

  if (smoothing > 1)
  {
    PlotLine *ma = t->getMA(vfi, maType, smoothing);
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
  delete t;
}

QMemArray<int> VFI::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() == 0)
    return alerts;
  PlotLine *vfi = output.at(0);

  int dataLoop = data->count() - vfi->getSize();
  int status = 0;
  int loop;
  for (loop = 0; loop < (int) vfi->getSize(); loop++, dataLoop++)
  {
    if (vfi->getData(loop) > 0)
          status = 1;
    if (vfi->getData(loop) < 0)
	  status = -1;
    alerts[dataLoop] = status;
  }
  return alerts;
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
    maType = (QSMath::MAType) dialog->getComboIndex(tr("Smoothing Type"));
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
    maType = (QSMath::MAType) s->left(s->length()).toInt();

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

Plugin * create ()
{
  VFI *o = new VFI;
  return ((Plugin *) o);
}


