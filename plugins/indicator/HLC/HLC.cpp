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

#include "HLC.h"
#include "PrefDialog.h"
#include <qdict.h>

HLC::HLC ()
{
  pluginName = "HLC";
  plotFlag = TRUE;
  alertFlag = TRUE;
  setDefaults();
}

HLC::~HLC ()
{
}

void HLC::setDefaults ()
{
  upperColor.setNamedColor("red");
  lowerColor.setNamedColor("red");
  upperLineType = PlotLine::Line;
  lowerLineType = PlotLine::Line;
  upperLabel = tr("HLCU");
  lowerLabel = tr("HLCL");
  period = 20;
}

void HLC::calculate ()
{
  PlotLine *ub = new PlotLine();
  PlotLine *lb = new PlotLine();

  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    int loop2;
    double h = -99999999;
    double l = 99999999;
    for (loop2 = 1; loop2 <= period; loop2++)
    {
      double high = data->getHigh(loop - loop2);
      double low = data->getLow(loop - loop2);

      if (high > h)
        h = high;

      if (low < l)
        l = low;
    }

    ub->append(h);
    lb->append(l);
  }

  ub->setColor(upperColor);
  ub->setType(upperLineType);
  ub->setLabel(upperLabel);
  output.append(ub);

  lb->setColor(lowerColor);
  lb->setType(lowerLineType);
  lb->setLabel(lowerLabel);
  output.append(lb);
}

QMemArray<int> HLC::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 2)
    return alerts;

  PlotLine *u = output.at(0);
  PlotLine *l = output.at(1);

  int listLoop = data->count() - u->getSize();

  int loop;
  int status = 0;
  for (loop = 0; loop < (int) u->getSize(); loop++, listLoop++)
  {
    double close = data->getClose(listLoop);

    switch (status)
    {
      case -1:
        if (close > u->getData(loop))
	  status = 1;
	break;
      case 1:
        if (close < l->getData(loop))
	  status = -1;
	break;
      default:
        if (close > u->getData(loop))
	  status = 1;
	else
	{
          if (close < l->getData(loop))
	    status = -1;
	}
	break;
    }
    
    alerts[listLoop] = status;
  }

  return alerts;
}

int HLC::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("HLC Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  
  dialog->createPage (tr("Upper"));
  dialog->addColorItem(tr("Upper Color"), tr("Upper"), upperColor);
  dialog->addComboItem(tr("Upper Line Type"), tr("Upper"), lineTypes, upperLineType);
  dialog->addTextItem(tr("Upper Label"), tr("Upper"), upperLabel);
  
  dialog->createPage (tr("Lower"));
  dialog->addColorItem(tr("Lower Color"), tr("Lower"), lowerColor);
  dialog->addComboItem(tr("Lower Line Type"), tr("Lower"), lineTypes, lowerLineType);
  dialog->addTextItem(tr("Lower Label"), tr("Lower"), lowerLabel);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(tr("Period"));
    upperColor = dialog->getColor(tr("Upper Color"));
    upperLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Upper Line Type"));
    upperLabel = dialog->getText(tr("Upper Label"));
    lowerColor = dialog->getColor(tr("Lower Color"));
    lowerLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Lower Line Type"));
    lowerLabel = dialog->getText(tr("Lower Label"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void HLC::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["period"];
  if (s)
    period = s->left(s->length()).toInt();
  
  s = dict["upperColor"];
  if (s)
    upperColor.setNamedColor(s->left(s->length()));
    
  s = dict["upperLineType"];
  if (s)
    upperLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["upperLabel"];
  if (s)
    upperLabel = s->left(s->length());
      
  s = dict["lowerColor"];
  if (s)
    lowerColor.setNamedColor(s->left(s->length()));
    
  s = dict["lowerLineType"];
  if (s)
    lowerLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["lowerLabel"];
  if (s)
    lowerLabel = s->left(s->length());
}

void HLC::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("period", new QString(QString::number(period)));
  dict.replace("upperColor", new QString(upperColor.name()));
  dict.replace("upperLineType", new QString(QString::number(upperLineType)));
  dict.replace("upperLabel", new QString(upperLabel));
  dict.replace("lowerColor", new QString(lowerColor.name()));
  dict.replace("lowerLineType", new QString(QString::number(lowerLineType)));
  dict.replace("lowerLabel", new QString(lowerLabel));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  HLC *o = new HLC;
  return ((Plugin *) o);
}


