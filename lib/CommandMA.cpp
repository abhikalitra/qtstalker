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

#include "CommandMA.h"
#include "MAType.h"
#include "CurveData.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"

#include <QtDebug>

CommandMA::CommandMA (QObject *p) : Command (p)
{
  _name = "MA";
}

int CommandMA::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  MAType mat;

  QString s = sg->value("METHOD");
  Setting *method = vdi.setting(SettingFactory::_MA, script, s);
  if (! method)
  {
    qDebug() << "CommandMA::runScript: invalid METHOD" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("OUTPUT");
  Setting *output = vdi.setting(SettingFactory::_STRING, script, s);
  if (! output)
  {
    qDebug() << "CommandMA::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("INPUT");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    qDebug() << "CommandMA::runScript: INPUT missing " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // PERIOD
  s = sg->value("PERIOD");
  Setting *period = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! period)
  {
    qDebug() << "CommandMA::runScript: invalid PERIOD " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  Data *line = mat.getMA(in, period->toInteger(), method->toInteger());
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(output->toString(), line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
