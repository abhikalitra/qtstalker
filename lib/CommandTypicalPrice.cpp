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

#include "CommandTypicalPrice.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "DataDouble.h"

#include <QtDebug>

CommandTypicalPrice::CommandTypicalPrice (QObject *p) : Command (p)
{
  _name = "TYPICAL_PRICE";
}

int CommandTypicalPrice::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandTypicalPrice::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // HIGH
  s = sg->value("HIGH");
  Data *ihigh = vdi.toCurve(script, s);
  if (! ihigh)
  {
    qDebug() << "CommandTypicalPrice::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LOW
  s = sg->value("LOW");
  Data *ilow = vdi.toCurve(script, s);
  if (! ilow)
  {
    qDebug() << "CommandTypicalPrice::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // CLOSE
  s = sg->value("CLOSE");
  Data *iclose = vdi.toCurve(script, s);
  if (! iclose)
  {
    qDebug() << "CommandTypicalPrice::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose;

  Data *line = getTP(list);
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

Data * CommandTypicalPrice::getTP (QList<Data *> &list)
{
  if (list.count() != 3)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  Data *line = new CurveData;
  int loop = 0;
  Data *ihigh = list.at(loop++);
  Data *ilow = list.at(loop++);
  Data *iclose = list.at(loop++);
  for (loop = 0; loop < keys.count(); loop++)
  {
    Data *hbar = ihigh->toData(keys.at(loop));
    if (! hbar)
      continue;

    Data *lbar = ilow->toData(keys.at(loop));
    if (! lbar)
      continue;

    Data *cbar = iclose->toData(keys.at(loop));
    if (! cbar)
      continue;

    double t = (hbar->toData(CurveBar::_VALUE)->toDouble() +
                lbar->toData(CurveBar::_VALUE)->toDouble() +
                cbar->toData(CurveBar::_VALUE)->toDouble()) / 3.0;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(t));
    line->set(keys.at(loop), b);
  }

  return line;
}
