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

#include "CommandSTDDEV.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingDouble.h"

#include <QtDebug>

CommandSTDDEV::CommandSTDDEV (QObject *p) : Command (p)
{
  _type = "STDDEV";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandSTDDEV::CommandSTDDEV: error on TA_Initialize");
}

int CommandSTDDEV::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString name = sg->get("NAME")->getString();
  Curve *line = script->curve(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate name" << name;
    return _ERROR;
  }

  QString key = sg->get("INPUT")->getString();
  QString s = script->setting(key)->getString();
  Curve *in = script->curve(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: invalid INPUT" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD")->getInteger();

  double dev = sg->get("DEVIATION")->getDouble();

  QList<Curve *> list;
  list << in;
  line = getSTDDEV(list, period, dev);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandSTDDEV::getSTDDEV (QList<Curve *> &list, int period, double dev)
{
  if (! list.count())
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input[0], &input[0], &input[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_STDDEV(0,
                            size - 1,
                            &input[0],
			    period,
			    dev,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSTDDEV: TA-Lib error" << rc;
    return 0;
  }

  QList<Curve *> outs;
  Curve *c = new Curve;
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

SettingGroup * CommandSTDDEV::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 20, 9999, 2);
  si->setKey("PERIOD");
  sg->set(si);

  SettingDouble *sd = new SettingDouble(0, 0, 2.0, 9999.0, -9999.0);
  sd->setKey("DEVIATION");
  sg->set(sd);

  return sg;
}
