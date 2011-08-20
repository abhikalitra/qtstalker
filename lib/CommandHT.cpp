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

#include "CommandHT.h"
#include "ta_libc.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingList.h"

#include <QtDebug>

CommandHT::CommandHT (QObject *p) : Command (p)
{
  _type = "HT";
  _method << "DCPERIOD" << "DCPHASE" << "TRENDLINE" << "TRENDMODE";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandHT::CommandHT: error on TA_Initialize");
}

int CommandHT::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString name = sg->get("NAME")->getString();
  Curve *line = script->curve(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate NAME" << name;
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

  s = sg->get("METHOD")->getString();
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << _type << "::runScript: invalid METHOD" << s;
    return _ERROR;
  }

  QList<Curve *> list;
  list << in;
  line = getHT(list, method);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandHT::getHT (QList<Curve *> &list, int method)
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
  switch ((Method) method)
  {
    case _DCPERIOD:
      rc = TA_HT_DCPERIOD (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case _DCPHASE:
      rc = TA_HT_DCPHASE (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case _TRENDLINE:
      rc = TA_HT_TRENDLINE (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case _TRENDMODE:
    {
      TA_Integer iout[size];
      rc = TA_HT_TRENDMODE (0, size - 1, &input[0], &outBeg, &outNb, &iout[0]);
      if (rc != TA_SUCCESS)
      {
        qDebug() << _type << "::getHT: TA-Lib error" << rc;
        return 0;
      }

      Curve *line = new Curve;
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        line->setBar(keys.at(keyLoop), new CurveBar((double) iout[outLoop]));
        keyLoop--;
        outLoop--;
      }
      return line;
      break;
    }
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getHT: TA-Lib error" << rc;
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

SettingGroup * CommandHT::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("INPUT");
  sg->set(ss);

  SettingList *sl = new SettingList(_method, _method.at(0));
  sl->setKey("METHOD");
  sg->set(sl);

  return sg;
}
