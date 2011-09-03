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

#include "CommandULTOSC.h"
#include "ta_libc.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandULTOSC::CommandULTOSC (QObject *p) : Command (p)
{
  _type = "ULTOSC";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandULTOSC::CommandULTOSC: error on TA_Initialize");
}

int CommandULTOSC::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("HIGH").toString();
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::command: invalid HIGH" << s;
    return 1;
  }

  s = sg->get("LOW").toString();
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::command: invalid LOW" << s;
    return 1;
  }

  s = sg->get("CLOSE").toString();
  Data *iclose = script->data(s);
  if (! iclose)
  {
    qDebug() << _type << "::command: invalid CLOSE" << s;
    return 1;
  }

  int sp = sg->get("PERIOD_SHORT").toInt();

  int mp = sg->get("PERIOD_MED").toInt();

  int lp = sg->get("PERIOD_LONG").toInt();

  QList<Data *> list;
  list << ihigh << ilow << iclose;
  line = getULTOSC(list, sp, mp, lp);
  if (! line)
    return 1;

  script->setData(name, line);

  return _OK;
}

Data * CommandULTOSC::getULTOSC (QList<Data *> &list, int sp, int mp, int lp)
{
  if (list.count() != 3)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &close[0], &close[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
			    sp,
			    mp,
			    lp,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getULTOSC: TA-Lib error" << rc;
    return 0;
  }

  QList<Data *> outs;
  Data *c = new CurveData;
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

Data * CommandULTOSC::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("HIGH", QVariant(QString()));
  sg->set("LOW", QVariant(QString()));
  sg->set("CLOSE", QVariant(QString()));
  sg->set("PERIOD_SHORT", QVariant(7));
  sg->set("PERIOD_MED", QVariant(14));
  sg->set("PERIOD_LONG", QVariant(28));
  return sg;
}
