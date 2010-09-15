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

#include "IndicatorDeleteDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QLabel>

IndicatorDeleteDialog::IndicatorDeleteDialog () : Dialog (Dialog::_Dialog, 0)
{
  setWindowTitle(tr("QtStalker: Delete Indicator"));

  createMainPage();
}

void IndicatorDeleteDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // name
  QLabel *label = new QLabel(tr("Indicator Name"));
  grid->addWidget(label, row++, col);

  // list
  QStringList l;
  _db.getActiveIndicatorList(l);
  
  _list = new QListWidget;
  _list->addItems(l);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _list->setSortingEnabled(TRUE);
  grid->addWidget(_list, row++, col);
  
  grid->setRowStretch(row, 1);

  _tabs->addTab(w, QString());
}

void IndicatorDeleteDialog::done ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (! sl.count())
  {
    reject();
    return;
  }

  Dialog *dialog = new Dialog(Dialog::_Message, 0);
  dialog->setWindowTitle(tr("Qtstalker: Delete Indicator"));
  dialog->setMessage(tr("Are you sure you want to delete selected indicators?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(done2()));
  dialog->show();
}

void IndicatorDeleteDialog::done2 ()
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
    
    Indicator i;
    i.setName(s);
    _db.getIndicator(i);

    emit signalDelete(i);

    _db.deleteIndicator(s);
  }

  _db.commit();
  g_mutex.unlock();

  emit signalMessage(QString(tr("Indicators deleted.")));

  accept();
}

