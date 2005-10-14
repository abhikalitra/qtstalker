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

#include "MOM.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

MOM::MOM ()
{
  pluginName = "MOM";
  helpFile = "mom.html";

  methodList.append("MOM");
  methodList.append("ROC");
  
  setDefaults();
}

MOM::~MOM ()
{
}

void MOM::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  period = 10;
  smoothing = 10;  
  maType = 1;  
  input = BarData::Close;
  method = "MOM";
}

void MOM::calculate ()
{
  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("MOM::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void MOM::calculate2 (PlotLine *in)
{
  PlotLine *mom = new PlotLine();

  int loop;
  if (! method.compare("MOM"))
  {
    for (loop = period; loop < (int) in->getSize(); loop++)
      mom->append(in->getData(loop) - in->getData(loop - period));
  }
  else
  {
    for (loop = period; loop < (int) in->getSize(); loop++)
      mom->append(((in->getData(loop) - in->getData(loop - period)) / in->getData(loop - period)) * 100);
  }

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(mom, maType, smoothing, 0, 0);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete mom;
  }
  else
  {
    mom->setColor(color);
    mom->setType(lineType);
    mom->setLabel(label);
    output->addLine(mom);
  }
}

int MOM::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");
  QString il = QObject::tr("Input");
  QString ml = QObject::tr("Method");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MOM Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addComboItem(ml, pl, methodList, method);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  dialog->addComboItem(il, pl, inputTypeList, input);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    maType = dialog->getComboIndex(stl);
    smoothing = dialog->getInt(sl);
    input = (BarData::InputType) dialog->getComboIndex(il);
    method = dialog->getCombo(ml);

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MOM::setIndicatorSettings (Setting &dict)
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
    
  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();

  s = dict.getData("method");
  if (s.length())
    method = s;
}      

void MOM::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("input", QString::number(input));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("method", method);
  dict.setData("plugin", pluginName);
}

PlotLine * MOM::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, METHOD, MA_TYPE, PERIOD, SMOOTHING

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 5)
    ;
  else
  {
    qDebug("MOM::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("MOM::calculateCustom: no input");
    return 0;
  }

  if (methodList.findIndex(l[1]) == -1)
  {
    qDebug("MOM::calculateCustom: invalid METHOD parm");
    return 0;
  }
  else
    method = methodList.findIndex(l[1]);
  
  QStringList mal = getMATypes();
  if (mal.findIndex(l[2]) == -1)
  {
    qDebug("MOM::calculateCustom: invalid MA_TYPE parm");
    return 0;
  }
  else
    maType = mal.findIndex(l[2]);

  bool ok;
  int t = l[3].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("MOM::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  t = l[4].toInt(&ok);
  if (ok)
    smoothing = t;
  else
  {
    qDebug("MOM::calculateCustom: invalid SMOOTHING parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

int MOM::getMinBars ()
{
  int t = minBars + period + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MOM *o = new MOM;
  return ((IndicatorPlugin *) o);
}

