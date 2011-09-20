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

#include "CommandSymbolCurrent.h"
#include "Strip.h"
#include "SharedMemory.h"
#include "Symbol.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "SettingString.h"
#include "SettingDouble.h"
#include "SettingDateTime.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"

#include <QtDebug>
#include <QSharedMemory>
#include <QSettings>

CommandSymbolCurrent::CommandSymbolCurrent (QObject *p) : Command (p)
{
  _type = "SYMBOL_CURRENT";
}

int CommandSymbolCurrent::runScript (Message *sg, Script *script)
{
  QSettings settings("QtStalker/qtstalkerrc");
  QSharedMemory sm(settings.value("shared_memory_key").toString());

  SharedMemory smd;
  QString s;
  if (smd.data(sm, s))
  {
    _message << "shared memory error";
    return _ERROR;
  }

  Data *bd = new Symbol;
  if (bd->fromString(s))
  {
    _message << "Data::fromString error " + s;
    delete bd;
    return _ERROR;
  }

  // dump to dialog
//  message(script->session(), s);

  // date
  VerifyDataInput vdi;
  s = sg->value("DATE");
  Setting *set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid DATE " + s;
    return _ERROR;
  }
  Data *dline = new CurveData;
  script->setData(set->toString(), dline);

  // open
  s = sg->value("OPEN");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid OPEN " + s;
    return _ERROR;
  }
  Data *oline = new CurveData;
  script->setData(set->toString(), oline);

  // high
  s = sg->value("HIGH");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid HIGH " + s;
    return _ERROR;
  }
  Data *hline = new CurveData;
  script->setData(set->toString(), hline);

  // low
  s = sg->value("LOW");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid LOW " + s;
    return _ERROR;
  }
  Data *lline = new CurveData;
  script->setData(set->toString(), lline);

  // close
  s = sg->value("CLOSE");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid CLOSE " + s;
    return _ERROR;
  }
  Data *cline = new CurveData;
  script->setData(set->toString(), cline);

  // volume
  s = sg->value("VOLUME");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid VOLUME " + s;
    return _ERROR;
  }
  Data *vline = new CurveData;
  script->setData(set->toString(), vline);

  // oi
  s = sg->value("OI");
  set = vdi.setting(SettingFactory::_STRING, script, s);
  if (! set)
  {
    _message << "invalid OI " + s;
    return _ERROR;
  }
  Data *iline = new CurveData;
  script->setData(set->toString(), iline);

  int loop = 0;
  QList<int> barKeys = bd->barKeys();
  for (; loop < barKeys.count(); loop++)
  {
    Data *b = bd->getData(barKeys.at(loop));
//qDebug() << b->get(CurveBar::_DATE)->toDateTime();

    Data *db = new CurveBar;
    db->set(CurveBar::_DATE, new SettingDateTime(b->get(CurveBar::_DATE)->toDateTime()));
    dline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_OPEN)->toDouble()));
    oline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_HIGH)->toDouble()));
    hline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_LOW)->toDouble()));
    lline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_CLOSE)->toDouble()));
    cline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_VOLUME)->toDouble()));
    vline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new SettingDouble(b->get(CurveBar::_OI)->toDouble()));
    iline->set(loop, db);
  }

  delete bd;

  return _OK;
}
