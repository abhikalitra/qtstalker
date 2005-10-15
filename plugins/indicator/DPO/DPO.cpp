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

#include "DPO.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

DPO::DPO ()
{
  pluginName = "DPO";
  helpFile = "dpo.html";
  
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
  maType = 1;
  input = BarData::Close;
}

void DPO::calculate ()
{
  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("DPO::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void DPO::calculate2 (PlotLine *in)
{
  PlotLine *ma = getMA(in, maType, period);

  PlotLine *dpo = new PlotLine();
  dpo->setColor(color);
  dpo->setType(lineType);
  dpo->setLabel(label);
  output->addLine(dpo);

  int maLoop = ma->getSize() - 1;
  int closeLoop = in->getSize() - 1;
  int t = (int) ((period / 2) + 1);

  while (maLoop >= t)
  {
    dpo->prepend(in->getData(closeLoop) - ma->getData(maLoop - t));
    closeLoop--;
    maLoop--;
  }

  delete ma;
}

int DPO::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString stl = QObject::tr("MA Type");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("DPO Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  dialog->addComboItem(il, pl, inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    maType = dialog->getComboIndex(stl);
    input = (BarData::InputType) dialog->getComboIndex(il);
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void DPO::setIndicatorSettings (Setting &dict)
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
      
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();

  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
}

void DPO::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("input", QString::number(input));
  dict.setData("plugin", pluginName);
}

PlotLine * DPO::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, MA_TYPE, PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 3)
    ;
  else
  {
    qDebug("DPO::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("DPO::calculateCustom: no input");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[1]) == -1)
  {
    qDebug("DPO::calculateCustom: invalid MA_TYPE parm");
    return 0;
  }
  else
    maType = mal.findIndex(l[1]);

  bool ok;
  int t = l[2].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("DPO::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

int DPO::getMinBars ()
{
  int t = minBars + period;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  DPO *o = new DPO;
  return ((IndicatorPlugin *) o);
}

