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
#include "SettingString.h"
#include "SettingInteger.h"

#include <QtDebug>

CommandAROON::CommandAROON (QObject *p) : Command (p)
{
  _type = "AROON";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandAROON::CommandAROON: error on TA_Initialize");
}

int CommandAROON::runScript (void *d)
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

  QString lname = sg->get("NAME_LOWER")->getString();
  line = script->curve(lname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_LOWER" << lname;
    return _ERROR;
  }

  QString key = sg->get("HIGH")->getString();
  QString s = script->setting(key)->getString();
  Curve *ihigh = script->curve(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  key = sg->get("LOW")->getString();
  s = script->setting(key)->getString();
  Curve *ilow = script->curve(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD")->getInteger();

  QList<Curve *> list;
  list << ihigh << ilow;
  QList<Curve *> lines = getAROON(list, period);
  if (! lines.count())
    return _ERROR;

  Curve *upper = lines.at(0);
  upper->setLabel(uname);
  script->setCurve(uname, upper);

  if (lines.count() == 2)
  {
    Curve *lower = lines.at(1);
    lower->setLabel(lname);
    script->setCurve(lname, lower);
  }

  return _OK;
}

QList<Curve *> CommandAROON::getAROON (QList<Curve *> &list, int period)
{
  QList<Curve *> lines;
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

  Curve *c = new Curve;
  lines.append(c);
  c = new Curve;
  lines.append(c);
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

SettingGroup * CommandAROON::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("AROONU"));
  ss->setKey("NAME_UPPER");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("AROONL"));
  ss->setKey("NAME_LOWER");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("HIGH");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("LOW");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 14, 9999, 2);
  si->setKey("PERIOD");
  sg->set(si);

  return sg;
}
