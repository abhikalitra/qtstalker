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

#include "CommandTest.h"
#include "CurveData.h"
#include "VerifyDataInput.h"
#include "Test.h"

#include <QtDebug>

CommandTest::CommandTest (QObject *p) : Command (p)
{
  _name = "TEST";
}

int CommandTest::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  Test test(this);

  // NAME
  QString ts;
  QString s = sg->value("NAME");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandTest::runScript: invalid NAME" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setName(ts);

  // SYMBOL
  s = sg->value("SYMBOL");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandTest::runScript: invalid SYMBOL" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setSymbol(ts);

  // ENTER_LONG
  s = sg->value("ENTER_LONG");
  Data *d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid ENTER_LONG" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
//  int eloffset = el->offset();
  test.setEnterLong(d);

  // EXIT_LONG
  s = sg->value("EXIT_LONG");
  d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid EXIT_LONG" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
//  int xloffset = xl->offset();
  test.setExitLong(d);

  // ENTER_SHORT
  s = sg->value("ENTER_SHORT");
  d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid ENTER_SHORT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
//  int esoffset = es->offset();
  test.setEnterShort(d);

  // EXIT_SHORT
  s = sg->value("EXIT_SHORT");
  d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid EXIT_SHORT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
//  int xsoffset = xs->offset();
  test.setExitShort(d);

  // DATE
  s = sg->value("DATE");
  d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid DATE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setDate(d);

  // OPEN
  s = sg->value("OPEN");
  d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid OPEN" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setOpen(d);

  // HIGH
  s = sg->value("HIGH");
  d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setHigh(d);

  // LOW
  s = sg->value("LOW");
  d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setLow(d);

  // CLOSE
  s = sg->value("CLOSE");
  d = vdi.toCurve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setClose(d);

  // ENTER_COMM
  double td = 0;
  s = sg->value("ENTER_COMM");
  if (vdi.toDouble(script, s, td))
  {
    qDebug() << "CommandTest::runScript: invalid ENTER_COMM" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setEnterComm(td);

  // EXIT_COMM
  s = sg->value("EXIT_COMM");
  if (vdi.toDouble(script, s, td))
  {
    qDebug() << "CommandTest::runScript: invalid EXIT_COMM" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setExitComm(td);

  // EQUITY
  s = sg->value("EQUITY");
  if (vdi.toDouble(script, s, td))
  {
    qDebug() << "CommandTest::runScript: invalid EQUITY" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setEquity(td);

  // MAX_LOSS_STOP
  s = sg->value("MAX_LOSS_STOP");
  if (vdi.toDouble(script, s, td))
  {
    qDebug() << "CommandTest::runScript: invalid MAX_LOSS_STOP" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setMaxLossStop(td);

  // TRAILING_STOP
  bool tb = FALSE;
  s = sg->value("TRAILING_STOP");
  if (vdi.toBool(script, s, tb))
  {
    qDebug() << "CommandTest::runScript: invalid TRAILING_STOP" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setTrailingStop(tb);

  // VOLUME
  s = sg->value("VOLUME");
  if (vdi.toDouble(script, s, td))
  {
    qDebug() << "CommandTest::runScript: invalid VOLUME" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setVolume(td);

  // run test
  if (! test.run())
    _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
