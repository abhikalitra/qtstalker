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
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingList.h"

#include <QtDebug>

CommandMACD::CommandMACD (QObject *p) : Command (p)
{
  _type = "MACD";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMACD::CommandMACD: error on TA_Initialize");
}

int CommandMACD::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString mname = sg->get("NAME_MACD")->getString();
  Curve *line = script->curve(mname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_MACD" << mname;
    return _ERROR;
  }

  QString sname = sg->get("NAME_SIGNAL")->getString();
  line = script->curve(sname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_SIGNAL" << sname;
    return _ERROR;
  }

  QString hname = sg->get("NAME_HIST")->getString();
  line = script->curve(hname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_HIST" << hname;
    return _ERROR;
  }

  QString key = sg->get("INPUT")->getString();
  QString s = script->setting(key)->getString();
  Curve *in = script->curve(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  int fperiod = sg->get("PERIOD_FAST")->getInteger();

  MAType mat;
  s = sg->get("MA_TYPE_FAST")->getString();
  int ftype = mat.fromString(s);
  if (ftype == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE_FAST" << s;
    return _ERROR;
  }

  int speriod = sg->get("PERIOD_SLOW")->getInteger();

  s = sg->get("MA_TYPE_SLOW")->getString();
  int stype = mat.fromString(s);
  if (stype == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE_SLOW" << s;
    return _ERROR;
  }

  int sigperiod = sg->get("PERIOD_SIGNAL")->getInteger();

  s = sg->get("MA_TYPE_SIGNAL")->getString();
  int sigtype = mat.fromString(s);
  if (sigtype == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE_SIGNAL" << s;
    return _ERROR;
  }

  QList<Curve *> list;
  list << in;
  QList<Curve *> lines = getMACD(list, fperiod, speriod, sigperiod, ftype, stype, sigtype);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Curve *macd = lines.at(0);
  macd->setLabel(mname);
  script->setCurve(mname, macd);

  Curve *signal = lines.at(1);
  signal->setLabel(sname);
  script->setCurve(sname, signal);

  Curve *hist = lines.at(2);
  hist->setLabel(hname);
  script->setCurve(hname, hist);

  return _OK;
}

QList<Curve *> CommandMACD::getMACD (QList<Curve *> &list, int fp, int sp, int sigp, int fma, int sma, int sigma)
{
  QList<Curve *> lines;
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

  Curve *c = new Curve;
  lines.append(c);
  c = new Curve;
  lines.append(c);
  c = new Curve;
  lines.append(c);
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out3[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

SettingGroup * CommandMACD::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_MACD");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_SIGNAL");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_HIST");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 12, 9999, 2);
  si->setKey("PERIOD_FAST");
  sg->set(si);

  si = new SettingInteger(0, 0, 26, 9999, 2);
  si->setKey("PERIOD_SLOW");
  sg->set(si);

  si = new SettingInteger(0, 0, 9, 9999, 2);
  si->setKey("PERIOD_SIGNAL");
  sg->set(si);

  MAType mat;
  SettingList *sl = new SettingList(mat.list(), QString("EMA"));
  sl->setKey("MA_TYPE_FAST");
  sg->set(sl);

  sl = new SettingList(mat.list(), QString("EMA"));
  sl->setKey("MA_TYPE_SLOW");
  sg->set(sl);

  sl = new SettingList(mat.list(), QString("EMA"));
  sl->setKey("MA_TYPE_SIGNAL");
  sg->set(sl);

  return sg;
}
