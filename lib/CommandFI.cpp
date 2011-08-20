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
 *  MERCHANTABILITY or CommandFITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "CommandFI.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingInteger.h"

#include <QtDebug>

CommandFI::CommandFI (QObject *p) : Command (p)
{
  _type = "FI";
}

int CommandFI::runScript (void *d)
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

  QString key = sg->get("CLOSE")->getString();
  QString s = script->setting(key)->getString();
  Curve *iclose = script->curve(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  key = sg->get("VOLUME")->getString();
  s = script->setting(key)->getString();
  Curve *ivol = script->curve(s);
  if (! ivol)
  {
    qDebug() << _type << "::runScript: invalid VOLUME" << s;
    return _ERROR;
  }

  QList<Curve *> list;
  list << iclose << ivol;
  line = getFI(list);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandFI::getFI (QList<Curve *> &list)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Curve *line = new Curve;
  Curve *close = list.at(0);
  Curve *vol = list.at(1);
  int loop = 1;
  double force = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *cbar = close->bar(keys.at(loop));
    if (! cbar)
      continue;

    CurveBar *ycbar = close->bar(keys.at(loop - 1));
    if (! ycbar)
      continue;

    CurveBar *vbar = vol->bar(keys.at(loop));
    if (! vbar)
      continue;

    double cdiff = cbar->data() - ycbar->data();
    force = vbar->data() * cdiff;

    line->setBar(keys.at(loop), new CurveBar(force));
  }

  return line;
}

SettingGroup * CommandFI::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("CLOSE");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("VOLUME");
  sg->set(ss);

  return sg;
}
