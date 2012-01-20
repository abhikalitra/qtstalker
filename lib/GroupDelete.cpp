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

#include "GroupDelete.h"
#include "SelectDialog.h"
#include "WindowTitle.h"
#include "EAVDataBase.h"

#include <QtDebug>
#include <QTimer>

GroupDelete::GroupDelete (QObject *p) : QObject (p)
{
}

void GroupDelete::run ()
{
  QTimer::singleShot(0, this, SLOT(remove()));
}

void GroupDelete::remove ()
{
  QStringList names;
  EAVDataBase db("groups");
  db.names(names);

  WindowTitle wt;
  SelectDialog *dialog = new SelectDialog(0);
  dialog->setWindowTitle(wt.title(tr("Delete Group"), QString()));
  dialog->setItems(names);
  dialog->setTitle(tr("Groups"));
  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(remove2(QStringList)));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  dialog->show();
}

void GroupDelete::remove2 (QStringList gl)
{
  if (! gl.count())
  {
    done();
    return;
  }

  EAVDataBase db("groups");
  db.transaction();
  db.remove(gl);
  db.commit();

  emit signalDone();

  done();
}

void GroupDelete::done ()
{
  deleteLater();
}
