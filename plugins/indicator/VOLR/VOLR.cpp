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

#include "VOLR.h"
#include "PrefDialog.h"
#include <qdict.h>

VOLR::VOLR ()
{
  pluginName = "VOLR";
  plotFlag = FALSE;
  alertFlag = TRUE;
  setDefaults();
}

VOLR::~VOLR ()
{
}

void VOLR::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  period = 14;
}

void VOLR::calculate ()
{
  QSMath *t = new QSMath(data);
  PlotLine *volr = t->getVOLR(period);
  volr->setColor(color);
  volr->setType(lineType);
  volr->setLabel(label);
  output.append(volr);
  delete t;
}

QMemArray<int> VOLR::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *line = output.at(0);

  int lineLoop;
  int listLoop = data->count() - line->getSize();
  for (lineLoop = 0; lineLoop < line->getSize(); lineLoop++, listLoop++)
  {
    if (line->getData(lineLoop) > 2)
      alerts[listLoop] = 1;
  }

  return alerts;
}

int VOLR::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("VOLR Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addComboItem(tr("Line Type"), 1, lineTypes, lineType);
  dialog->addTextItem(tr("Label"), 1, label);
  dialog->addIntItem(tr("Period"), 1, period, 1, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void VOLR::loadIndicatorSettings (QString file)
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
}

void VOLR::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("label", new QString(label));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  VOLR *o = new VOLR;
  return ((Plugin *) o);
}

