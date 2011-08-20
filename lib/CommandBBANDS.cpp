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
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingDouble.h"
#include "SettingList.h"

#include <QtDebug>

CommandBBANDS::CommandBBANDS (QObject *p) : Command (p)
{
  _type = "BBANDS";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandBBANDS::CommandBBANDS: error on TA_Initialize");
}

int CommandBBANDS::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString uname = sg->get("NAME_UPPER")->getString();
  Curve *line = script->curve(uname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_UPPER" << uname;
    return _ERROR;
  }

  QString mname = sg->get("NAME_MIDDLE")->getString();
  line = script->curve(mname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_MIDDLE" << mname;
    return _ERROR;
  }

  QString lname = sg->get("NAME_LOWER")->getString();
  line = script->curve(lname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_LOWER" << lname;
    return _ERROR;
  }

  QString key = sg->get("INPUT")->getString();
  QString s = script->setting(key)->getString();
  Curve *input = script->curve(s);
  if (! input)
  {
    qDebug() << _type << "::runScript: invalid INPUT" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD")->getInteger();

  if (input->count() < period)
    return _ERROR;

  MAType types;
  s = sg->get("MA_TYPE")->getString();
  int type = types.fromString(s);
  if (type == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE" << s;
    return _ERROR;
  }

  double udev = sg->get("DEV_UP")->getDouble();

  double ldev = sg->get("DEV_DOWN")->getDouble();

  QList<Curve *> list;
  list << input;
  QList<Curve *> lines = getBBANDS(list, period, udev, ldev, type);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Curve *upper = lines.at(0);
  upper->setLabel(uname);
  script->setCurve(uname, upper);

  Curve *middle = lines.at(1);
  middle->setLabel(mname);
  script->setCurve(mname, middle);

  Curve *lower = lines.at(2);
  lower->setLabel(lname);
  script->setCurve(lname, lower);

  return _OK;
}

QList<Curve *> CommandBBANDS::getBBANDS (QList<Curve *> &list, int period, double udev, double ldev, int type)
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

SettingGroup * CommandBBANDS::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("BBU"));
  ss->setKey("NAME_UPPER");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("BBM"));
  ss->setKey("NAME_MIDDLE");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("BBL"));
  ss->setKey("NAME_LOWER");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 14, 9999, 2);
  si->setKey("PERIOD");
  sg->set(si);

  SettingDouble *sd = new SettingDouble(0, 0, 2.0, 9999.0, -9999.0);
  sd->setKey("DEV_UP");
  sg->set(sd);

  sd = new SettingDouble(0, 0, 2.0, 9999.0, -9999.0);
  sd->setKey("DEV_DOWN");
  sg->set(sd);

  MAType mat;
  SettingList *sl = new SettingList(mat.list(), QString("EMA"));
  sl->setKey("MA_TYPE");
  sg->set(sl);

  return sg;
}
