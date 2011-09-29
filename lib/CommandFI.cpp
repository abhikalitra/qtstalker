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
 *  MERCHANTABILITY or CommandFITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "CommandFI.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"
#include "SettingDouble.h"

#include <QtDebug>

CommandFI::CommandFI (QObject *p) : Command (p)
{
  _name = "FI";
}

int CommandFI::runScript (Message *sg, Script *script)
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

  s = sg->value("CLOSE");
  Data *iclose = vdi.curve(script, s);
  if (! iclose)
  {
    _message << "invalid CLOSE " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("VOLUME");
  Data *ivol = vdi.curve(script, s);
  if (! ivol)
  {
    _message << "invalid VOLUME " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << iclose << ivol;

  Data *line = getFI(list);
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

Data * CommandFI::getFI (QList<Data *> &list)
{
  if (list.count() != 2)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  Data *line = new CurveData;
  Data *close = list.at(0);
  Data *vol = list.at(1);
  int loop = 1;
  double force = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *cbar = close->getData(keys.at(loop));
    if (! cbar)
      continue;

    Data *ycbar = close->getData(keys.at(loop - 1));
    if (! ycbar)
      continue;

    Data *vbar = vol->getData(keys.at(loop));
    if (! vbar)
      continue;

    double cdiff = cbar->get(CurveBar::_VALUE)->toDouble() - ycbar->get(CurveBar::_VALUE)->toDouble();
    force = vbar->get(CurveBar::_VALUE)->toDouble() * cdiff;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new SettingDouble(force));
    line->set(keys.at(loop), b);
  }

  return line;
}
