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

#include "PER.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

PER::PER ()
{
  pluginName = "PER";
  helpFile = "per.html";
  
  setDefaults();
}

PER::~PER ()
{
}

void PER::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  input = BarData::Close;
}

void PER::calculate ()
{
  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("PER::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void PER::calculate2 (PlotLine *in)
{
  PlotLine *per = new PlotLine();
  per->setColor(color);
  per->setType(lineType);
  per->setLabel(label);

  double base = in->getData(0);
  int loop;
  for (loop = 1; loop < (int) in->getSize(); loop++)
    per->append(((in->getData(loop) - base) / base) * 100);

  output->addLine(per);
}

int PER::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("PER Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(il, pl, inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    input = (BarData::InputType) dialog->getComboIndex(il);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void PER::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
}

void PER::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("label", label);
  dict.setData("input", QString::number(input));
  dict.setData("plugin", pluginName);
}

PlotLine * PER::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 1)
    ;
  else
  {
    qDebug("PER::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("PER::calculateCustom: no input");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

int PER::getMinBars ()
{
  int t = minBars + 1;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  PER *o = new PER;
  return ((IndicatorPlugin *) o);
}






