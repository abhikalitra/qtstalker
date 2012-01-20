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

#include "GroupNew.h"
#include "NewDialog.h"
#include "WindowTitle.h"
#include "EAVDataBase.h"
#include "GroupEditDialog.h"

#include <QtDebug>
#include <QTimer>
#include <QStringList>

GroupNew::GroupNew (QObject *p) : QObject (p)
{
}

void GroupNew::run ()
{
  QTimer::singleShot(0, this, SLOT(run2()));
}

void GroupNew::run2 ()
{
  EAVDataBase db("groups");
  QStringList l;
  db.names(l);

  NewDialog *dialog = new NewDialog(0);
  dialog->setItems(l);
  dialog->setTitle(tr("Enter new group name"));
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(run3(QString)));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));

  WindowTitle wt;
  dialog->setWindowTitle(wt.title(tr("New Group"), QString()));

  dialog->show();
}

void GroupNew::run3 (QString d)
{
  if (d.isEmpty())
  {
    done();
    return;
  }

  GroupEditDialog *dialog = new GroupEditDialog(0, d);
  connect(dialog, SIGNAL(accepted()), this, SLOT(run4()));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  dialog->show();
}

void GroupNew::run4 ()
{
  emit signalDone();
  done();
}

void GroupNew::done ()
{
  deleteLater();
}
