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

#include "CommandADX.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandADX::CommandADX (QObject *p) : Command (p)
{
  _name = "ADX";
  _method << "ADX" << "ADXR" << "MDI" << "PDI";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandADX::CommandADX: error on TA_Initialize");
}

int CommandADX::runScript (Message *sg, Script *script)
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

  s = sg->value("HIGH");
  Data *ihigh = vdi.curve(script, s);
  if (! ihigh)
  {
    _message << "invalid HIGH " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("LOW");
  Data *ilow = vdi.curve(script, s);
  if (! ilow)
  {
    _message << "invalid LOW " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("CLOSE");
  Data *iclose = vdi.curve(script, s);
  if (! iclose)
  {
    _message << "invalid CLOSE " + s;
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

  QList<Data *> list;
  list << ihigh << ilow << iclose;

  Data *line = getADX(list, period->toInteger(), method);
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

Data * CommandADX::getADX (QList<Data *> &list, int period, int method)
{
  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill3(list, keys, &high[0], &low[0], &close[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;

  switch (method)
  {
    case 0: // ADX
      rc = TA_ADX(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1: // ADXR
      rc = TA_ADXR(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 2: // MDI
      rc = TA_MINUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3: // PDI
      rc = TA_PLUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getADX: TA-Lib error" << rc;
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
