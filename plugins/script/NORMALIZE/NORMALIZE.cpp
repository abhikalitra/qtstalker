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

#include "NORMALIZE.h"
#include "Curve.h"
#include "Globals.h"
#include "InputType.h"
#include "RuleWidget.h"

#include <QtDebug>
#include <cmath>

NORMALIZE::NORMALIZE ()
{
  _plugin = "NORMALIZE";
  _type = "INDICATOR";
}

int NORMALIZE::calculate (BarData *bd, Indicator *i, Setting *settings)
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
    Curve *in = i->line(settings->data(key));
    if (! in)
    {
      InputType it;
      in = it.input(bd, settings->data(key));
      if (! in)
      {
        qDebug() << _plugin << "::calculate: no input" << settings->data(key);
        return 1;
      }

      in->setZ(-1);
      in->setLabel(settings->data(key));
      i->setLine(settings->data(key), in);
    }

    line = new Curve;
    double max = 0;
    double min = 0;
    QList<int> keys;
    in->keys(keys);
    in->highLowRange(keys.at(0), keys.count() - 1, max, min);
    double range = fabs(max) + fabs(min);
    int loop2 = 0;
    for (; loop2 < keys.count(); loop2++)
    {
      CurveBar *bar = in->bar(keys.at(loop2));
      line->setBar(keys.at(loop2), new CurveBar(((bar->data() - min) / range) * 100));
    }

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    in->setAllColor(QColor(settings->data(key)));

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    in->setType(settings->data(key));

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    in->setZ(settings->getInt(key));

    in->setLabel(name);
    i->setLine(name, in);
  }
  
  return 0;
}

int NORMALIZE::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT

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
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  if (in->count() < 1)
    return 1;

  line = new Curve;
  
  double max = 0;
  double min = 0;
  QList<int> keys;
  in->keys(keys);
  in->highLowRange(keys.at(0), keys.count() - 1, max, min);

  double range = fabs(max) + fabs(min);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    line->setBar(keys.at(loop), new CurveBar(((bar->data() - min) / range) * 100));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * NORMALIZE::dialog (QWidget *p, Setting *set)
{
  QStringList header;
  header << tr("Output") << tr("Input") << tr("Color") << tr("Style") << tr("Plot");

  QList<int> format;
  format << RuleWidget::_OUTPUT << RuleWidget::_INPUT << RuleWidget::_COLOR;
  format << RuleWidget::_STYLE << RuleWidget::_PLOT;

  RuleWidget *w = new RuleWidget(p, _plugin);
  w->setRules(set, format, header);
  w->loadSettings();
  return w;
}

void NORMALIZE::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  NORMALIZE *o = new NORMALIZE;
  return ((Plugin *) o);
}
