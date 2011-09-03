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

#include "CommandMAMA.h"
#include "ta_libc.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandMAMA::CommandMAMA (QObject *p) : Command (p)
{
  _type = "MAMA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMAMA::CommandMAMA: error on TA_Initialize");
}

int CommandMAMA::runScript (Data *sg, Script *script)
{
  QString mname = sg->get("OUTPUT_MAMA").toString();
  Data *line = script->data(mname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_MAMA" << mname;
    return _ERROR;
  }

  QString fname = sg->get("OUTPUT_FAMA").toString();
  line = script->data(fname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_FAMA" << fname;
    return _ERROR;
  }

  QString s = sg->get("INPUT").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  double flimit = sg->get("LIMIT_FAST").toDouble();

  double slimit = sg->get("LIMIT_SLOW").toDouble();

  QList<Data *> list;
  list << in;
  QList<Data *> lines = getMAMA(list, flimit, slimit);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Data *mama = lines.at(0);
  script->setData(mname, mama);

  Data *fama = lines.at(1);
  script->setData(fname, fama);

  return _OK;
}

QList<Data *> CommandMAMA::getMAMA (QList<Data *> &list, double flimit, double slimit)
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
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &in[0], &in[0], &in[0], &in[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &in[0],
                          flimit,
                          slimit,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMAMA: TA-Lib error" << rc;
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

Data * CommandMAMA::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT_MAMA", QVariant(QString()));
  sg->set("OUTPUT_FAMA", QVariant(QString()));
  sg->set("INPUT", QVariant(QString()));
  sg->set("LIMIT_FAST", QVariant(0.5));
  sg->set("LIMIT_SLOW", QVariant(0.05));
  return sg;
}
