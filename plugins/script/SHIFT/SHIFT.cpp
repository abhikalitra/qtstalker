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

#include "SHIFT.h"
#include "Curve.h"
#include "Globals.h"
#include "InputType.h"
#include "RuleWidget.h"

#include <QtDebug>

SHIFT::SHIFT ()
{
  _plugin = "SHIFT";
  _type = "INDICATOR";
}

int SHIFT::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int rows = settings->getInt("ROWS");
  int loop = 0;
  for (; loop < rows; loop++)
  {
    // output
    int col = 0;
    QString key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    QString name = settings->data(key);
    Curve *line = i->line(name);
    if (line)
    {
      qDebug() << _plugin << "::calculate: duplicate OUTPUT" << name;
      return 1;
    }

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    InputType it;
    QStringList order;
    order << settings->data(key);
    QList<Curve *> list;
    if (it.inputs(list, order, i, bd))
    {
      qDebug() << _plugin << "::calculate: input missing";
      return 1;
    }

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    int period = settings->getInt(key);
  
    line = getSHIFT(list, period);
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

int SHIFT::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // PERIOD

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

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  QList<Curve *> list;
  list << in;
  line = getSHIFT(list, period);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * SHIFT::getSHIFT (QList<Curve *> &list, int period)
{
  if (! list.count())
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Curve *line = new Curve;
  Curve *in = list.at(0);
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    line->setBar(keys.at(loop) + period, new CurveBar(bar->data()));
  }
    
  return line;
}

QWidget * SHIFT::dialog (QWidget *p, Setting *set)
{
  QStringList header;
  header << tr("Output") << tr("Input") << tr("Period") << tr("Color") << tr("Style") << tr("Plot");

  QList<int> format;
  format << RuleWidget::_OUTPUT << RuleWidget::_INPUT << RuleWidget::_INTEGER;
  format << RuleWidget::_COLOR << RuleWidget::_STYLE << RuleWidget::_PLOT;

  RuleWidget *w = new RuleWidget(p, _plugin);
  w->setRules(set, format, header);
  w->loadSettings();
  return w;
}

void SHIFT::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SHIFT *o = new SHIFT;
  return ((Plugin *) o);
}
