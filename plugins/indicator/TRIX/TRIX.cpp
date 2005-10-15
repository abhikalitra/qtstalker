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

#include "TRIX.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

TRIX::TRIX ()
{
  pluginName = "TRIX";
  helpFile = "trix.html";
  
  setDefaults();
}

TRIX::~TRIX ()
{
}

void TRIX::setDefaults ()
{
  color.setNamedColor("red");
  trigColor.setNamedColor("yellow");
  lineType = PlotLine::Line;
  trigLineType = PlotLine::Dash;
  label = pluginName;
  trigLabel = QObject::tr("TRIX Trig");
  period = 12;
  tperiod = 9;
  input = BarData::Close;
  maType = 1;  
}

void TRIX::calculate ()
{
  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("TRIX::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void TRIX::calculate2 (PlotLine *in)
{
  PlotLine *ema = getMA(in, 0, period);
  
  PlotLine *ema2 = getMA(ema, 0, period);
  
  PlotLine *ema3 = getMA(ema2, 0, period);
  int emaLoop = ema3->getSize() - 1;

  PlotLine *trix = new PlotLine();

  while (emaLoop > 0)
  {
    trix->prepend(((ema3->getData(emaLoop) - ema3->getData(emaLoop - 1)) / ema3->getData(emaLoop - 1)) * 100);
    emaLoop--;
  }

  PlotLine *trigger = getMA(trix, maType, tperiod);
  trigger->setColor(trigColor);
  trigger->setType(trigLineType);
  trigger->setLabel(trigLabel);

  trix->setColor(color);
  trix->setType(lineType);
  trix->setLabel(label);
  output->addLine(trix);

  output->addLine(trigger);

  delete ema;
  delete ema2;
  delete ema3;
}

int TRIX::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("TRIX Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  dialog->addComboItem(il, pl, inputTypeList, input);
  
  pl = QObject::tr("Trigger Parms");
  dialog->createPage (pl);
  QString t = QObject::tr("Trigger Color");
  dialog->addColorItem(t, pl, trigColor);
  t = QObject::tr("Trigger Line Type");
  dialog->addComboItem(t, pl, lineTypes, trigLineType);
  t = QObject::tr("Trigger Label");
  dialog->addTextItem(t, pl, trigLabel);
  t = QObject::tr("Trigger Period");
  dialog->addIntItem(t, pl, tperiod, 1, 99999999);
  QStringList l = getMATypes();
  t = QObject::tr("Trigger Type");
  dialog->addComboItem(t, pl, l, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    input = (BarData::InputType) dialog->getComboIndex(il);
    
    t = QObject::tr("Trigger Color");
    trigColor = dialog->getColor(t);
    t = QObject::tr("Trigger Line Type");
    trigLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("Trigger Period");
    tperiod = dialog->getInt(t);
    t = QObject::tr("Trigger Label");
    trigLabel = dialog->getText(t);
    t = QObject::tr("Trigger Type");
    maType = dialog->getComboIndex(t);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void TRIX::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
  
  s = dict.getData("trigColor");
  if (s.length())
    trigColor.setNamedColor(s);
    
  s = dict.getData("trigLineType");
  if (s.length())
    trigLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("tperiod");
  if (s.length())
    tperiod = s.toInt();

  s = dict.getData("trigLabel");
  if (s.length())
    trigLabel = s;
  
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();
}

void TRIX::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("input", QString::number(input));

  dict.setData("trigColor", trigColor.name());
  dict.setData("trigLineType", QString::number(trigLineType));
  dict.setData("tperiod", QString::number(tperiod));
  dict.setData("trigLabel", trigLabel);
  dict.setData("maType", QString::number(maType));
  dict.setData("plugin", pluginName);
}

PlotLine * TRIX::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 2)
    ;
  else
  {
    qDebug("TRIX::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("TRIX::calculateCustom: no input");
    return 0;
  }

  bool ok;
  int t = l[1].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("TRIX::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

int TRIX::getMinBars ()
{
  int t = minBars + period + period + period + tperiod;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  TRIX *o = new TRIX;
  return ((IndicatorPlugin *) o);
}


