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
#include "Script.h"
#include "CurveData.h"

#include <QtDebug>

CommandMA::CommandMA (QObject *p) : Command (p)
{
  _type = "MA";
}

int CommandMA::runScript (Data *sg, Script *script)
{
  MAType mat;
  QString s = sg->get("METHOD").toString();
  int method = mat.fromString(s);
  if (method == -1)
  {
    qDebug() << _type << "::runScript: invalid METHOD" << s;
    return _ERROR;
  }

  QString name = sg->get("OUTPUT").toString();
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  s = sg->get("INPUT").toString();
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD").toInt();

  line = mat.getMA(in, period, method);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandMA::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QVariant(QString()));
  sg->set("INPUT", QVariant(QString("close")));
  sg->set("PERIOD", QVariant(10));
  sg->set("METHOD", QVariant(QString("EMA")));
  return sg;
}
