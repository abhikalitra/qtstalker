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

#include "CommandArithmetic.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingList.h"

#include <QtDebug>

CommandArithmetic::CommandArithmetic (QObject *p) : Command (p)
{
  _type = "ARITHMETIC";
  _method << "ADD" << "DIV" << "MULT" << "SUB";
}

int CommandArithmetic::runScript (void *d)
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

  // verify INPUT
  QString key = sg->get("INPUT")->getString();
  QString s = script->setting(key)->getString();
  Curve *in = script->curve(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT not found" << s;
    return _ERROR;
  }

  // verify INPUT2
  Curve *in2 = 0;
  key = sg->get("INPUT_2")->getString();
  bool ok;
  double value = key.toDouble(&ok);
  if (ok)
  {
    in2 = new Curve;
    QList<int> keys;
    in->keys(keys);
    int loop2 = 0;
    for (; loop2 < keys.count(); loop2++)
      in2->setBar(keys.at(loop2), new CurveBar(value));
    in2->setLabel(QString::number(value));
    script->setCurve(QString::number(value), in2);
  }
  else
  {
    s = script->setting(key)->getString();
    in2 = script->curve(s);
    if (! in2)
    {
      qDebug() << _type << "::runScript: invalid INPUT_2" << s;
      return _ERROR;
    }
  }

  s = sg->get("METHOD")->getString();
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << _type << "::runScript: invalid METHOD" << s;
    return _ERROR;
  }

  line = getArithmetic(in, in2, method);
  if (! line)
  {
    qDebug() << _type << "::runScript: getArithmetic error" << name;
    return _ERROR;
  }

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandArithmetic::getArithmetic (Curve *in, Curve *in2, int method)
{
  QList<int> keys;
  int size = in->count();
  in->keys(keys);

  if (in2->count() < size)
  {
    size = in2->count();
    in2->keys(keys);
  }

  Curve *line = new Curve;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    switch (method)
    {
      case 0: // add
        line->setBar(keys.at(loop), new CurveBar(bar->data() + bar2->data()));
	break;
      case 1: // div
        line->setBar(keys.at(loop), new CurveBar(bar->data() / bar2->data()));
	break;
      case 2: // mult
        line->setBar(keys.at(loop), new CurveBar(bar->data() * bar2->data()));
	break;
      case 3: // sub
        line->setBar(keys.at(loop), new CurveBar(bar->data() - bar2->data()));
	break;
      default:
	break;
    }
  }

  return line;
}

SettingGroup * CommandArithmetic::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString());
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT_2");
  sg->set(ss);

  SettingList *sl = new SettingList(_method, QString("ADD"));
  sl->setKey("METHOD");
  sg->set(sl);

  return sg;
}
