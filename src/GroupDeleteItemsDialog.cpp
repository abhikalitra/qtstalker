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

#include "GroupDeleteItemsDialog.h"
#include "Globals.h"

#include <QtDebug>

GroupDeleteItemsDialog::GroupDeleteItemsDialog (QString name) : Dialog (Dialog::_Dialog, 0)
{
  _group.setName(name);
  _db.getGroup(_group);
  
  setWindowTitle("QtStalker" + g_session + ": " + tr("Delete Group Items"));

  createMainPage();
}

void GroupDeleteItemsDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  w->setLayout(vbox);

  // list
  QStringList l;
  _group.getStringList(l);
  
  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _list->setSortingEnabled(TRUE);
  vbox->addWidget(_list);
  _list->addItems(l);
  
  _tabs->addTab(w, tr("Group Contents"));
}

void GroupDeleteItemsDialog::done ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (! sl.count())
  {
    reject();
    return;
  }

  Dialog *dialog = new Dialog(Dialog::_Message, 0);
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Delete Group Items"));
  dialog->setMessage(tr("Are you sure you want to delete selected items?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(done2()));
  dialog->show();
}

void GroupDeleteItemsDialog::done2 ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (! sl.count())
  {
    reject();
    return;
  }

  g_mutex.lock();
  _db.transaction();
  
  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    QString s = item->text();
    _group.deleteSymbol(s);
  }

  _db.setGroup(_group);
  _db.commit();
  g_mutex.unlock();

  emit signalMessage(QString(tr("Group Item(s) deleted.")));

  emit signalDelete();
  
  accept();
}

