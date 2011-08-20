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

#include "CommandSINE.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"

#include <QtDebug>

CommandSINE::CommandSINE (QObject *p) : Command (p)
{
  _type = "SINE";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandSINE::CommandSINE: error on TA_Initialize");
}

int CommandSINE::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString sname = sg->get("NAME_SINE")->getString();
  Curve *line = script->curve(sname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate SINE" << sname;
    return _ERROR;
  }

  QString lname = sg->get("NAME_LEAD")->getString();
  line = script->curve(lname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_LEAD" << lname;
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

  QList<Curve *> list;
  list << in;
  QList<Curve *> lines = getSINE(list);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Curve *sline = lines.at(0);
  sline->setLabel(sname);
  script->setCurve(sname, sline);

  Curve *lline = lines.at(1);
  lline->setLabel(lname);
  script->setCurve(lname, lline);

  return _OK;
}

QList<Curve *> CommandSINE::getSINE (QList<Curve *> &list)
{
  QList<Curve *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input[0], &input[0], &input[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_HT_SINE (0,
                              size - 1,
                              &input[0],
                              &outBeg,
                              &outNb,
                              &out[0],
                              &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSINE: TA-Lib error" << rc;
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

SettingGroup * CommandSINE::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_SINE");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_LEAD");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  return sg;
}
