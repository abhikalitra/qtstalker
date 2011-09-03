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

#include "CommandBBANDS.h"
#include "ta_libc.h"
#include "MAType.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandBBANDS::CommandBBANDS (QObject *p) : Command (p)
{
  _type = "BBANDS";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandBBANDS::CommandBBANDS: error on TA_Initialize");
}

int CommandBBANDS::runScript (Data *sg, Script *script)
{
  QString uname = sg->get("OUTPUT_UPPER").toString();
  Data *line = script->data(uname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_UPPER" << uname;
    return _ERROR;
  }

  QString mname = sg->get("OUTPUT_MIDDLE").toString();
  line = script->data(mname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_MIDDLE" << mname;
    return _ERROR;
  }

  QString lname = sg->get("OUTPUT_LOWER").toString();
  line = script->data(lname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_LOWER" << lname;
    return _ERROR;
  }

  QString s = sg->get("INPUT").toString();
  Data *input = script->data(s);
  if (! input)
  {
    qDebug() << _type << "::runScript: invalid INPUT" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD").toInt();

  MAType types;
  s = sg->get("MA_TYPE").toString();
  int type = types.fromString(s);
  if (type == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE" << s;
    return _ERROR;
  }

  double udev = sg->get("DEV_UP").toDouble();

  double ldev = sg->get("DEV_DOWN").toDouble();

  QList<Data *> list;
  list << input;
  QList<Data *> lines = getBBANDS(list, period, udev, ldev, type);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Data *upper = lines.at(0);
  script->setData(uname, upper);

  Data *middle = lines.at(1);
  script->setData(mname, middle);

  Data *lower = lines.at(2);
  script->setData(lname, lower);

  return _OK;
}

QList<Data *> CommandBBANDS::getBBANDS (QList<Data *> &list, int period, double udev, double ldev, int type)
{
  QList<Data *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real in[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &in[0], &in[0], &in[0], &in[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_BBANDS(0,
                            size - 1,
                            &in[0],
                            period,
                            udev,
                            ldev,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0],
			    &out3[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getBBANDS: TA-Lib error" << rc;
    return lines;
  }

  lines.clear();

  Data *c = new CurveData;
  lines << c;
  c = new CurveData;
  lines << c;
  c = new CurveData;
  lines << c;
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out3[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

Data * CommandBBANDS::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT_UPPER", QVariant(QString()));
  sg->set("OUTPUT_MIDDLE", QVariant(QString()));
  sg->set("OUTPUT_LOWER", QVariant(QString()));
  sg->set("INPUT", QVariant(QString()));
  sg->set("PERIOD", QVariant(14));
  sg->set("DEV_UP", QVariant(2.0));
  sg->set("DEV_DOWN", QVariant(2.0));
  sg->set("MA_TYPE", QVariant(QString("EMA")));
  return sg;
}
