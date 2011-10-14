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
#include "VerifyDataInput.h"

#include <QtDebug>

CommandMA::CommandMA (QObject *p) : Command (p)
{
  _name = "MA";
}

int CommandMA::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  MAType mat;

  // METHOD
  int method;
  QString s = sg->value("METHOD");
  if (vdi.toMA(script, s, method))
  {
    qDebug() << "CommandMA::runScript: invalid METHOD, using default" << s;
    method = MAType::_EMA;
  }

  // OUTPUT
  QString output;
  s = sg->value("OUTPUT");
  if (vdi.toString(script, s, output))
  {
    qDebug() << "CommandMA::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT
  s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandMA::runScript: INPUT missing " << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // PERIOD
  int period = 10;
  s = sg->value("PERIOD");
  if (vdi.toInteger(script, s, period))
  {
    qDebug() << "CommandMA::runScript: invalid PERIOD, using default " << s;
    period = 10;
  }

  Data *line = mat.getMA(in, period, method);
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(output, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
