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

#include "GroupAdd.h"
#include "SelectDialog.h"
#include "Global.h"
#include "EAVDataBase.h"
#include "GroupDataBaseKey.h"

#include <QtDebug>
#include <QSettings>
#include <QTimer>

GroupAdd::GroupAdd (QObject *p, QStringList symbols) : QObject (p)
{
  _symbols = symbols;
}

void GroupAdd::run ()
{
  QTimer::singleShot(0, this, SLOT(add()));
}

void GroupAdd::add ()
{
  if (! _symbols.count())
  {
    done();
    return;
  }

  QStringList l;
  EAVDataBase db("groups");
  db.names(l);

  SelectDialog *dialog = new SelectDialog(0);
  dialog->setItems(l);
  dialog->setTitle(tr("Groups"));
  dialog->setMode(1);

  l.clear();
  l << "QtStalker" + g_session + ":" << tr("Add To Group");
  dialog->setWindowTitle(l.join(" "));

  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(add2(QStringList)));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  dialog->show();
}

void GroupAdd::add2 (QStringList gl)
{
  if (! gl.count())
  {
    done();
    return;
  }

  Entity g;
  g.setName(gl.at(0));
  
  EAVDataBase db("groups");
  if (db.get(g))
  {
    done();
    return;
  }

  GroupDataBaseKey gkeys;
  Data td;
  g.toData(gkeys.indexToString(GroupDataBaseKey::_LIST), td);
  QStringList l = td.toList();

  int loop = 0;
  for (; loop < _symbols.size(); loop++)
    l << _symbols.at(loop);

  l.removeDuplicates();

  Data dl;
  dl.set(l);
  g.set(gkeys.indexToString(GroupDataBaseKey::_LIST), dl);

  db.transaction();
  if (db.set(g))
  {
    done();
    return;
  }
  db.commit();

  emit signalDone();

  done();
}

void GroupAdd::done ()
{
  deleteLater();
}
