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

#include "CommandNewHighLow.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"

#include <QtDebug>

CommandNewHighLow::CommandNewHighLow (QObject *p) : Command (p)
{
  _name = "NEW_HIGH_LOW";
  _method << "HIGH" << "LOW";
}

int CommandNewHighLow::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // INPUT
  QString s = sg->value("INPUT");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandNewHighLow::runScript: invalid INPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // METHOD
  s = sg->value("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << "CommandNewHighLow::runScript: invalid METHOD, using default" << s;
    method = 0;
  }

  int flag = 0;
  int rc = getNewHighLow(in, method, flag);
  if (rc)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  _returnString = QString::number(flag);

  emit signalResume((void *) this);

  return _OK;
}

int CommandNewHighLow::getNewHighLow (Data *in, int method, int &flag)
{
  flag = 0;
  QList<int> keys = in->keys();
  if (! keys.count())
    return 1;

  int loop = 0;
  Data *bar = in->toData(keys.at(loop++));
  double v = bar->toData(CurveBar::_VALUE)->toDouble();

  if (method == 0) // highest
  {
    for (; loop < keys.count() - 2; loop++)
    {
      bar = in->toData(keys.at(loop));
      double tv = bar->toData(CurveBar::_VALUE)->toDouble();
      if (tv > v)
        v = tv;
    }
  }
  else // lowest
  {
    for (; loop < keys.count() - 2; loop++)
    {
      bar = in->toData(keys.at(loop));
      double tv = bar->toData(CurveBar::_VALUE)->toDouble();
      if (tv < v)
        v = tv;
    }
  }

  bar = in->toData(keys.at(keys.count() - 1));
  if (method == 0)
  {
    if (bar->toData(CurveBar::_VALUE)->toDouble() > v)
      flag = 1;
  }
  else
  {
    if (bar->toData(CurveBar::_VALUE)->toDouble() < v)
      flag = 1;
  }

  return 0;
}
