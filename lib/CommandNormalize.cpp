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

#include "CommandNormalize.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"
#include "SettingDouble.h"

#include <QtDebug>
#include <cmath>

CommandNormalize::CommandNormalize (QObject *p) : Command (p)
{
  _name = "NORMALIZE";
}

int CommandNormalize::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *name = vdi.setting(SettingFactory::_STRING, script, s);
  if (! name)
  {
    _message << "invalid OUTPUT " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("INPUT");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    _message << "INPUT missing " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << in;

  Data *line = getNORM(list);
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name->toString(), line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandNormalize::getNORM (QList<Data *> &list)
{
  if (! list.count())
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  Data *in = list.at(0);
  double max = 0;
  double min = 0;
  if (in->highLow(max, min))
    return 0;

  double range = fabs(max) + fabs(min);

  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->getData(keys.at(loop));
    double t = ((bar->get(CurveBar::_VALUE)->toDouble() - min) / range) * 100;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new SettingDouble(t));
    line->set(keys.at(loop), b);
  }

  return line;
}
