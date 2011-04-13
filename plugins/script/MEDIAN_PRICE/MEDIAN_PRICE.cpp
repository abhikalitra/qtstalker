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

#include "MEDIAN_PRICE.h"
#include "Curve.h"
#include "Globals.h"
#include "InputType.h"
#include "RuleWidget.h"

#include <QtDebug>

MEDIAN_PRICE::MEDIAN_PRICE ()
{
  _plugin = "MEDIAN_PRICE";
  _type = "INDICATOR";
}

int MEDIAN_PRICE::calculate (BarData *bd, Indicator *i, Setting *settings)
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

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    QStringList order;
    order << settings->data(key);
    
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    order << settings->data(key);

    InputType it;
    QList<Curve *> list;
    if (it.inputs(list, order, i, bd))
    {
      qDebug() << _plugin << "::calculate: input missing";
      return 1;
    }

    line = getMP(list);
    if (! line)
      return 1;

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

int MEDIAN_PRICE::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // INPUT2

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  Curve *in2 = i->line(command->parm("INPUT2"));
  if (! in2)
  {
    qDebug() << _plugin << "::command: INPUT2 missing" << command->parm("INPUT2");
    return 1;
  }

  QList<Curve *> list;
  list << in << in2;
  line = getMP(list);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * MEDIAN_PRICE::getMP (QList<Curve *> &list)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Curve *line = new Curve;
  Curve *in = list.at(0);
  Curve *in2 = list.at(1);
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    double t = (bar->data() + bar2->data()) / 2.0;
    line->setBar(keys.at(loop), new CurveBar(t));
  }

  return line;
}

QWidget * MEDIAN_PRICE::dialog (QWidget *p, Setting *set)
{
  QStringList header;
  header << tr("Output") << tr("Input 1") << tr("Input 2") << tr("Color") << tr("Style") << tr("Plot");

  QList<int> format;
  format << RuleWidget::_OUTPUT << RuleWidget::_INPUT << RuleWidget::_INPUT;
  format << RuleWidget::_COLOR << RuleWidget::_STYLE << RuleWidget::_PLOT;

  RuleWidget *w = new RuleWidget(p, _plugin);
  w->setRules(set, format, header);
  w->loadSettings();
  return w;
}

void MEDIAN_PRICE::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  MEDIAN_PRICE *o = new MEDIAN_PRICE;
  return ((Plugin *) o);
}
