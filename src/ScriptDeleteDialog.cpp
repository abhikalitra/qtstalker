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

#include "ScriptDeleteDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QLabel>

ScriptDeleteDialog::ScriptDeleteDialog () : Dialog (Dialog::_Dialog, 0)
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("Delete Scripts"));

  createMainPage();
}

void ScriptDeleteDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  w->setLayout(vbox);

  // list
  QStringList l;
  _db.getScripts(l);
  
  _list = new QListWidget;
  _list->addItems(l);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _list->setSortingEnabled(TRUE);
  vbox->addWidget(_list);
  
  _tabs->addTab(w, tr("Script Name"));
}

void ScriptDeleteDialog::done ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (! sl.count())
  {
    reject();
    return;
  }

  Dialog *dialog = new Dialog(Dialog::_Message, 0);
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Delete Script"));
  dialog->setMessage(tr("Are you sure you want to delete selected scripts?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(done2()));
  dialog->show();
}

void ScriptDeleteDialog::done2 ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (! sl.count())
  {
    reject();
    return;
  }

  _db.transaction();

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    QString s = item->text();
    
    Script script;
    script.setName(s);
   _db.deleteScript(&script);
  }

  _db.commit();

  emit signalMessage(QString(tr("Scripts deleted.")));

  emit signalDelete();
  
  accept();
}

