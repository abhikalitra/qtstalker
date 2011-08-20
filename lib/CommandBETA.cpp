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

#include "CommandBETA.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingInteger.h"

#include <QtDebug>

CommandBETA::CommandBETA (QObject *p) : Command (p)
{
  _type = "BETA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandBETA::CommandBETA: error on TA_Initialize");
}

int CommandBETA::runScript (void *d)
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
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  key = sg->get("INDEX")->getString();
  s = script->setting(key)->getString();
  Curve *in2 = script->curve(s);
  if (! in2)
  {
    qDebug() << _type << "::runScript: INDEX missing" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD")->getInteger();

  QList<Curve *> list;
  list << in << in2;
  line = getBETA(list, period);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandBETA::getBETA (QList<Curve *> &list, int period)
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

  TA_RetCode rc = TA_BETA(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          period,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getBETA: TA-Lib error" << rc;
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

SettingGroup * CommandBETA::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INDEX");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 5, 9999, 2);
  si->setKey("PERIOD");
  sg->set(si);

  return sg;
}
