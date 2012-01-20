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

#include "CommandVIDYA.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"

#include <QtDebug>
#include <cmath>
#include <QVector>

#define PI 3.14159265

CommandVIDYA::CommandVIDYA ()
{
  _name = "VIDYA";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(14);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);
  
  td = Data(10);
  td.setLabel(QObject::tr("Period V"));
  Entity::set(QString("PERIODV"), td);

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandVIDYA::CommandVIDYA: error on TA_Initialize");
}

QString CommandVIDYA::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandVIDYA::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Data p, pv;
  Entity::toData(QString("PERIOD"), p);
  Entity::toData(QString("PERIODV"), pv);
  
  CurveData line;
  if (getVIDYA(in, p.toInteger(), pv.toInteger(), line))
  {
    qDebug() << "CommandVIDYA::run: getVIDYA error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandVIDYA::getVIDYA (Entity &in, int period, int vperiod, Entity &out)
{
  Entity cmoParms;
  cmoParms.set(QString("FUNCTION"), Data(TypeTALibFunction::_CMO));
  cmoParms.set(QString("PERIOD"), vperiod);
  
  CurveBar cmo;
  TALibFunction func;
  if (func.run(cmoParms, 1, in, in, in, in, 1, cmo, cmo, cmo))
  {
    qDebug() << "CommandVIDYA::getVIDYA: TALibFunction error for CMO";
    return 1;
  }
  
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys1(in, keys))
    return 1;

  int size = keys.size();

  QVector<double> inSeries(size);
  inSeries.fill(0.0);
  QVector<double> offset(size);
  offset.fill(0.0);
  QVector<double> absCmo(size);
  absCmo.fill(0.0);
  QVector<double> vidya(size);
  vidya.fill(0.0);

  double c = 2 / (double) period + 1;

  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity bar;
    in.toEntity(keys.at(loop), bar);
    Data tv;
    if (bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tv))
      continue;
    inSeries[loop] = tv.toDouble();
  }

  keys = cmo.ekeys();

  int index = inSeries.size() -1;
  loop = keys.size() - 1;
  for (; loop > -1; loop--)
  {
    Entity bar;
    cmo.toEntity(keys.at(loop), bar);
    Data tv;
    if (bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tv))
      continue;
    absCmo[index] = fabs(tv.toDouble() / 100);
    index--;
  }

  loop = vperiod + period;
  for (; loop < (int) inSeries.size(); loop++)         // period safety
  {
    vidya[loop] = (inSeries.at(loop) * c * absCmo.at(loop)) + ((1 - absCmo.at(loop) * c) * vidya.at(loop - 1));

    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(vidya.at(loop)));
    out.setEntity(QString::number(loop), b);
  }

  return 0;
}
