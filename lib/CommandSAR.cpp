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

#include "CommandSAR.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingDouble.h"

#include <QtDebug>

CommandSAR::CommandSAR (QObject *p) : Command (p)
{
  _type = "SAR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandSAR::CommandSAR: error on TA_Initialize");
}

int CommandSAR::runScript (void *d)
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

  double init = sg->get("STEP_INITIAL")->getDouble();

  double max = sg->get("STEP_MAX")->getDouble();

  QList<Curve *> list;
  list << ihigh << ilow;
  line = getSAR(list, init, max);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandSAR::getSAR (QList<Curve *> &list, double init, double max)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &low[0], &low[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SAR(0,
                         size - 1,
                         &high[0],
                         &low[0],
			 init,
			 max,
                         &outBeg,
                         &outNb,
                         &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSAR: TA-Lib error" << rc;
    return 0;
  }

  QList<Curve *> outs;
  Curve *c = new Curve;
  c->setType("Dot");
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

SettingGroup * CommandSAR::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("HIGH");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("LOW");
  sg->set(ss);

  SettingDouble *sd = new SettingDouble(0, 0, 0.02, 1, 0);
  sd->setKey("STEP_INITIAL");
  sg->set(sd);

  sd = new SettingDouble(0, 0, 0.2, 1, 0);
  sd->setKey("STEP_MAX");
  sg->set(sd);

  return sg;
}
