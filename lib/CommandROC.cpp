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

#include "CommandROC.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingList.h"

#include <QtDebug>

CommandROC::CommandROC (QObject *p) : Command (p)
{
  _type = "ROC";
  _method << "ROC" << "ROCP" << "ROCR" << "ROCR100";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandROC::CommandROC: error on TA_Initialize");
}

int CommandROC::runScript (void *d)
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

  s = sg->get("METHOD")->getString();
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << _type << "::command: invalid METHOD" << s;
    return _ERROR;
  }

  QList<Curve *> list;
  list << in;
  line = getROC(list, period, method);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandROC::getROC (QList<Curve *> &list, int period, int method)
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

  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_ROC(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_ROCP(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_ROCR(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_ROCR100(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getROC: TA-Lib error" << rc;
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

SettingGroup * CommandROC::settings ()
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

  SettingList *sl = new SettingList(_method, _method.at(0));
  sl->setKey("METHOD");
  sg->set(sl);

  return sg;
}
