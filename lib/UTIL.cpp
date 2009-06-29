/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "UTIL.h"
#include "PrefDialog.h"
#include <QInputDialog>
#include <QObject>
#include <QtDebug>
#include <math.h>



UTIL::UTIL ()
{
  methodList.append("ACCUM");
  methodList.append("NORMAL");
  methodList.append("COMP");
  methodList.append("REF");
  methodList.append("PER");
  methodList.append("COLOR");
  methodList.append("INRANGE");
  methodList.sort();

  opList.append("EQ");
  opList.append("LT");
  opList.append("LTEQ");
  opList.append("GT");
  opList.append("GTEQ");
  opList.append("AND");
  opList.append("OR");
  opList.append("XOR");
}

UTIL::~UTIL ()
{
}

void UTIL::calculate (BarData *bd, IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  data = bd;
  QString s, s2;

  s = "Method";
  parms.getData(s, s2);

  if (! s2.compare("ACCUM"))
  {
    calculateAccum(parms, tlines);
    return;
  }

  if (! s2.compare("Normal"))
  {
    calculateNormal(parms, tlines);
    return;
  }

  if (! s2.compare("COMP"))
  {
    calculateCOMP(parms, tlines);
    return;
  }

  if (! s2.compare("REF"))
  {
    calculateREF(parms, tlines);
    return;
  }

  if (! s2.compare("PER"))
  {
    calculatePER(parms, tlines);
    return;
  }

  if (! s2.compare("COLOR"))
  {
    calculateCOLOR(parms, tlines);
    return;
  }

  if (! s2.compare("INRANGE"))
  {
    calculateINRANGE(parms, tlines);
  }
}

void UTIL::calculateAccum (IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  PlotLine *line = new PlotLine;
  QString s, s2;
  parms.getVariable(s);
  tlines.insert(s, line);

  s = "Input";
  parms.getData(s, s2);
  PlotLine *input = tlines.value(s2);
  if (! input)
    return;
  
  int loop;
  double accum = 0;
  for (loop = 0; loop < (int) input->getSize(); loop++)
  {
    accum = accum + input->getData(loop);
    line->append(accum);
  }
}

void UTIL::calculateNormal(IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  PlotLine *normal = new PlotLine;
  QString s, s2;
  parms.getVariable(s);
  tlines.insert(s, normal);

  s = "Input";
  parms.getData(s, s2);
  PlotLine *input = tlines.value(s2);
  if (! input)
    return;

  int loop = 0;
  double range = 0;
  double max = -99999999.0;
  double min = 99999999.0;
  double norm = 0;
  for (loop = 0; loop < input->getSize(); loop++)
  {
    if (input->getData(loop) > max)
      max = input->getData(loop);

    if (input->getData(loop) < min)
      min = input->getData(loop);
  }
	
  range = fabs(max) + fabs(min);
	
  for (loop = 0; loop < input->getSize(); loop++)
  {	
    norm = ((input->getData(loop) - min) / range) * 100;
    input->append(norm);
  }
}

void UTIL::calculateCOMP (IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, OPERATOR
  PlotLine *line = new PlotLine;
  QString s, s2;
  parms.getVariable(s);
  tlines.insert(s, line);

  s = "Input";
  parms.getData(s, s2);
  PlotLine *input = tlines.value(s2);
  if (! input)
    return;

  s = "Input2";
  parms.getData(s, s2);
  PlotLine *input2 = tlines.value(s2);
  if (! input2)
    return;

  int loop = input->getSize() - 1;
  int loop2 = input2->getSize() - 1;

  s = "Operator";
  parms.getData(s, s2);
  Operator op = getOperator(s2);
  
  while (loop > -1 && loop2 > -1)
  {
    double t = input2->getData(loop2);
      
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
      case And:
        if (input->getData(loop) && t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      case Or:
        if (input->getData(loop) || t)
          line->prepend(1);
	else
          line->prepend(0);
        break;
      default:
        break;
    }
      
    loop--;
    loop2--;
  }
}

void UTIL::calculateREF (IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  QString s, s2;
  PlotLine *line = new PlotLine;
  parms.getVariable(s);
  tlines.insert(s, line);

  s = "Period";
  parms.getData(s, s2);
  int period = s2.toInt();

  s = "Input";
  parms.getData(s, s2);
  PlotLine *in = tlines.value(s2);
  if (! in)
  {
    in = data->getInput(data->getInputType(s2));
    if (in)
      tlines.insert(s2, in);
    else
      return;
  }
  
  int loop = 0;
  for (loop = 0; loop < in->getSize(); loop++)
  {
    if (loop - period < 0)
      continue;
      
    line->append(in->getData(loop - period));
  }
}

void UTIL::calculatePER (IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  // format: METHOD, INPUT_ARRAY

  QString s, s2;
  PlotLine *line = new PlotLine();
  parms.getVariable(s);
  tlines.insert(s, line);

  s = "Input";
  parms.getData(s, s2);
  PlotLine *input = tlines.value(s2);
  if (! input)
    return;
  
  double base = input->getData(0);
  int loop;
  for (loop = 1; loop < (int) input->getSize(); loop++)
    line->append(((input->getData(loop) - base) / base) * 100);
}

void UTIL::calculateCOLOR (IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  // format1: METHOD, INPUT_ARRAY, COLOR_ARRAY, VALUE, COLOR
  QString s, s2;
  s = "Value";
  parms.getData(s, s2);
  int value = s2.toInt();

  s = "Color";
  parms.getData(s, s2);
  QColor c(s);

  s = "Input";
  parms.getData(s, s2);
  PlotLine *inbool = tlines.value(s2);
  if (! inbool)
    return;
  int inboolLoop = inbool->getSize() - 1;

  s = "Color Array";
  parms.getData(s, s2);
  PlotLine *incol = tlines.value(s2);
  if (! incol)
    return;
  incol->setColorFlag(TRUE);
  int incolLoop = incol->getSize() - 1;

  while (inboolLoop > -1 && incolLoop > -1)
  {
    if (inbool->getData(inboolLoop) == value)
      incol->setColorBar(incolLoop, c);

    inboolLoop--;
    incolLoop--;
  }
}

void UTIL::calculateINRANGE (IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  // format: METHOD, INPUT_ARRAY, INPUT_ARRAY2, INPUT_ARRAY3

  PlotLine *line = new PlotLine();
  QString s, s2;
  parms.getVariable(s);
  tlines.insert(s, line);

  s = "Input";
  parms.getData(s, s2);
  PlotLine *input = tlines.value(s2);
  if (! input)
    return;
  int loop = input->getSize() - 1;

  s = "Min Array";
  parms.getData(s, s2);
  PlotLine *input2 = tlines.value(s2);
  if (! input2)
    return;
  int loop2 = input2->getSize() - 1;

  s = "Max Array";
  parms.getData(s, s2);
  PlotLine *input3 = tlines.value(s2);
  if (! input3)
    return;
  int loop3 = input3->getSize() - 1;

  double min = 0;
  double max = 0;
  while (loop > -1 && loop2 > -1 && loop3 > -1)
  {
    min = input2->getData(loop2);
    loop2--;

    max = input3->getData(loop3);
    loop3--;

    if (input->getData(loop) >= min && input->getData(loop) <= max)
      line->prepend(1);
    else
      line->prepend(0);

    loop--;
  }
}

void UTIL::prefDialog (IndicatorParms &parms, QStringList &vl)
{
  QString s("Method");
  QString method;
  parms.getData(s, method);
  if (! method.length())
  {
    bool ok;
    method = QInputDialog::getItem(0,
				   QObject::tr("UTIL Indicator Selection"),
                                   QObject::tr("Select an indicator:"),
                                   methodList,
                                   0,
                                   TRUE,
                                   &ok);
    if (! ok)
      return;
  
    parms.setData(s, method);
  }

  QString vnl = QObject::tr("Variable Name");
  QString ai1l = QObject::tr("Input");
  QString ai2l = QObject::tr("Input2");
  QString cal = QObject::tr("Color Array");
  QString ol = QObject::tr("Operator");
  QString perl = QObject::tr("Period");
  QString cl = QObject::tr("Color");
  QString minl = QObject::tr("Minimum");
  QString maxl = QObject::tr("Maximum");
  QString cil = QObject::tr("Value");

  PrefDialog *dialog = new PrefDialog(0);
  dialog->setWindowTitle(QObject::tr("UTIL Format"));

  QString s2;
  parms.getVariable(s);
  dialog->addTextItem(vnl, s);

  if (method == "ACCUM" || method == "Normal" || method == "PER")
  {
    // format: METHOD, INPUT_ARRAY
    parms.getData(ai1l, s);
    dialog->addComboItem(ai1l, vl, s);
  }

  if (method == "COMP")
  {
    // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, OPERATOR
    parms.getData(ai1l, s);
    dialog->addComboItem(ai1l, vl, s);

    parms.getData(ai2l, s);
    dialog->addComboItem(ai2l, vl, s);

    parms.getData(ol, s);
    dialog->addComboItem(ol, opList, s);
  }

  if (method == "REF")
  {
    // format1: METHOD, ARRAY_INPUT, PERIOD
    parms.getData(ai1l, s);
    dialog->addComboItem(ai1l, vl, s);

    parms.getData(perl, s);
    dialog->addIntItem(perl, s.toInt(), 0, 999999);
  }

  if (method == "COLOR")
  {
    // format1: METHOD, INPUT_ARRAY, COLOR_ARRAY, VALUE, COLOR
    parms.getData(ai1l, s);
    dialog->addComboItem(ai1l, vl, s);

    parms.getData(cal, s);
    dialog->addComboItem(cal, vl, s);

    parms.getData(cil, s);
    dialog->addDoubleItem(cil, s.toDouble(), -99999999.0, 99999999.0);

    parms.getData(cl, s);
    QColor color(s);
    dialog->addColorItem(cl, color);
  }

  if (method == "INRANGE")
  {
    // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, ARRAY_INPUT3
    parms.getData(ai1l, s);
    dialog->addComboItem(ai1l, vl, s);

    QString s = QObject::tr("Min Array");
    parms.getData(s, s2);
    dialog->addComboItem(s, vl, s2);

    s = QObject::tr("Max Array");
    parms.getData(s, s2);
    dialog->addComboItem(s, vl, s2);
  }

  int rc = dialog->exec();
 
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  s = "Method";
  parms.setData(s, method);

  dialog->getText(vnl, s);
  parms.setVariable(s);

  if (method == "ACCUM" || method == "Normal" || method == "PER")
  {
    // format: METHOD, INPUT_ARRAY
    dialog->getCombo(ai1l, s);
    parms.setData(ai1l, s);
  }

  if (method == "COMP")
  {
    // format2: METHOD, ARRAY_INPUT, ARRAY_INPUT2, OPERATOR
    dialog->getCombo(ai1l, s);
    parms.setData(ai1l, s);

    dialog->getCombo(ai2l, s);
    parms.setData(ai2l, s);

    dialog->getCombo(ol, s);
    parms.setData(ol, s);
  }

  if (method == "REF")
  {
    // format1: METHOD, ARRAY_INPUT, PERIOD
    dialog->getCombo(ai1l, s);
    parms.setData(ai1l, s);

    s = QString::number(dialog->getInt(perl));
    parms.setData(perl, s);
  }

  if (method == "COLOR")
  {
    // format1: METHOD, INPUT_ARRAY, COLOR_ARRAY, VALUE, COLOR
    dialog->getCombo(ai1l, s);
    parms.setData(ai1l, s);

    dialog->getCombo(cal, s);
    parms.setData(cal, s);

    s = QString::number(dialog->getDouble(cil));
    parms.setData(cil, s);

    QColor color;
    dialog->getColor(cl, color);
    s = color.name();
    parms.setData(cl, s);
  }

  if (method == "INRANGE")
  {
    // format1: METHOD, ARRAY_INPUT, DOUBLE, DOUBLE
    dialog->getCombo(ai1l, s);
    parms.setData(ai1l, s);

    QString s = QObject::tr("Min Array");
    dialog->getCombo(s, s2);
    parms.setData(s, s2);

    s = QObject::tr("Max Array");
    dialog->getCombo(s, s2);
    parms.setData(s, s2);
  }

  delete dialog;
}

UTIL::Operator UTIL::getOperator (QString &d)
{
  int i = opList.indexOf(d);
  return (Operator) i;
}


