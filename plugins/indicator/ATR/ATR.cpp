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

#include "ATR.h"
#include "PrefDialog.h"
#include <qdict.h>

ATR::ATR ()
{
  pluginName = "ATR";
  plotFlag = FALSE;
  setDefaults();
}

ATR::~ATR ()
{
}

void ATR::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  smoothing = 14;
  maType = IndicatorPlugin::SMA;  
}

void ATR::calculate ()
{
  PlotLine *tr = getTR();
  PlotLine *ma = getMA(tr, maType, smoothing);
  ma->setColor(color);
  ma->setType(lineType);
  ma->setLabel(label);
  output.append(ma);
  delete tr;
}

int ATR::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("ATR Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addIntItem(tr("Smoothing"), tr("Parms"), smoothing, 1, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    smoothing = dialog->getInt(tr("Smoothing"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void ATR::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["color"];
  if (s)
    color.setNamedColor(s->left(s->length()));
    
  s = dict["label"];
  if (s)
    label = s->left(s->length());
        
  s = dict["lineType"];
  if (s)
    lineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["smoothing"];
  if (s)
    smoothing = s->left(s->length()).toInt();

  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
}

void ATR::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("label", new QString(label));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("smoothing", new QString(QString::number(smoothing)));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

PlotLine * ATR::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

QString ATR::getCustomSettings ()
{
  QString s("ATR");
  s.append("," + maType);
  s.append("," + QString::number(smoothing));
  s.append("," + color.name());
  s.append("," + QString::number(lineType));
  s.append("," + label);
  return s;
}

void ATR::setCustomSettings (QString d)
{
  customFlag = TRUE;

  QStringList l = QStringList::split(",", d, FALSE);
  maType = (IndicatorPlugin::MAType) l[1].toInt();
  smoothing = l[2].toInt();
  color.setNamedColor(l[3]);
  lineType = (PlotLine::LineType) l[4].toInt();
  label = l[5];
}

Plugin * create ()
{
  ATR *o = new ATR;
  return ((Plugin *) o);
}




