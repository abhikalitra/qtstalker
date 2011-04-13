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

#include "MA.h"
#include "Curve.h"
#include "MAType.h"
#include "InputType.h"
#include "RuleWidget.h"

#include <QtDebug>

MA::MA ()
{
  _plugin = "MA";
  _type = "INDICATOR";
}

int MA::calculate (BarData *bd, Indicator *i, Setting *settings)
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

    // input
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
    if (! list.count())
      return 1;
    Curve *in = list.at(0);

    // type
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    MAType mat;
    int type = mat.fromString(settings->data(key));
  
    // period
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    int period = settings->getInt(key);

    Curve *ma = mat.getMA(in, period, type);
    if (! ma)
      return 1;

    // color
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    ma->setAllColor(QColor(settings->data(key)));
    
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    ma->setType(settings->data(key));
    
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    ma->setZ(settings->getInt(key));
    
    ma->setLabel(name);
    i->setLine(name, ma);
  }

  return 0;
}

int MA::command (Command *command)
{
  // PARMS:
  // METHOD
  // NAME
  // INPUT
  // PERIOD

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  MAType mat;
  int method = mat.fromString(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
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

  line = mat.getMA(in, period, method);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * MA::dialog (QWidget *p, Setting *set)
{
  QStringList header;
  header << tr("Output") << tr("Input") << tr("Type") << tr("Period") << tr("Color") << tr("Style") << tr("Plot");

  QList<int> format;
  format << RuleWidget::_OUTPUT << RuleWidget::_INPUT << RuleWidget::_MA;
  format << RuleWidget::_INTEGER << RuleWidget::_COLOR << RuleWidget::_STYLE << RuleWidget::_PLOT;

  RuleWidget *w = new RuleWidget(p, _plugin);
  w->setRules(set, format, header);
  w->loadSettings();
  return w;
}

void MA::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  MA *o = new MA;
  return ((Plugin *) o);
}
