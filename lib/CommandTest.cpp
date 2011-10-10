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
#include "SettingFactory.h"
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
  QString s = sg->value("NAME");
  Setting *set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    qDebug() << "CommandTest::runScript: invalid NAME" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setName(set->toString());

  // SYMBOL
  s = sg->value("SYMBOL");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    qDebug() << "CommandTest::runScript: invalid SYMBOL" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setSymbol(set->toString());

  // ENTER_LONG
  s = sg->value("ENTER_LONG");
  Data *d = vdi.curveAll(script, s);
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
  d = vdi.curveAll(script, s);
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
  d = vdi.curveAll(script, s);
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
  d = vdi.curveAll(script, s);
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
  d = vdi.curve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid DATE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setDate(d);

  // OPEN
  s = sg->value("OPEN");
  d = vdi.curve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid OPEN" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setOpen(d);

  // HIGH
  s = sg->value("HIGH");
  d = vdi.curve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setHigh(d);

  // LOW
  s = sg->value("LOW");
  d = vdi.curve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setLow(d);

  // CLOSE
  s = sg->value("CLOSE");
  d = vdi.curve(script, s);
  if (! d)
  {
    qDebug() << "CommandTest::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setClose(d);

  // ENTER_COMM
  s = sg->value("ENTER_COMM");
  set = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! set)
  {
    qDebug() << "CommandTest::runScript: invalid ENTER_COMM" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setEnterComm(set->toDouble());

  // EXIT_COMM
  s = sg->value("EXIT_COMM");
  set = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! set)
  {
    qDebug() << "CommandTest::runScript: invalid EXIT_COMM" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setExitComm(set->toDouble());

  // EQUITY
  s = sg->value("EQUITY");
  set = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! set)
  {
    qDebug() << "CommandTest::runScript: invalid EQUITY" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setEquity(set->toDouble());

  // MAX_LOSS_STOP
  s = sg->value("MAX_LOSS_STOP");
  set = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! set)
  {
    qDebug() << "CommandTest::runScript: invalid MAX_LOSS_STOP" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setMaxLossStop(set->toDouble());

  // TRAILING_STOP
  s = sg->value("TRAILING_STOP");
  set = vdi.setting(SettingFactory::_BOOL, script, s);
  if (! set)
  {
    qDebug() << "CommandTest::runScript: invalid TRAILING_STOP" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setTrailingStop(set->toBool());

  // VOLUME
  s = sg->value("VOLUME");
  set = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! set)
  {
    qDebug() << "CommandTest::runScript: invalid VOLUME" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  test.setVolume(set->toDouble());

  // run test
  if (! test.run())
    _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
