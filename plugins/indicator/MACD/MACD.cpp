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

#include "MACD.h"
#include "PrefDialog.h"
#include <qdict.h>

MACD::MACD ()
{
  pluginName = "MACD";
  plotFlag = FALSE;
  alertFlag = TRUE;
  setDefaults();
}

MACD::~MACD ()
{
}

void MACD::setDefaults ()
{
  macdColor.setNamedColor("red");
  trigColor.setNamedColor("yellow");
  oscColor.setNamedColor("blue");
  macdLineType = PlotLine::Line;
  trigLineType = PlotLine::Dash;
  oscLineType = PlotLine::Histogram;
  macdLabel = "MACD";
  trigLabel = "Trig";
  oscLabel = "Osc";
  fastPeriod = 12;
  slowPeriod = 26;
  trigPeriod = 9;
  macdMAType = QSMath::EMA;  
  macdInput = BarData::Close;
  oscScaleFlag = FALSE;
}

void MACD::calculate ()
{
  QSMath *t = new QSMath();

  PlotLine *d = data->getInput(macdInput);
  
  PlotLine *macd = t->getOSC(d, macdMAType, macdMAType, fastPeriod, slowPeriod);
  macd->setColor(macdColor);
  macd->setType(macdLineType);
  macd->setLabel(macdLabel);
  
  PlotLine *signal = t->getMA(macd, macdMAType, trigPeriod);
  signal->setColor(trigColor);
  signal->setType(trigLineType);
  signal->setLabel(trigLabel);

  PlotLine *osc = new PlotLine();
  osc->setColor(oscColor);
  osc->setType(oscLineType);
  osc->setLabel(oscLabel);
  osc->setScaleFlag(oscScaleFlag);

  int floop = macd->getSize() - 1;
  int sloop = signal->getSize() - 1;

  while (floop > -1 && sloop > -1)
  {
    osc->prepend((macd->getData(floop) - signal->getData(sloop)));
    floop--;
    sloop--;
  }

  output.append(osc);
  output.append(macd);
  output.append(signal);

  delete d;
  delete t;
}

QMemArray<int> MACD::getAlerts ()
{
  alerts.fill(0, data->count());

  if (output.count() != 3)
    return alerts;

  PlotLine *macd = output.at(1);
  PlotLine *trig = output.at(2);

  int listLoop = data->count() - trig->getSize();
  int macdLoop = macd->getSize() - trig->getSize();
  int trigLoop;
  int status = 0;
  for (trigLoop = 0; trigLoop < (int) trig->getSize(); trigLoop++, macdLoop++, listLoop++)
  {
    switch (status)
    {
      case -1:
        if (macd->getData(macdLoop) > trig->getData(trigLoop))
	  status = 1;
	break;
      case 1:
        if (macd->getData(macdLoop) < trig->getData(trigLoop))
	  status = -1;
	break;
      default:
        if (macd->getData(macdLoop) > trig->getData(trigLoop))
	  status = 1;
	else
	{
          if (macd->getData(macdLoop) < trig->getData(trigLoop))
	    status = -1;
	}
	break;
    }
    
    alerts[listLoop] = status;
  }

  return alerts;
}

int MACD::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("MACD Indicator"));
  
  dialog->createPage (tr("MACD"));
  dialog->addColorItem(tr("MACD Color"), 1, macdColor);
  dialog->addIntItem(tr("Fast Period"), 1, fastPeriod, 1, 99999999);
  dialog->addIntItem(tr("Slow Period"), 1, slowPeriod, 1, 99999999);
  dialog->addTextItem(tr("MACD Label"), 1, macdLabel);
  dialog->addComboItem(tr("MACD Line Type"), 1, lineTypes, macdLineType);
  dialog->addComboItem(tr("MACD MA Type"), 1, maTypeList, macdMAType);
  dialog->addComboItem(tr("MACD Input"), 1, inputTypeList, macdInput);
  
  dialog->createPage (tr("Trigger"));
  dialog->addColorItem(tr("Trigger Color"), 2, trigColor);
  dialog->addIntItem(tr("Trigger Period"), 2, trigPeriod, 1, 99999999);
  dialog->addTextItem(tr("Trigger Label"), 2, trigLabel);
  dialog->addComboItem(tr("Trigger Line Type"), 2, lineTypes, trigLineType);
  
  dialog->createPage (tr("Osc"));
  dialog->addColorItem(tr("Osc Color"), 3, oscColor);
  dialog->addTextItem(tr("Osc Label"), 3, oscLabel);
  dialog->addComboItem(tr("Osc Line Type"), 3, lineTypes, oscLineType);
  dialog->addCheckItem(tr("Osc Scaling Max"), 3, oscScaleFlag);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    macdColor = dialog->getColor(tr("MACD Color"));
    fastPeriod = dialog->getInt(tr("Fast Period"));
    slowPeriod = dialog->getInt(tr("Slow Period"));
    macdLabel = dialog->getText(tr("MACD Label"));
    macdLineType = (PlotLine::LineType) dialog->getComboIndex(tr("MACD Line Type"));
    macdMAType = (QSMath::MAType) dialog->getComboIndex(tr("MACD MA Type"));
    macdInput = (BarData::InputType) dialog->getComboIndex(tr("MACD Input"));
    
    trigColor = dialog->getColor(tr("Trigger Color"));
    trigPeriod = dialog->getInt(tr("Trigger Period"));
    trigLabel = dialog->getText(tr("Trigger Label"));
    trigLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Trigger Line Type"));
    
    oscColor = dialog->getColor(tr("Osc Color"));
    oscLabel = dialog->getText(tr("Osc Label"));
    oscLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Osc Line Type"));
    oscScaleFlag = dialog->getCheck(tr("Osc Scaling Max"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MACD::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["macdColor"];
  if (s)
    macdColor.setNamedColor(s->left(s->length()));
    
  s = dict["fastPeriod"];
  if (s)
    fastPeriod = s->left(s->length()).toInt();
	
  s = dict["slowPeriod"];
  if (s)
    slowPeriod = s->left(s->length()).toInt();
  
  s = dict["macdLabel"];
  if (s)
    macdLabel = s->left(s->length());
        
  s = dict["macdLineType"];
  if (s)
    macdLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["macdMAType"];
  if (s)
    macdMAType = (QSMath::MAType) s->left(s->length()).toInt();
        
  s = dict["macdInput"];
  if (s)
    macdInput = (BarData::InputType) s->left(s->length()).toInt();

  s = dict["trigColor"];
  if (s)
    trigColor.setNamedColor(s->left(s->length()));
  
  s = dict["trigPeriod"];
  if (s)
    trigPeriod = s->left(s->length()).toInt();
  
  s = dict["trigLabel"];
  if (s)
    trigLabel = s->left(s->length());
        
  s = dict["trigLineType"];
  if (s)
    trigLineType = (PlotLine::LineType) s->left(s->length()).toInt();
        
  s = dict["oscColor"];
  if (s)
    oscColor.setNamedColor(s->left(s->length()));
  
  s = dict["oscLabel"];
  if (s)
    oscLabel = s->left(s->length());
        
  s = dict["oscLineType"];
  if (s)
    oscLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["oscScaleFlag"];
  if (s)
    oscScaleFlag = s->left(s->length()).toInt();
}

void MACD::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("macdColor", new QString(macdColor.name()));
  dict.replace("fastPeriod", new QString(QString::number(fastPeriod)));
  dict.replace("slowPeriod", new QString(QString::number(slowPeriod)));
  dict.replace("macdLabel", new QString(macdLabel));
  dict.replace("macdLineType", new QString(QString::number(macdLineType)));
  dict.replace("macdMAType", new QString(QString::number(macdMAType)));
  dict.replace("macdInput", new QString(QString::number(macdInput)));
  
  dict.replace("trigColor", new QString(trigColor.name()));
  dict.replace("trigPeriod", new QString(QString::number(trigPeriod)));
  dict.replace("trigLabel", new QString(trigLabel));
  dict.replace("trigLineType", new QString(QString::number(trigLineType)));
  
  dict.replace("oscColor", new QString(oscColor.name()));
  dict.replace("oscLabel", new QString(oscLabel));
  dict.replace("oscLineType", new QString(QString::number(oscLineType)));
  dict.replace("oscScaleFlag", new QString(QString::number(oscScaleFlag)));
  
  dict.replace("plugin", new QString(pluginName));
  
  saveFile(file, dict);
}

Plugin * create ()
{
  MACD *o = new MACD;
  return ((Plugin *) o);
}

