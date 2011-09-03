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

#include "CommandMAVP.h"
#include "ta_libc.h"
#include "InputType.h"
#include "MAType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandMAVP::CommandMAVP (QObject *p) : Command (p)
{
  _type = "MAVP";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMAVP::CommandMAVP: error on TA_Initialize");
}

int CommandMAVP::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("INPUT_1").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT_1 missing" << s;
    return _ERROR;
  }

  s = sg->get("INPUT_2").toString();
  Data *in2 = script->data(s);
  if (! in2)
  {
    qDebug() << _type << "::runScript: INPUT_2 missing" << s;
    return _ERROR;
  }

  int min = sg->get("PERIOD_MIN").toInt();

  int max = sg->get("PERIOD_MAX").toInt();

  MAType mat;
  s = sg->get("MA_TYPE").toString();
  int type = mat.fromString(s);
  if (type == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in << in2;
  line = getMAVP(list, min, max, type);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandMAVP::getMAVP (QList<Data *> &list, int min, int max, int type)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input2[0], &input2[0], &input2[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_MAVP(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          min,
                          max,
                          (TA_MAType) type,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMAVP: TA-Lib error" << rc;
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

Data * CommandMAVP::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("INPUT_1", QVariant(QString()));
  sg->set("INPUT_2", QVariant(QString()));
  sg->set("PERIOD_MIN", QVariant(2));
  sg->set("PERIOD_MAX", QVariant(30));
  sg->set("MA_TYPE", QVariant(QString("EMA")));
  return sg;
}
