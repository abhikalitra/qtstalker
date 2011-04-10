/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "ARITHMETIC.h"
#include "Curve.h"
#include "Globals.h"
#include "RuleWidget.h"
#include "InputType.h"

#include <QtDebug>

ARITHMETIC::ARITHMETIC ()
{
  _plugin = "ARITHMETIC";
  _type = "INDICATOR";
}

int ARITHMETIC::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int rows = settings->getInt("ROWS");
  int loop = 0;
  for (; loop < rows; loop++)
  {
    int col = 0;
    QString key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    QString name = settings->data(key);
    Curve *line = i->line(name);
    if (line)
    {
      qDebug() << _plugin << "::calculate: duplicate output" << name;
      return 1;
    }

    QStringList l;
    l << "ADD" << "DIV" << "MULT" << "SUB";
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    int method = l.indexOf(settings->data(key));

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    Curve *in = i->line(settings->data(key));
    if (! in)
    {
      InputType it;
      in = it.input(bd, settings->data(key));
      if (! in)
      {
        qDebug() << _plugin << "::calculate: no INPUT" << settings->data(key);
        return 1;
      }

      in->setLabel(settings->data(key));
      i->setLine(settings->data(key), in);
    }

    double value = 0;
    int valueFlag = FALSE;
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    Curve *in2 = i->line(settings->data(key));
    if (! in2)
    {
      InputType it;
      in2 = it.input(bd, settings->data(key));
      if (! in2)
      {
        bool ok;
        value = settings->data(key).toDouble(&ok);
	if (! ok)
	{
          qDebug() << _plugin << "::calculate: invalid INPUT2" << settings->data(key);
          return 1;
	}

	valueFlag++;
      }
      else
      {
        in2->setLabel(settings->data(key));
        i->setLine(settings->data(key), in2);
      }
    }

    int high = 0;
    int low = 0;
    in->keyRange(low, high);

    if (! valueFlag)
    {
      int tlow = 0;
      int thigh = 0;
      in2->keyRange(tlow, thigh);
      if (tlow < low)
        low = tlow;
      if (thigh > high)
        high = thigh;
    }

    line = new Curve;
    int loop2 = low;
    for (; loop2 <= high; loop2++)
    {
      CurveBar *bar = in->bar(loop2);
      if (! bar)
        continue;

      double v = 0;
      if (valueFlag)
        v = value;
      else
      {
        CurveBar *bar2 = in2->bar(loop2);
        if (! bar2)
          continue;
        v = bar2->data();
      }

      switch (method)
      {
        case 0: // add
          line->setBar(loop2, new CurveBar(bar->data() + v));
	  break;
        case 1: // div
          line->setBar(loop2, new CurveBar(bar->data() / v));
	  break;
        case 2: // mult
          line->setBar(loop2, new CurveBar(bar->data() * v));
	  break;
        case 3: // sub
          line->setBar(loop2, new CurveBar(bar->data() - v));
	  break;
        default:
	  break;
      }
    }
    
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    line->setAllColor(QColor(settings->data(key)));
    
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    line->setType(settings->data(key));
    
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    line->setZ(settings->getInt(key));
    
    line->setLabel(name);
    i->setLine(name, line);
  }

  return 0;
}

int ARITHMETIC::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // INPUT2
  // METHOD

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify NAME
  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  // verify INPUT
  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT not found" << command->parm("INPUT");
    return 1;
  }

  // verify INPUT2
  Curve *in2 = i->line(command->parm("INPUT2"));
  if (! in2)
  {
    qDebug() << _plugin << "::command: invalid INPUT2" << command->parm("INPUT2");
    return 1;
  }

  QStringList l;
  l << "ADD" << "DIV" << "MULT" << "SUB";
  int method = l.indexOf(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
    return 1;
  }

  QList<int> keys;
  int size = in->count();
  if (in2->count() > size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  line = new Curve;
  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    switch (method)
    {
      case 0: // add
        line->setBar(keys.at(loop), new CurveBar(bar->data() + bar2->data()));
	break;
      case 1: // div
        line->setBar(keys.at(loop), new CurveBar(bar->data() / bar2->data()));
	break;
      case 2: // mult
        line->setBar(keys.at(loop), new CurveBar(bar->data() * bar2->data()));
	break;
      case 3: // sub
        line->setBar(keys.at(loop), new CurveBar(bar->data() - bar2->data()));
	break;
      default:
	break;
    }
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * ARITHMETIC::dialog (QWidget *p, Setting *set)
{
  QStringList header;
  header << tr("Output") << tr("Method") << tr("Input 1") << tr("Input 2") << tr("Color");
  header << tr("Style") << tr("Plot");

  QList<int> format;
  format << RuleWidget::_OUTPUT << RuleWidget::_LIST << RuleWidget::_INPUT;
  format << RuleWidget::_INPUT << RuleWidget::_COLOR << RuleWidget::_STYLE << RuleWidget::_PLOT;

  RuleWidget *w = new RuleWidget(p, _plugin);
  QStringList l;
  l << "ADD" << "DIV" << "MULT" << "SUB";
  w->setList(l);
  w->setRules(set, format, header);
  w->loadSettings();
  return w;
}

void ARITHMETIC::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  ARITHMETIC *o = new ARITHMETIC;
  return ((Plugin *) o);
}