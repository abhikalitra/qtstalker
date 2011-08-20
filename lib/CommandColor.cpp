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

#include "CommandColor.h"
#include "Globals.h"
#include "Script.h"
#include "Operator.h"
#include "SettingString.h"
#include "SettingList.h"
#include "SettingColor.h"
#include "SettingInteger.h"

#include <QtDebug>

CommandColor::CommandColor (QObject *p) : Command (p)
{
  _type = "COLOR";
}

int CommandColor::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  // verify NAME
  QString key = sg->get("NAME")->getString();
  QString name = script->setting(key)->getString();
  Curve *line = script->curve(name);
  if (! line)
  {
    qDebug() << _type << "::runScript: NAME not found" << name;
    return _ERROR;
  }

  // get NAME_OFFSET
  int offset = sg->get("NAME_OFFSET")->getInteger();

  // verify OP
  Operator top;
  QString s = sg->get("OP")->getString();
  Operator::Type op = top.stringToOperator(s);
  if (op == -1)
  {
    qDebug() << _type << "::runScript: invalid OP" << s;
    return _ERROR;
  }

  // verify NAME2
  int valueFlag2 = FALSE;
  double value2 = 0;
  Curve *line2 = 0;
  key = sg->get("NAME_2")->getString();
  bool ok;
  value2 = key.toDouble(&ok);
  if (ok)
    valueFlag2++;
  else
  {
    s = script->setting(key)->getString();
    line2 = script->curve(s);
    if (! line2)
    {
      qDebug() << _type << "::runScript: invalid NAME_2" << s;
      return _ERROR;
    }
  }

  // get NAME_2_OFFSET
  int offset2 = sg->get("NAME_2_OFFSET")->getInteger();

  // verify NAME3
  key = sg->get("NAME_3")->getString();
  s = script->setting(key)->getString();
  Curve *line3 = script->curve(s);
  if (! line3)
  {
    qDebug() << _type << "::runScript: NAME_3 not found" << s;
    return _ERROR;
  }

  // get NAME_3_OFFSET
  int offset3 = sg->get("NAME_3_OFFSET")->getInteger();

  QColor color = sg->get("COLOR")->getColor();

  // find lowest and highest index values
  int high = 0;
  int low = 0;
  line->keyRange(low, high);

  if (! valueFlag2)
  {
    int tlow = 0;
    int thigh = 0;
    line2->keyRange(tlow, thigh);
    if (tlow < low)
      low = tlow;
    if (thigh > high)
      high = thigh;
  }

  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar = line->bar(loop - offset);
    if (! bar)
      continue;

    double v2 = 0;
    if (valueFlag2)
      v2 = value2;
    else
    {
      CurveBar *bar2 = line2->bar(loop - offset2);
      if (! bar2)
        continue;
      v2 = bar2->data();
    }

    CurveBar *bar3 = line3->bar(loop - offset3);
    if (! bar3)
      continue;

    if (top.test(bar->data(), op, v2))
      bar3->setColor(color);
  }

  return _OK;
}

SettingGroup * CommandColor::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("NAME");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 0, 999999, 0);
  si->setKey("NAME_OFFSET");
  sg->set(si);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("NAME_2");
  sg->set(ss);

  si = new SettingInteger(0, 0, 0, 999999, 0);
  si->setKey("NAME_2_OFFSET");
  sg->set(si);

  Operator op;
  SettingList *sl = new SettingList(Setting::_NONE, Setting::_NONE, op.list(), QString("EQ"));
  sl->setKey("OP");
  sg->set(sl);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("NAME_3");
  sg->set(ss);

  si = new SettingInteger(0, 0, 0, 999999, 0);
  si->setKey("NAME_3_OFFSET");
  sg->set(si);

  SettingColor *sc = new SettingColor(QColor(Qt::red));
  sc->setKey("COLOR");
  sg->set(sc);

  return sg;
}
