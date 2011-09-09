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

#include <QtDebug>
#include <QSharedMemory>
#include <QSettings>

CommandSymbolCurrent::CommandSymbolCurrent (QObject *p) : Command (p)
{
  _type = "SYMBOL_CURRENT";
}

int CommandSymbolCurrent::runScript (Data *sg, Script *script)
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

  // dump to disk
//  message(script->session(), s);

  // date
  s = sg->get("DATE").toString();
  Data *dline = new CurveData;
  script->setData(s, dline);

  // open
  s = sg->get("OPEN").toString();
  Data *oline = new CurveData;
  script->setData(s, oline);

  // high
  s = sg->get("HIGH").toString();
  Data *hline = new CurveData;
  script->setData(s, hline);

  // low
  s = sg->get("LOW").toString();
  Data *lline = new CurveData;
  script->setData(s, lline);

  // close
  s = sg->get("CLOSE").toString();
  Data *cline = new CurveData;
  script->setData(s, cline);

  // volume
  s = sg->get("VOLUME").toString();
  Data *vline = new CurveData;
  script->setData(s, vline);

  // oi
  s = sg->get("OI").toString();
  Data *iline = new CurveData;
  script->setData(s, iline);

  int loop = 0;
  QList<int> barKeys = bd->barKeys();
  for (; loop < barKeys.count(); loop++)
  {
    Data *b = bd->getData(barKeys.at(loop));

    Data *db = new CurveBar;
    db->set(CurveBar::_DATE, b->get(CurveBar::_DATE));
    dline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, b->get(CurveBar::_OPEN));
    oline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, b->get(CurveBar::_HIGH));
    hline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, b->get(CurveBar::_LOW));
    lline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, b->get(CurveBar::_CLOSE));
    cline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, b->get(CurveBar::_VOLUME));
    vline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, b->get(CurveBar::_OI));
    iline->set(loop, db);
  }

  delete bd;

  return _OK;
}

Data * CommandSymbolCurrent::settings ()
{
  Data *sg = new Data;
  sg->set("DATE", QVariant(QString("date")));
  sg->set("OPEN", QVariant(QString("open")));
  sg->set("HIGH", QVariant(QString("high")));
  sg->set("LOW", QVariant(QString("low")));
  sg->set("CLOSE", QVariant(QString("close")));
  sg->set("VOLUME", QVariant(QString("volume")));
  sg->set("OI", QVariant(QString("oi")));
  return sg;
}
