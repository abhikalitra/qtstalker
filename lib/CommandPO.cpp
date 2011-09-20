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

#include "CommandPO.h"
#include "ta_libc.h"
#include "MAType.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandPO::CommandPO (QObject *p) : Command (p)
{
  _type = "PO";
  _method << "APO" << "PPO";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandPO::CommandPO: error on TA_Initialize");
}

int CommandPO::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT";
    return _ERROR;
  }
  Setting *name = vdi.setting(SettingFactory::_STRING, script, s);
  if (! name)
  {
    _message << "invalid OUTPUT " + s;
    return _ERROR;
  }

  s = sg->value("INPUT");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    _message << "INPUT missing " + s;
    return _ERROR;
  }

  s = sg->value("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    _message << "invalid METHOD " + s;
    return _ERROR;
  }

  s = sg->value("PERIOD_FAST");
  Setting *fast = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! fast)
  {
    _message << "invalid PERIOD_FAST " + s;
    return _ERROR;
  }

  s = sg->value("PERIOD_SLOW");
  Setting *slow = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! slow)
  {
    _message << "invalid PERIOD_SLOW " + s;
    return _ERROR;
  }

  MAType mat;
  s = sg->value("MA_TYPE");
  int type = mat.fromString(s);
  if (type == -1)
  {
    _message << "invalid MA_TYPE " + s;
    return _ERROR;
  }

  QList<Data *> list;
  list << in;

  Data *line = getPO(list, fast->toInteger(), slow->toInteger(), type, method);
  if (! line)
    return _ERROR;

  script->setData(name->toString(), line);

  return _OK;
}

Data * CommandPO::getPO (QList<Data *> &list, int fast, int slow, int type, int method)
{
  if (! list.count())
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real in[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill1(list, keys, &in[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;
  switch ((Method) method)
  {
    case _APO:
      rc = TA_APO(0, size - 1, &in[0], fast, slow, (TA_MAType) type, &outBeg, &outNb, &out[0]);
      break;
    case _PPO:
      rc = TA_PPO(0, size - 1, &in[0], fast, slow, (TA_MAType) type, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getPO: TA-Lib error" << rc;
    return 0;
  }

  QList<Data *> outs;
  Data *c = new CurveData;
  outs.append(c);

  TALibOutput tao;
  if (tao.fillDouble1(outs, keys, outNb, &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}
