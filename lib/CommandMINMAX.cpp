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

#include "CommandMINMAX.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandMINMAX::CommandMINMAX (QObject *p) : Command (p)
{
  _name = "MINMAX";
  _method << "MIN" << "MAX";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandMINMAX::CommandMINMAX: error on TA_Initialize");
}

int CommandMINMAX::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *name = vdi.setting(SettingFactory::_STRING, script, s);
  if (! name)
  {
    _message << "invalid OUTPUT " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("INPUT");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    _message << "INPUT missing " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("PERIOD");
  Setting *period = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! period)
  {
    _message << "invalid PERIOD " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    _message << "invalid METHOD " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  Data *line = getMINMAX(in, period->toInteger(), method);
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name->toString(), line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandMINMAX::getMINMAX (Data *in, int tperiod, int method)
{
  QList<int> keys = in->barKeys();

  int period = tperiod;
  if (period == 0)
    period = keys.count();

  int size = keys.count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<Data *> list;
  list << in;

  TALibInput tai;
  size = tai.fill1(list, keys, &input[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;

  switch (method)
  {
    case 0: // MIN
      rc = TA_MIN(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1: // MAX
      rc = TA_MAX(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      return 0;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getMINMAX: TA-Lib error" << rc;
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
