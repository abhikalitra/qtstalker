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

#include "SAR.h"
#include "PrefDialog.h"
#include <qdict.h>

SAR::SAR ()
{
  pluginName = "SAR";
  plotFlag = TRUE;
  alertFlag = TRUE;
  setDefaults();
}

SAR::~SAR ()
{
}

void SAR::setDefaults ()
{
  color.setNamedColor("white");
  lineType = PlotLine::Dot;
  label = pluginName;
  initial = 0.02;
  add = 0.02;
  limit = 0.2;
}

void SAR::calculate ()
{
  QSMath *t = new QSMath(data);
  PlotLine *d = t->getSAR(initial, add, limit);
  d->setColor(color);
  d->setType(lineType);
  d->setLabel(label);
  output.append(d);
  delete t;
}

QMemArray<int> SAR::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *line = output.at(0);
  int lineLoop;
  int listLoop = data->count() - line->getSize();
  int status = 0;
  for (lineLoop = 0; lineLoop < (int) line->getSize(); lineLoop++, listLoop++)
  {
    double high = data->getHigh(listLoop);
    double low = data->getLow(listLoop);

    switch (status)
    {
      case -1:
        if (line->getData(lineLoop) > high)
	  status = 1;
	break;
      case 1:
        if (line->getData(lineLoop) < low)
	  status = -1;
	break;
      default:
        if (line->getData(lineLoop) > high)
	  status = 1;
	else
	{
          if (line->getData(lineLoop) < low)
	    status = -1;
	}
	break;
    }
    
    alerts[listLoop] = status;
  }

  return alerts;
}

int SAR::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("SAR Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addFloatItem(tr("Initial"), tr("Parms"), initial, 0, 99999999);
  dialog->addFloatItem(tr("Add"), tr("Parms"), add, 0, 99999999);
  dialog->addFloatItem(tr("Limit"), tr("Parms"), limit, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    initial = dialog->getFloat(tr("Initial"));
    add = dialog->getFloat(tr("Add"));
    limit = dialog->getFloat(tr("Limit"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void SAR::loadIndicatorSettings (QString file)
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
      
  s = dict["initial"];
  if (s)
    initial = s->left(s->length()).toFloat();

  s = dict["add"];
  if (s)
    add = s->left(s->length()).toFloat();

  s = dict["limit"];
  if (s)
    limit = s->left(s->length()).toFloat();
}

void SAR::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("label", new QString(label));
  dict.replace("initial", new QString(QString::number(initial)));
  dict.replace("add", new QString(QString::number(add)));
  dict.replace("limit", new QString(QString::number(limit)));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  SAR *o = new SAR;
  return ((Plugin *) o);
}





