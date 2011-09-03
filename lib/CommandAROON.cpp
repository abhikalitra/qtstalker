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

#include "CommandAROON.h"
#include "ta_libc.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandAROON::CommandAROON (QObject *p) : Command (p)
{
  _type = "AROON";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandAROON::CommandAROON: error on TA_Initialize");
}

int CommandAROON::runScript (Data *sg, Script *script)
{
  QString uname = sg->get("OUTPUT_UPPER").toString();
  Data *line = script->data(uname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_UPPER" << uname;
    return _ERROR;
  }

  QString lname = sg->get("OUTPUT_LOWER").toString();
  line = script->data(lname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_LOWER" << lname;
    return _ERROR;
  }

  QString s = sg->get("HIGH").toString();
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  s = sg->get("LOW").toString();
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD").toInt();

  QList<Data *> list;
  list << ihigh << ilow;
  QList<Data *> lines = getAROON(list, period);
  if (! lines.count())
    return _ERROR;

  Data *upper = lines.at(0);
  script->setData(uname, upper);

  if (lines.count() == 2)
  {
    Data *lower = lines.at(1);
    script->setData(lname, lower);
  }

  return _OK;
}

QList<Data *> CommandAROON::getAROON (QList<Data *> &list, int period)
{
  QList<Data *> lines;
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &low[0], &low[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_AROON(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           period,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getAROON: TA-Lib error" << rc;
    return lines;
  }

  Data *c = new CurveData;
  lines.append(c);
  c = new CurveData;
  lines.append(c);
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

Data * CommandAROON::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT_UPPER", QVariant(QString()));
  sg->set("OUTPUT_LOWER", QVariant(QString()));
  sg->set("HIGH", QVariant(QString()));
  sg->set("LOW", QVariant(QString()));
  sg->set("PERIOD", QVariant(14));
  return sg;
}
