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

#include "DPO.h"
#include "PrefDialog.h"
#include <qdict.h>

DPO::DPO ()
{
  pluginName = "DPO";
  plotFlag = FALSE;
  setDefaults();
}

DPO::~DPO ()
{
}

void DPO::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 21;
  maType = IndicatorPlugin::SMA;
}

void DPO::calculate ()
{
  PlotLine *c = data->getInput(BarData::Close);

  PlotLine *ma = getMA(c, maType, period);

  PlotLine *dpo = new PlotLine();
  dpo->setColor(color);
  dpo->setType(lineType);
  dpo->setLabel(label);
  output.append(dpo);

  int maLoop = ma->getSize() - 1;
  int closeLoop = c->getSize() - 1;
  int t = (int) ((period / 2) + 1);

  while (maLoop >= t)
  {
    dpo->prepend(c->getData(closeLoop) - ma->getData(maLoop - t));
    closeLoop--;
    maLoop--;
  }

  delete c;
  delete ma;
}

int DPO::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("DPO Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void DPO::loadIndicatorSettings (QString file)
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
}

void DPO::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("label", new QString(label));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

PlotLine * DPO::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

QString DPO::getCustomSettings ()
{
  QString s("DPO");
  s.append("," + maType);
  s.append("," + QString::number(period));
  s.append("," + color.name());
  s.append("," + QString::number(lineType));
  s.append("," + label);
  return s;
}

void DPO::setCustomSettings (QString d)
{
  customFlag = TRUE;

  QStringList l = QStringList::split(",", d, FALSE);
  maType = (IndicatorPlugin::MAType) l[1].toInt();
  period = l[2].toInt();
  color.setNamedColor(l[3]);
  lineType = (PlotLine::LineType) l[4].toInt();
  label = l[5];
}

Plugin * create ()
{
  DPO *o = new DPO;
  return ((Plugin *) o);
}

