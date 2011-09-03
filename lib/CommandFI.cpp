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
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandFI::CommandFI (QObject *p) : Command (p)
{
  _type = "FI";
}

int CommandFI::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("CLOSE").toString();
  Data *iclose = script->data(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  s = sg->get("VOLUME").toString();
  Data *ivol = script->data(s);
  if (! ivol)
  {
    qDebug() << _type << "::runScript: invalid VOLUME" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << iclose << ivol;
  line = getFI(list);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandFI::getFI (QList<Data *> &list)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
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

    double cdiff = cbar->get(CurveBar::_VALUE).toDouble() - ycbar->get(CurveBar::_VALUE).toDouble();
    force = vbar->get(CurveBar::_VALUE).toDouble() * cdiff;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, QVariant(force));
    line->set(keys.at(loop), b);
  }

  return line;
}

Data * CommandFI::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("CLOSE", QVariant(QString()));
  sg->set("VOLUME", QVariant(QString()));
  return sg;
}
