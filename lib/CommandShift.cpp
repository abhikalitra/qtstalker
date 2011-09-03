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

#include "CommandShift.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandShift::CommandShift (QObject *p) : Command (p)
{
  _type = "SHIFT";
}

int CommandShift::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("INPUT").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD").toInt();

  line = getShift(in, period);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandShift::getShift (Data *in, int period)
{
  QList<int> keys = in->barKeys();

  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->getData(keys.at(loop));

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, bar->get(CurveBar::_VALUE));
    line->set(keys.at(loop) + period, b);
  }

  return line;
}

Data * CommandShift::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("INPUT", QVariant(QString("close")));
  sg->set("PERIOD", QVariant(10));
  return sg;
}
