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

#include "CommandAD.h"
#include "ta_libc.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandAD::CommandAD (QObject *p) : Command (p)
{
  _type = "AD";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandAD::CommandAD: error on TA_Initialize");
}

int CommandAD::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("HIGH");
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  s = sg->get("LOW");
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  s = sg->get("CLOSE");
  Data *iclose = script->data(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  s = sg->get("VOLUME");
  Data *ivol = script->data(s);
  if (! ivol)
  {
    qDebug() << _type << "::runScript: invalid VOLUME" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose << ivol;
  line = getAD(list);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandAD::getAD (QList<Data *> &list)
{
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &close[0], &volume[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_AD(0,
                        size - 1,
                        &high[0],
                        &low[0],
                        &close[0],
                        &volume[0],
                        &outBeg,
                        &outNb,
                        &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getAD: TA-Lib error" << rc;
    return 0;
  }

  QList<Data *> outs;
  Data *c = new Data;
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

Data * CommandAD::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("HIGH", QString());
  sg->set("LOW", QString());
  sg->set("CLOSE", QString());
  sg->set("VOLUME", QString());
  return sg;
}
