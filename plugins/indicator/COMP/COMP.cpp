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

#include "COMP.h"
#include "PrefDialog.h"
#include <qdict.h>

COMP::COMP ()
{
  pluginName = "COMP";
  plotFlag = FALSE;
  customFlag = TRUE;
  
  opList.append("==");
  opList.append("<");
  opList.append("<=");
  opList.append(">");
  opList.append(">=");
  
  setDefaults();
}

COMP::~COMP ()
{
}

void COMP::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
  method = "==";
  data1 = "1";
  data2 = "2";
}

void COMP::calculate ()
{
  PlotLine *input = customLines->find(data1);
  if (! input)
  {
    qDebug("COMP::calculate: no data1 input %s", data1.latin1());
    return;
  }
  int loop = input->getSize() - 1;
    
  int loop2 = 0;
  double inputNum = 0;
  PlotLine *input2 = 0;
  if (data2.contains("#"))
  {
    QString s = data2;
    s.remove(0, 1);
    inputNum = s.toDouble();
  }
  else
  {
    input2 = customLines->find(data2);
    if (! input2)
    {
      qDebug("COMP::calculate: no data2 input");
      return;
    }
    
    loop2 = input2->getSize() - 1;
  }
    
  PlotLine *line = new PlotLine;
  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  
  Operator op = getOperator(method);
  
  while (loop > -1)
  {
    if (input2)
    {
      if (loop2 < 0)
        break;
    }
    
    double t = 0;
    
    if (! input2)
      t = inputNum;
    else
      t = input2->getData(loop2);
      
    switch (op)
    {
      case Equal:
        if (input->getData(loop) == t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case LessThan:
        if (input->getData(loop) < t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case LessThanEqual:
        if (input->getData(loop) <= t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case GreaterThan:
        if (input->getData(loop) > t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case GreaterThanEqual:
        if (input->getData(loop) >= t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }
      
    loop--;
    if (input2)
      loop2--;
  }
  
  output.append(line);
}

int COMP::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("COMP Indicator"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Color"), tr("Parms"), color);
  dialog->addTextItem(tr("Label"), tr("Parms"), label);
  dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, lineType);
  dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, data1);
  dialog->addComboItem(tr("Method"), tr("Parms"), opList, method);
  dialog->addFormulaInputItem(tr("Data2"), tr("Parms"), TRUE, data2);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(tr("Line Type"));
    label = dialog->getText(tr("Label"));
    method = dialog->getCombo(tr("Method"));
    data1 = dialog->getFormulaInput(tr("Data1"));
    data2 = dialog->getFormulaInput(tr("Data2"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void COMP::loadIndicatorSettings (QString file)
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
    
  s = dict["method"];
  if (s)
    method = s->left(s->length());

  s = dict["data1"];
  if (s)
    data1 = s->left(s->length());

  s = dict["data2"];
  if (s)
    data2 = s->left(s->length());
}

void COMP::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("color", new QString(color.name()));
  dict.replace("label", new QString(label));
  dict.replace("lineType", new QString(QString::number(lineType)));
  dict.replace("plugin", new QString(pluginName));
  dict.replace("method", new QString(method));
  dict.replace("data1", new QString(data1));
  dict.replace("data2", new QString(data2));

  saveFile(file, dict);
}

PlotLine * COMP::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output.at(0);
}

QString COMP::getCustomSettings ()
{
  QString s("COMP");
  s.append("," + method);
  s.append("," + data1);
  s.append("," + data2);
  s.append("," + color.name());
  s.append("," + QString::number(lineType));
  s.append("," + label);
  return s;
}

void COMP::setCustomSettings (QString d)
{
  customFlag = TRUE;

  QStringList l = QStringList::split(",", d, FALSE);
  method = l[1];
  data1 = l[2];
  data2 = l[3];
  color.setNamedColor(l[4]);
  lineType = (PlotLine::LineType) l[5].toInt();
  label = l[6];
}

COMP::Operator COMP::getOperator (QString d)
{
  Operator op = NoOp;
  
  while (1)
  {
    if (! d.compare("=="))
    {
      op = Equal;
      break;
    }
    
    if (! d.compare("<"))
    {
      op = LessThan;
      break;
    }
  
    if (! d.compare("<="))
    {
      op = LessThanEqual;
      break;
    }
      
    if (! d.compare(">"))
    {
      op = GreaterThan;
      break;
    }
    
    if (! d.compare(">="))
      op = GreaterThanEqual;
    
    break;
  }
  
  return op;
}

//***************************************************************
//***************************************************************
//***************************************************************

Plugin * create ()
{
  COMP *o = new COMP;
  return ((Plugin *) o);
}



