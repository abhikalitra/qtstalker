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
#include "PrefDialog.h"
#include <qdict.h>

BB::BB ()
{
  pluginName = "BB";
  plotFlag = TRUE;
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
  maType = QSMath::SMA;  
}

void BB::calculate ()
{
  QSMath *t = new QSMath(data);
  
  PlotLine *bbu = t->getBB(maType, period, deviation, 1);
  bbu->setColor(color);
  bbu->setType(lineType);
  bbu->setLabel(tr("BBU"));
  
  PlotLine *bbl = t->getBB(maType, period, deviation, 0);
  bbl->setColor(color);
  bbl->setType(lineType);
  bbl->setLabel(tr("BBL"));
  
  PlotLine *in = t->getTP();
  PlotLine *sma = t->getMA(in, maType, period);
  sma->setColor(color);
  sma->setType(lineType);
  sma->setLabel(tr("BBM"));
  delete in;

  output.append(bbu);
  output.append(sma);
  output.append(bbl);
  delete t;
}

int BB::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("BB Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addFloatItem(tr("Deviation"), tr("Parms"), deviation, 0, 99999999);
  dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    maType = (QSMath::MAType) dialog->getComboIndex(tr("MA Type"));
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
    maType = (QSMath::MAType) s->left(s->length()).toInt();
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
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  BB *o = new BB;
  return ((Plugin *) o);
}


