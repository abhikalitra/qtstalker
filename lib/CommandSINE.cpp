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

#include "CommandSINE.h"
#include "ta_libc.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandSINE::CommandSINE (QObject *p) : Command (p)
{
  _type = "HT_SINE";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandSINE::CommandSINE: error on TA_Initialize");
}

int CommandSINE::runScript (Data *sg, Script *script)
{
  QString sname = sg->get("OUTPUT_SINE").toString();
  Data *line = script->data(sname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_SINE" << sname;
    return _ERROR;
  }

  QString lname = sg->get("OUTPUT_LEAD").toString();
  line = script->data(lname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_LEAD" << lname;
    return _ERROR;
  }

  QString s = sg->get("INPUT").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in;
  QList<Data *> lines = getSINE(list);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Data *sline = lines.at(0);
  script->setData(sname, sline);

  Data *lline = lines.at(1);
  script->setData(lname, lline);

  return _OK;
}

QList<Data *> CommandSINE::getSINE (QList<Data *> &list)
{
  QList<Data *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input[0], &input[0], &input[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_HT_SINE (0,
                              size - 1,
                              &input[0],
                              &outBeg,
                              &outNb,
                              &out[0],
                              &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSINE: TA-Lib error" << rc;
    return lines;
  }

  lines.clear();
  Data *c = new CurveData;
  lines << c;
  c = new CurveData;
  lines << c;
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

Data * CommandSINE::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT_SINE", QVariant(QString()));
  sg->set("OUTPUT_LEAD", QVariant(QString()));
  sg->set("INPUT", QVariant(QString()));
  return sg;
}
