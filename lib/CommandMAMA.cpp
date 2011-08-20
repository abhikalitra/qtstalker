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

#include "CommandMAMA.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingDouble.h"

#include <QtDebug>

CommandMAMA::CommandMAMA (QObject *p) : Command (p)
{
  _type = "MAMA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMAMA::CommandMAMA: error on TA_Initialize");
}

int CommandMAMA::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString mname = sg->get("NAME_MAMA")->getString();
  Curve *line = script->curve(mname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_MAMA" << mname;
    return _ERROR;
  }

  QString fname = sg->get("NAME_FAMA")->getString();
  line = script->curve(fname);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME_FAMA" << fname;
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

  double flimit = sg->get("LIMIT_FAST")->getDouble();

  double slimit = sg->get("LIMIT_SLOW")->getDouble();

  QList<Curve *> list;
  list << in;
  QList<Curve *> lines = getMAMA(list, flimit, slimit);
  if (lines.count() != 2)
  {
    qDeleteAll(lines);
    return _ERROR;
  }

  Curve *mama = lines.at(0);
  mama->setLabel(mname);
  script->setCurve(mname, mama);

  Curve *fama = lines.at(1);
  fama->setLabel(fname);
  script->setCurve(fname, fama);

  return _OK;
}

QList<Curve *> CommandMAMA::getMAMA (QList<Curve *> &list, double flimit, double slimit)
{
  QList<Curve *> lines;
  if (! list.count())
    return lines;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real in[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &in[0], &in[0], &in[0], &in[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &in[0],
                          flimit,
                          slimit,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMAMA: TA-Lib error" << rc;
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

SettingGroup * CommandMAMA::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_MAMA");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME_FAMA");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  SettingDouble *sd = new SettingDouble(0, 0, 0.5, 1, 0);
  sd->setKey("LIMIT_FAST");
  sg->set(sd);

  sd = new SettingDouble(0, 0, 0.05, 1, 0);
  sd->setKey("LIMIT_SLOW");
  sg->set(sd);

  return sg;
}
