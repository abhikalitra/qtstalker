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

#include "MINMAX.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

MINMAX::MINMAX ()
{
  pluginName = "MINMAX";
  customFlag = TRUE;
  helpFile = "minmax.html";

  lineList.append("High");
  lineList.append("Low");
    
  setDefaults();
}

MINMAX::~MINMAX ()
{
}

void MINMAX::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  data1 = "1";
  period = 10;
  lineRequest = "High";
}

void MINMAX::calculate ()
{
  PlotLine *input = customLines->find(data1);
  if (! input)
  {
    qDebug("MINMAX::calculate: no data1 input %s", data1.latin1());
    return;
  }
    
  PlotLine *hline = new PlotLine;
  hline->setColor(color);
  hline->setType(lineType);
  hline->setLabel(label);
  
  PlotLine *lline = new PlotLine;
  lline->setColor(color);
  lline->setType(lineType);
  lline->setLabel(label);

  int loop;
  for (loop = period; loop < (int) input->getSize(); loop++)
  {
    int loop2;
    double h = -99999999;
    double l = 99999999;
    for (loop2 = 0; loop2 <= period; loop2++)
    {
      double t = input->getData(loop - loop2);
      if (t > h)
        h = t;
      if (t < l)
        l = t;
    }

    lline->append(l);
    hline->append(h);
  }
  
  output->addLine(hline);
  output->addLine(lline);
}

int MINMAX::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString d1l = QObject::tr("Data1");
  QString perl = QObject::tr("Period");
  
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MINMAX Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addFormulaInputItem(d1l, pl, FALSE, data1);
  dialog->addIntItem(perl, pl, period, 1, 99999999);

  if (customFlag)
  {
    QString t = QObject::tr("Plot");
    dialog->addComboItem(t, pl, lineList, lineRequest);
  }
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    label = dialog->getText(ll);
    data1 = dialog->getFormulaInput(d1l);    
    period = dialog->getInt(perl);

    if (customFlag)
    {
      QString t = QObject::tr("Plot");
      lineRequest = dialog->getCombo(t);    
    }
      
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MINMAX::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("label");
  if (s.length())
    label = s;
        
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();
    
  s = dict.getData("data1");
  if (s.length())
    data1 = s;

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("lineRequest");
  if (s.length())
    lineRequest = s;
}

void MINMAX::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("label", label);
  dict.setData("lineType", QString::number(lineType));
  dict.setData("plugin", pluginName);
  dict.setData("data1", data1);
  dict.setData("period", QString::number(period));
  dict.setData("lineRequest", lineRequest);
}

PlotLine * MINMAX::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();

  if (! lineRequest.compare("High"))
    return output->getLine(0);
  else
    return output->getLine(1);
}

int MINMAX::getMinBars ()
{
  int t = minBars + period;
  return t;
}

//******************************************************************
//******************************************************************
//******************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MINMAX *o = new MINMAX;
  return ((IndicatorPlugin *) o);
}



