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
  alertFlag = TRUE;
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
  maType = QSMath::SMA;  
}

void ATR::calculate ()
{
  QSMath *t = new QSMath(data);

  PlotLine *line = t->getTR();

  PlotLine *ma = t->getMA(line, maType, smoothing);
  ma->setColor(color);
  ma->setType(lineType);
  ma->setLabel(label);
  output.append(ma);
  
  delete line;
  delete t;
}

QMemArray<int> ATR::getAlerts ()
{
  alerts.fill(0, data->count());

  if (! output.count())
    return alerts;

  PlotLine *atr = output.at(0);
  int atrLoop;
  int listLoop = data->count() - atr->getSize() + 18;
  int status = 0;

  for (atrLoop = 18; atrLoop < (int) atr->getSize(); atrLoop++, listLoop++)
  {
    int loop;
    double h = 0;
    double l = 99999999;
    for (loop = 0; loop < 18; loop++)
    {
      if (atr->getData(atrLoop - loop) > h)
	  h = atr->getData(atrLoop - loop);

      if (atr->getData(atrLoop - loop) < l)
        l = atr->getData(atrLoop - loop);
    }

    switch (status)
    {
      case -1:
        if (atr->getData(atrLoop) <= l)
	  status = 1;
	break;
      case 1:
        if (atr->getData(atrLoop) >= h)
	  status = -1;
	break;
      default:
        if (atr->getData(atrLoop) <= l)
	  status = 1;
	else
	{
          if (atr->getData(atrLoop) >= h)
	    status = -1;
	}
	break;
    }

    alerts[listLoop] = status;
  }

  return alerts;
}

int ATR::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("ATR Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addTextItem(tr("Label"), 1, label);
  dialog->addComboItem(tr("Line Type"), 1, lineTypes, lineType);
  dialog->addIntItem(tr("Smoothing"), 1, smoothing, 1, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), 1, maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    smoothing = dialog->getInt(tr("Smoothing"));
    maType = (QSMath::MAType) dialog->getComboIndex(tr("Smoothing Type"));
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
    maType = (QSMath::MAType) s->left(s->length()).toInt();
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

Plugin * create ()
{
  ATR *o = new ATR;
  return ((Plugin *) o);
}




