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

#include "CommandCompare.h"
#include "Operator.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingList.h"

#include <QtDebug>

CommandCompare::CommandCompare (QObject *p) : Command (p)
{
  _type = "COMPARE";
}

int CommandCompare::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  // NAME
  QString name = sg->get("NAME")->getString();
  Curve *line = script->curve(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate name" << name;
    return _ERROR;
  }

  // INPUT
  QString key = sg->get("INPUT")->getString();
  QString s = script->setting(key)->getString();
  Curve *in = script->curve(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT not found" << s;
    return _ERROR;
  }

  int offset = sg->get("OFFSET")->getInteger();

  // verify OP
  Operator top;
  s = sg->get("OP")->getString();
  Operator::Type op = top.stringToOperator(s);
  if (op == -1)
  {
    qDebug() << _type << "::runScript: invalid OP" << s;
    return _ERROR;
  }

  // INPUT_2
  int valueFlag2 = FALSE;
  Curve *in2 = 0;
  key = sg->get("INPUT_2")->getString();
  bool ok;
  double value2 = key.toDouble(&ok);
  if (ok)
    valueFlag2++;
  else
  {
    s = script->setting(key)->getString();
    in2 = script->curve(s);
    if (! in2)
    {
      qDebug() << _type << "::runScript: INPUT_2 not found" << s;
      return _ERROR;
    }
  }

  int offset2 = sg->get("OFFSET_2")->getInteger();

  // INPUT_3
  int valueFlag3 = FALSE;
  Curve *in3 = 0;
  key = sg->get("INPUT_3")->getString();
  double value3 = key.toDouble(&ok);
  if (ok)
    valueFlag3++;
  else
  {
    s = script->setting(key)->getString();
    in3 = script->curve(s);
    if (! in3)
    {
      qDebug() << _type << "::runScript: INPUT_3 not found" << s;
      return _ERROR;
    }
  }

  int offset3 = sg->get("OFFSET_3")->getInteger();

  // INPUT_4
  int valueFlag4 = FALSE;
  Curve *in4 = 0;
  key = sg->get("INPUT_4")->getString();
  double value4 = key.toDouble(&ok);
  if (ok)
    valueFlag4++;
  else
  {
    s = script->setting(key)->getString();
    in4 = script->curve(s);
    if (! in4)
    {
      qDebug() << _type << "::runScript: INPUT_4 not found" << s;
      return _ERROR;
    }
  }

  int offset4 = sg->get("OFFSET_4")->getInteger();

  // find lowest and highest index values
  // INPUT
  int high = 0;
  int low = 0;
  in->keyRange(low, high);

  // INPUT_2
  if (! valueFlag2)
  {
    int tlow = 0;
    int thigh = 0;
    in2->keyRange(tlow, thigh);
    if (tlow < low)
      low = tlow;
    if (thigh > high)
      high = thigh;
  }

  // INPUT_2
  if (! valueFlag3)
  {
    int tlow = 0;
    int thigh = 0;
    in3->keyRange(tlow, thigh);
    if (tlow < low)
      low = tlow;
    if (thigh > high)
      high = thigh;
  }

  // INPUT_4
  if (! valueFlag4)
  {
    int tlow = 0;
    int thigh = 0;
    in4->keyRange(tlow, thigh);
    if (tlow < low)
      low = tlow;
    if (thigh > high)
      high = thigh;
  }

  line = new Curve;
  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar = in->bar(loop - offset);
    if (! bar)
      continue;

    double v2 = 0;
    if (valueFlag2)
      v2 = value2;
    else
    {
      CurveBar *bar2 = in2->bar(loop - offset2);
      if (! bar2)
        continue;
      v2 = bar2->data();
    }

    double v3 = 0;
    if (valueFlag3)
      v3 = value3;
    else
    {
      CurveBar *bar3 = in3->bar(loop - offset3);
      if (! bar3)
        continue;
      v3 = bar3->data();
    }

    double v4 = 0;
    if (valueFlag4)
      v4 = value4;
    else
    {
      CurveBar *bar4 = in4->bar(loop - offset4);
      if (! bar4)
        continue;
      v4 = bar4->data();
    }

    if (top.test(bar->data(), op, v2))
      line->setBar(loop, new CurveBar(v3));
    else
      line->setBar(loop, new CurveBar(v4));
  }

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

SettingGroup * CommandCompare::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString());
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 0, 9999, 0);
  si->setKey("OFFSET");
  sg->set(si);

  Operator op;
  SettingList *sl = new SettingList(op.list(), QString("EQ"));
  sl->setKey("OP");
  sg->set(sl);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT_2");
  sg->set(ss);

  si = new SettingInteger(0, 0, 0, 9999, 0);
  si->setKey("OFFSET_2");
  sg->set(si);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT_3");
  sg->set(ss);

  si = new SettingInteger(0, 0, 0, 9999, 0);
  si->setKey("OFFSET_3");
  sg->set(si);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT_4");
  sg->set(ss);

  si = new SettingInteger(0, 0, 0, 9999, 0);
  si->setKey("OFFSET_4");
  sg->set(si);

  return sg;
}
