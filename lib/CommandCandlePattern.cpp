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

#include "CommandCandlePattern.h"
#include "CandleType.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingDouble.h"
#include "SettingList.h"

#include <QtDebug>
#include <QList>

CommandCandlePattern::CommandCandlePattern (QObject *p) : Command (p)
{
  _type = "CANDLE_PATTERN";
}

int CommandCandlePattern::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  // verify NAME
  QString name = sg->get("NAME")->getString();
  Curve *line = script->curve(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME" << name;
    return _ERROR;
  }

  // verify OPEN
  QString key = sg->get("OPEN")->getString();
  QString s = script->setting(key)->getString();
  Curve *iopen = script->curve(s);
  if (! iopen)
  {
    qDebug() << _type << "::runScript: invalid OPEN" << s;
    return _ERROR;
  }

  key = sg->get("HIGH")->getString();
  s = script->setting(key)->getString();
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

  double pen = sg->get("PENETRATION")->getDouble();

  CandleType ct;
  s = sg->get("METHOD")->getString();
  int method = ct.fromString(s);
  if (method == -1)
  {
    qDebug() << _type << "::runScript: invalid METHOD" << s;
    return _ERROR;
  }

  QList<Curve *> list;
  list << iopen << ihigh << ilow << iclose;
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
  {
    qDebug() << _type << "::runScript: invalid keys";
    return _ERROR;
  }

  BarData bd;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *obar = iopen->bar(keys.at(loop));
    if (! obar)
      continue;

    CurveBar *hbar = ihigh->bar(keys.at(loop));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(loop));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(loop));
    if (! cbar)
      continue;

    Bar *b = new Bar;
    b->setOpen(obar->data());
    b->setHigh(hbar->data());
    b->setLow(lbar->data());
    b->setClose(cbar->data());
    bd.append(b);
  }

  line = ct.getPattern(&bd, method, pen);
  if (! line)
  {
    qDebug() << _type << "::runScript: CandleType error";
    return _ERROR;
  }

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

SettingGroup * CommandCandlePattern::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString());
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("OPEN");
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

  SettingDouble *sd = new SettingDouble(0, 0, 0.5, 1.0, 0.0);
  sd->setKey("PENETRATION");
  sg->set(sd);

  CandleType ct;
  SettingList *sl = new SettingList(ct.list(), QString("HARAMI"));
  sl->setKey("METHOD");
  sg->set(sl);

  return sg;
}
