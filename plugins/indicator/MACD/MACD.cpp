/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include <qobject.h>

MACD::MACD ()
{
  pluginName = "MACD";
  helpFile = "macd.html";
  
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
  label = macdLabel;
  trigLabel = "Trig";
  oscLabel = "Osc";
  fastPeriod = 12;
  slowPeriod = 26;
  trigPeriod = 9;
  macdMAType = 0;  
  macdInput = BarData::Close;
  oscScaleFlag = FALSE;
}

void MACD::calculate ()
{
  PlotLine *in = data->getInput(macdInput);
  if (! in)
  {
    qDebug("MACD::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void MACD::calculate2 (PlotLine *d)
{
  PlotLine *fma = getMA(d, macdMAType, fastPeriod, 0, 0);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(d, macdMAType, slowPeriod, 0, 0);
  int smaLoop = sma->getSize() - 1;
  
  PlotLine *macd = new PlotLine();
  macd->setColor(macdColor);
  macd->setType(macdLineType);
  macd->setLabel(macdLabel);
  
  while (fmaLoop > -1 && smaLoop > -1)
  {
    macd->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }
  delete fma;
  delete sma;

  PlotLine *signal = getMA(macd, macdMAType, trigPeriod, 0, 0);
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

  output->addLine(osc);
  output->addLine(macd);
  output->addLine(signal);
}

int MACD::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MACD Indicator"));
  dialog->setHelpFile(helpFile);
  
  QString pl = "MACD";
  dialog->createPage (pl);
  QString t = QObject::tr("MACD Color");
  dialog->addColorItem(t, pl, macdColor);
  t = QObject::tr("Fast Period");
  dialog->addIntItem(t, pl, fastPeriod, 1, 99999999);
  t = QObject::tr("Slow Period");
  dialog->addIntItem(t, pl, slowPeriod, 1, 99999999);
  t = QObject::tr("MACD Label");
  dialog->addTextItem(t, pl, macdLabel);
  t = QObject::tr("MACD Line Type");
  dialog->addComboItem(t, pl, lineTypes, macdLineType);
  QStringList l = getMATypes();
  t = QObject::tr("MACD MA Type");
  dialog->addComboItem(t, pl, l, macdMAType);
  t = QObject::tr("MACD Input");
  dialog->addComboItem(t, pl, inputTypeList, macdInput);
  
  pl = QObject::tr("Trigger");
  dialog->createPage (pl);
  t = QObject::tr("Trigger Color");
  dialog->addColorItem(t, pl, trigColor);
  t = QObject::tr("Trigger Period");
  dialog->addIntItem(t, pl, trigPeriod, 1, 99999999);
  t = QObject::tr("Trigger Label");
  dialog->addTextItem(t, pl, trigLabel);
  t = QObject::tr("Trigger Line Type");
  dialog->addComboItem(t, pl, lineTypes, trigLineType);
  
  pl = QObject::tr("Osc");
  dialog->createPage (pl);
  t = QObject::tr("Osc Color");
  dialog->addColorItem(t, pl, oscColor);
  t = QObject::tr("Osc Label");
  dialog->addTextItem(t, pl, oscLabel);
  t = QObject::tr("Osc Line Type");
  dialog->addComboItem(t, pl, lineTypes, oscLineType);
  t = QObject::tr("Osc Scaling Max");
  dialog->addCheckItem(t, pl, oscScaleFlag);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    t = QObject::tr("MACD Color");
    macdColor = dialog->getColor(t);
    t = QObject::tr("Fast Period");
    fastPeriod = dialog->getInt(t);
    t = QObject::tr("Slow Period");
    slowPeriod = dialog->getInt(t);
    t = QObject::tr("MACD Label");
    macdLabel = dialog->getText(t);
    label = macdLabel;
    t = QObject::tr("MACD Line Type");
    macdLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("MACD MA Type");
    macdMAType = dialog->getComboIndex(t);
    t = QObject::tr("MACD Input");
    macdInput = (BarData::InputType) dialog->getComboIndex(t);
    
    t = QObject::tr("Trigger Color");
    trigColor = dialog->getColor(t);
    t = QObject::tr("Trigger Period");
    trigPeriod = dialog->getInt(t);
    t = QObject::tr("Trigger Label");
    trigLabel = dialog->getText(t);
    t = QObject::tr("Trigger Line Type");
    trigLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    
    t = QObject::tr("Osc Color");
    oscColor = dialog->getColor(t);
    t = QObject::tr("Osc Label");
    oscLabel = dialog->getText(t);
    t = QObject::tr("Osc Line Type");
    oscLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("Osc Scaling Max");
    oscScaleFlag = dialog->getCheck(t);
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MACD::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("macdColor");
  if (s.length())
    macdColor.setNamedColor(s);
    
  s = dict.getData("fastPeriod");
  if (s.length())
    fastPeriod = s.toInt();
	
  s = dict.getData("slowPeriod");
  if (s.length())
    slowPeriod = s.toInt();
  
  s = dict.getData("macdLabel");
  if (s.length())
    macdLabel = s;
        
  s = dict.getData("macdLineType");
  if (s.length())
    macdLineType = (PlotLine::LineType) s.toInt();
        
  s = dict.getData("macdMAType");
  if (s.length())
    macdMAType = s.toInt();
        
  s = dict.getData("macdInput");
  if (s.length())
    macdInput = (BarData::InputType) s.toInt();

  s = dict.getData("trigColor");
  if (s.length())
    trigColor.setNamedColor(s);
  
  s = dict.getData("trigPeriod");
  if (s.length())
    trigPeriod = s.toInt();
  
  s = dict.getData("trigLabel");
  if (s.length())
    trigLabel = s;
        
  s = dict.getData("trigLineType");
  if (s.length())
    trigLineType = (PlotLine::LineType) s.toInt();
        
  s = dict.getData("oscColor");
  if (s.length())
    oscColor.setNamedColor(s);
  
  s = dict.getData("oscLabel");
  if (s.length())
    oscLabel = s;
        
  s = dict.getData("oscLineType");
  if (s.length())
    oscLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("oscScaleFlag");
  if (s.length())
    oscScaleFlag = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
}

void MACD::getIndicatorSettings (Setting &dict)
{
  dict.setData("macdColor", macdColor.name());
  dict.setData("fastPeriod", QString::number(fastPeriod));
  dict.setData("slowPeriod", QString::number(slowPeriod));
  dict.setData("macdLabel", macdLabel);
  dict.setData("macdLineType", QString::number(macdLineType));
  dict.setData("macdMAType", QString::number(macdMAType));
  dict.setData("macdInput", QString::number(macdInput));
  
  dict.setData("trigColor", trigColor.name());
  dict.setData("trigPeriod", QString::number(trigPeriod));
  dict.setData("trigLabel", trigLabel);
  dict.setData("trigLineType", QString::number(trigLineType));
  
  dict.setData("oscColor", oscColor.name());
  dict.setData("oscLabel", oscLabel);
  dict.setData("oscLineType", QString::number(oscLineType));
  dict.setData("oscScaleFlag", QString::number(oscScaleFlag));
  
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
}

PlotLine * MACD::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, MA_TYPE, FAST_PERIOD, SLOW_PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 4)
    ;
  else
  {
    qDebug("MACD::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("MACD::calculateCustom: no input");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[1]) == -1)
  {
    qDebug("MACD::calculateCustom: invalid MA_TYPE parm");
    return 0;
  }
  else
    macdMAType = mal.findIndex(l[1]);

  bool ok;
  int t = l[2].toInt(&ok);
  if (ok)
    fastPeriod = t;
  else
  {
    qDebug("MACD::calculateCustom: invalid FAST_PERIOD parm");
    return 0;
  }

  t = l[3].toInt(&ok);
  if (ok)
    slowPeriod = t;
  else
  {
    qDebug("MACD::calculateCustom: invalid SLOW_PERIOD parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(1);
}

int MACD::getMinBars ()
{
  int t = 0;
  if (fastPeriod > t)
    t = fastPeriod;
  if (slowPeriod > t)
    t = slowPeriod;
  t++;
  t = t + trigPeriod;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MACD *o = new MACD;
  return ((IndicatorPlugin *) o);
}

