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

#include "CommandPHASOR.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"

#include <QtDebug>

CommandPHASOR::CommandPHASOR (QObject *p) : Command (p)
{
  _type = "PHASOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandPHASOR::CommandPHASOR: error on TA_Initialize");
}

int CommandPHASOR::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString pname = sg->get("NAME_PHASE")->getString();
  Curve *line = script->curve(pname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate PHASE" << pname;
    return _ERROR;
  }

  QString qname = sg->get("NAME_QUAD")->getString();
  line = script->curve(qname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_QUAD" << qname;
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
  QList<Curve *> lines = getPHASOR(list);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Curve *pline = lines.at(0);
  pline->setLabel(pname);
  script->setCurve(pname, pline);

  Curve *qline = lines.at(1);
  qline->setLabel(qname);
  script->setCurve(qname, qline);

  return _OK;
}

QList<Curve *> CommandPHASOR::getPHASOR (QList<Curve *> &list)
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

  TA_RetCode rc = TA_HT_PHASOR (0,
                                size - 1,
                                &input[0],
                                &outBeg,
                                &outNb,
                                &out[0],
                                &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getPHASOR: TA-Lib error" << rc;
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

SettingGroup * CommandPHASOR::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_PHASE");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_QUAD");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  return sg;
}
