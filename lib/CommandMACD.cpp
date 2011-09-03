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

#include "CommandMACD.h"
#include "ta_libc.h"
#include "InputType.h"
#include "MAType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandMACD::CommandMACD (QObject *p) : Command (p)
{
  _type = "MACD";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMACD::CommandMACD: error on TA_Initialize");
}

int CommandMACD::runScript (Data *sg, Script *script)
{
  QString mname = sg->get("OUTPUT_MACD").toString();
  Data *line = script->data(mname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_MACD" << mname;
    return _ERROR;
  }

  QString sname = sg->get("OUTPUT_SIGNAL").toString();
  line = script->data(sname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_SIGNAL" << sname;
    return _ERROR;
  }

  QString hname = sg->get("OUTPUT_HIST").toString();
  line = script->data(hname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT_HIST" << hname;
    return _ERROR;
  }

  QString s = sg->get("INPUT").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  int fperiod = sg->get("PERIOD_FAST").toInt();

  MAType mat;
  s = sg->get("MA_TYPE_FAST").toString();
  int ftype = mat.fromString(s);
  if (ftype == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE_FAST" << s;
    return _ERROR;
  }

  int speriod = sg->get("PERIOD_SLOW").toInt();

  s = sg->get("MA_TYPE_SLOW").toString();
  int stype = mat.fromString(s);
  if (stype == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE_SLOW" << s;
    return _ERROR;
  }

  int sigperiod = sg->get("PERIOD_SIGNAL").toInt();

  s = sg->get("MA_TYPE_SIGNAL").toString();
  int sigtype = mat.fromString(s);
  if (sigtype == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE_SIGNAL" << s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in;
  QList<Data *> lines = getMACD(list, fperiod, speriod, sigperiod, ftype, stype, sigtype);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Data *macd = lines.at(0);
  script->setData(mname, macd);

  Data *signal = lines.at(1);
  script->setData(sname, signal);

  Data *hist = lines.at(2);
  script->setData(hname, hist);

  return _OK;
}

QList<Data *> CommandMACD::getMACD (QList<Data *> &list, int fp, int sp, int sigp, int fma, int sma, int sigma)
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

  TA_RetCode rc = TA_MACDEXT(0,
                             size - 1,
                             &in[0],
                             fp,
                             (TA_MAType) fma,
                             sp,
                             (TA_MAType) sma,
                             sigp,
                             (TA_MAType) sigma,
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0],
                             &out3[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMACD: TA-Lib error" << rc;
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

Data * CommandMACD::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT_MACD", QVariant(QString()));
  sg->set("OUTPUT_SIGNAL", QVariant(QString()));
  sg->set("OUTPUT_HIST", QVariant(QString()));
  sg->set("INPUT", QVariant(QString()));
  sg->set("PERIOD_FAST", QVariant(12));
  sg->set("PERIOD_SLOW", QVariant(26));
  sg->set("PERIOD_SIGNAL", QVariant(9));
  sg->set("MA_TYPE_FAST", QVariant(QString("EMA")));
  sg->set("MA_TYPE_SLOW", QVariant(QString("EMA")));
  sg->set("MA_TYPE_SIGNAL", QVariant(QString("EMA")));
  return sg;
}
