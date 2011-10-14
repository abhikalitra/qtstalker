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
#include "CurveData.h"
#include "CurveBar.h"
#include "DataString.h"
#include "DataDouble.h"
#include "DataDateTime.h"
#include "VerifyDataInput.h"
#include "GlobalSymbol.h"

#include <QtDebug>
#include <QMutexLocker>

CommandSymbolCurrent::CommandSymbolCurrent (QObject *p) : Command (p)
{
  _name = "SYMBOL_CURRENT";
//  _type = _NORMAL;
}

int CommandSymbolCurrent::runScript (Message *sg, Script *script)
{
  QMutexLocker locker(&g_symbolMutex);

  Symbol *bd = script->symbol();
  if (! bd)
  {
    qDebug() << "CommandSymbolCurrent::runScript: invalid symbol";
    emit signalResume((void *) this);
    return _ERROR;
  }

  // date
  VerifyDataInput vdi;
  QString name;
  QString s = sg->value("DATE");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSymbolCurrent::runScript: invalid DATE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *dline = new CurveData;
  script->setData(name, dline);

  // open
  s = sg->value("OPEN");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSymbolCurrent::runScript: invalid OPEN" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *oline = new CurveData;
  script->setData(name, oline);

  // high
  s = sg->value("HIGH");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSymbolCurrent::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *hline = new CurveData;
  script->setData(name, hline);

  // low
  s = sg->value("LOW");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSymbolCurrent::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *lline = new CurveData;
  script->setData(name, lline);

  // close
  s = sg->value("CLOSE");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSymbolCurrent::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *cline = new CurveData;
  script->setData(name, cline);

  // volume
  s = sg->value("VOLUME");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSymbolCurrent::runScript: invalid VOLUME" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *vline = new CurveData;
  script->setData(name, vline);

  // oi
  s = sg->value("OI");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSymbolCurrent::runScript: invalid OI" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *iline = new CurveData;
  script->setData(name, iline);

  int loop = 0;
  QList<int> keys = bd->barKeys();
  for (; loop < keys.count(); loop++)
  {
    Data *b = bd->getData(keys.at(loop));

    Data *db = new CurveBar;
    db->set(CurveBar::_DATE, new DataDateTime(b->toData(CurveBar::_DATE)->toDateTime()));
    dline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_OPEN)->toDouble()));
    oline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_HIGH)->toDouble()));
    hline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_LOW)->toDouble()));
    lline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_CLOSE)->toDouble()));
    cline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_VOLUME)->toDouble()));
    vline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_OI)->toDouble()));
    iline->set(loop, db);
  }

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
