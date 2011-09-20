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
  _type = "MA";
}

int CommandMA::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  MAType mat;
  QString s = sg->value("METHOD");
  int method = mat.fromString(s);
  if (method == -1)
  {
    _message << "invalid METHOD " + s;
    return _ERROR;
  }

  QString name = sg->value("OUTPUT");
  if (name.isEmpty())
  {
    _message << "invalid OUTPUT " + name;
    return _ERROR;
  }

  s = sg->value("INPUT");
  Data *in = vdi.curve(script, s);
  if (! in)
  {
    _message << "INPUT missing " + s;
    return _ERROR;
  }

  // PERIOD
  s = sg->value("PERIOD");
  Setting *period = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! period)
  {
    _message << "invalid PERIOD " + s;
    return _ERROR;
  }

  Data *line = mat.getMA(in, period->toInteger(), method);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}
