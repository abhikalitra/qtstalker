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

#include "CommandStochFast.h"
#include "ta_libc.h"
#include "MAType.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingList.h"

#include <QtDebug>

CommandStochFast::CommandStochFast (QObject *p) : Command (p)
{
  _type = "STOCH_FAST";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandStochFast::CommandStochFast: error on TA_Initialize");
}

int CommandStochFast::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString kname = sg->get("NAME_FASTK")->getString();
  Curve *line = script->curve(kname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_FASTK" << kname;
    return _ERROR;
  }

  QString dname = sg->get("NAME_FASTD")->getString();
  line = script->curve(dname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_FASTD" << dname;
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

  int kperiod = sg->get("PERIOD_FASTK")->getInteger();

  int dperiod = sg->get("PERIOD_FASTD")->getInteger();

  MAType mat;
  s = sg->get("MA_TYPE")->getString();
  int type = mat.fromString(s);
  if (type == -1)
  {
    qDebug() << _type << "::runScript: invalid MA_TYPE" << s;
    return _ERROR;
  }

  QList<Curve *> list;
  list << ihigh << ilow << iclose;
  QList<Curve *> lines = getSTOCHF(list, kperiod, dperiod, type);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Curve *kline = lines.at(0);
  kline->setLabel(kname);
  script->setCurve(kname, kline);

  Curve *dline = lines.at(1);
  dline->setLabel(dname);
  script->setCurve(dname, dline);

  return _OK;
}

QList<Curve *> CommandStochFast::getSTOCHF (QList<Curve *> &list, int kperiod, int dperiod, int type)
{
  QList<Curve *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &close[0], &close[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_STOCHF(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            kperiod,
                            dperiod,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSTOCHF: TA-Lib error" << rc;
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

SettingGroup * CommandStochFast::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("%K"));
  ss->setKey("NAME_FASTK");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("%D"));
  ss->setKey("NAME_FASTD");
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

  SettingInteger *si = new SettingInteger(0, 0, 5, 9999, 2);
  si->setKey("PERIOD_FASTK");
  sg->set(si);

  si = new SettingInteger(0, 0, 3, 9999, 2);
  si->setKey("PERIOD_FASTD");
  sg->set(si);

  MAType mat;
  SettingList *sl = new SettingList(mat.list(), QString("EMA"));
  sl->setKey("MA_TYPE");
  sg->set(sl);

  return sg;
}
