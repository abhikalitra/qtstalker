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
#include "VerifyDataInput.h"
#include "DataDouble.h"

#include <QtDebug>

CommandShift::CommandShift (QObject *p) : Command (p)
{
  _name = "SHIFT";
}

int CommandShift::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandShift::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT
  s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandShift::runScript: invalid INPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // PERIOD
  int period = 10;
  s = sg->value("PERIOD");
  if (vdi.toInteger(script, s, period))
  {
    qDebug() << "CommandShift::runScript: invalid PERIOD, using default" << s;
    period = 10;
  }

  Data *line = getShift(in, period);
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandShift::getShift (Data *in, int period)
{
  QList<int> keys = in->keys();

  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->toData(keys.at(loop));

    int index = keys.at(loop) + period;
    if (index < 0)
      continue;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(bar->toData(CurveBar::_VALUE)->toDouble()));
    line->set(keys.at(loop) + period, b);
  }

  return line;
}
