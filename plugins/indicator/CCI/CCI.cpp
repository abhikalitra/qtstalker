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

#include "CCI.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>

CCI::CCI ()
{
  pluginName = "CCI";
  plotFlag = FALSE;
  alertFlag = TRUE;
  setDefaults();

  alertList.append(tr("100 Rule"));
  alertList.append(tr("0 Rule"));
  alertList.sort();
}

CCI::~CCI ()
{
}

void CCI::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  smoothing = 3;
  period = 20;
  maType = QSMath::SMA;
  alertType = tr("100 Rule");
}

void CCI::calculate ()
{
  QSMath *t = new QSMath(data);
  
  PlotLine *cci = t->getCCI(period);

  if (smoothing > 1)
  {
    PlotLine *ma = t->getMA(cci, maType, smoothing);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output.append(ma);
    delete cci;
  }
  else
  {
    cci->setColor(color);
    cci->setType(lineType);
    cci->setLabel(label);
    output.append(cci);
  }

  delete t;
}

QMemArray<int> CCI::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() == 0)
    return alerts;

  if (! alertType.compare(tr("100 Rule")))
    alertHundred();

  if (! alertType.compare(tr("0 Rule")))
    alertZero();

  return alerts;
}

void CCI::alertHundred ()
{
  PlotLine *cci = output.at(0);

  int dataLoop = data->count() - cci->getSize();
  int loop;
  int status = 0;
  for (loop = 0; loop < (int) cci->getSize(); loop++, dataLoop++)
  {
    switch (status)
    {
      case -1:
        if (cci->getData(loop) > -100)
          status = 0;
        break;
      case 1:
        if (cci->getData(loop) < 100)
	  status = 0;
	break;
      default:
        if (cci->getData(loop) > 100)
	  status = 1;
	else
	{
          if (cci->getData(loop) < -100)
	    status = -1;
	}
	break;
    }

    alerts[dataLoop] = status;
  }
}

void CCI::alertZero ()
{
  PlotLine *cci = output.at(0);

  int dataLoop = data->count() - cci->getSize();
  int status = 0;
  int loop;
  for (loop = 0; loop < (int) cci->getSize(); loop++, dataLoop++)
  {
    switch (status)
    {
      case -1:
        if (cci->getData(loop) > 0)
          status = 0;
        break;
      case 1:
        if (cci->getData(loop) < 0)
	  status = 0;
	break;
      default:
        if (cci->getData(loop) > 0)
	  status = 1;
	else
	{
          if (cci->getData(loop) < 0)
	    status = -1;
	}
	break;
    }

    alerts[dataLoop] = status;
  }
}

int CCI::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("CCI Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addIntItem(tr("Period"), tr("Parms"), period, 1, 99999999);
  dialog->addIntItem(tr("Smoothing"), tr("Parms"), smoothing, 0, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("Parms"), maTypeList, maType);
  dialog->addComboItem(tr("Alert"), tr("Parms"), alertList, alertType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    period = dialog->getInt(tr("Period"));
    label = dialog->getText(tr("Label"));
    smoothing = dialog->getInt(tr("Smoothing"));
    maType = (QSMath::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    alertType = dialog->getCombo(tr("Alert"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void CCI::loadIndicatorSettings (QString file)
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

  s = dict["smoothing"];
  if (s)
    smoothing = s->left(s->length()).toInt();

  s = dict["label"];
  if (s)
    label = s->left(s->length());
      
  s = dict["maType"];
  if (s)
    maType = (QSMath::MAType) s->left(s->length()).toInt();

  s = dict["alertType"];
  if (s)
    alertType = s->left(s->length());
}

void CCI::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("smoothing", new QString(QString::number(smoothing)));
  dict.replace("label", new QString(label));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("alertType", new QString(alertType));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  CCI *o = new CCI;
  return ((Plugin *) o);
}





