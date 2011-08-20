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

#include "CommandADX.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingList.h"

#include <QtDebug>

CommandADX::CommandADX (QObject *p) : Command (p)
{
  _type = "ADX";
  _method << "ADX" << "ADXR" << "MDI" << "PDI";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandADX::CommandADX: error on TA_Initialize");
}

int CommandADX::runScript (void *d)
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

  key = sg->get("CLOSE")->getString();
  s = script->setting(key)->getString();
  Curve *iclose = script->curve(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD")->getInteger();

  s = sg->get("METHOD")->getString();
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << _type << "::runScript: invalid METHOD" << s;
    return _ERROR;
  }

  QList<Curve *> list;
  list << ihigh << ilow << iclose;
  line = getADX(list, period, method);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandADX::getADX (QList<Curve *> &list, int period, int method)
{
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &close[0], &close[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;

  switch (method)
  {
    case 0: // ADX
      rc = TA_ADX(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1: // ADXR
      rc = TA_ADXR(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 2: // MDI
      rc = TA_MINUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3: // PDI
      rc = TA_PLUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getADX: TA-Lib error" << rc;
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

SettingGroup * CommandADX::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("AD"));
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("HIGH");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("LOW");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("CLOSE");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 14, 9999, 2);
  si->setKey("PERIOD");
  sg->set(si);

  SettingList *sl = new SettingList(_method, QString("ADX"));
  sl->setKey("METHOD");
  sg->set(sl);

  return sg;
}
