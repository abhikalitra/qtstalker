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

#include "GroupAddDialog.h"
#include "GroupNewDialog.h"
#include "SymbolDialog.h"
#include "Globals.h"
#include "Config.h"
#include "GroupDataBase.h"

#include "../pics/newchart.xpm"

#include <QtDebug>
#include <QFormLayout>
#include <QString>
#include <QToolButton>
#include <QIcon>
#include <QPushButton>

GroupAddDialog::GroupAddDialog (Group group)
{
  _group = group;
  _configSizeParm = Config::GroupAddDialogSize;
  _configPosParm = Config::GroupAddDialogPosition;
  _helpFile = "main.html";
  
  setWindowTitle("QtStalker" + g_session + ": " + tr("Add To Group"));

  createMainPage();

  loadSettings();

  selectionChanged();
}

void GroupAddDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(10);
  w->setLayout(vbox);
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(0);
  vbox->addLayout(form);

  // groups
  QStringList gl;
  GroupDataBase db;
  db.getAllGroupsList(gl);
  
  _groups = new QComboBox;
  _groups->addItems(gl);
  _groups->setToolTip(tr("Current groups"));
  form->addRow(tr("Groups"), _groups);

  // new group
  QPushButton *b = new QPushButton;
  b->setIcon(QIcon(newchart_xpm));
  b->setText("...");
  b->setToolTip(tr("Create a new group"));
  connect(b, SIGNAL(clicked()), this, SLOT(newGroup()));
  form->addRow(tr("New Group"), b);

  // list
  QStringList l;
  _group.getStringList(l);

  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _list->setSortingEnabled(TRUE);
  _list->addItems(l);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  vbox->addWidget(_list);

  _tabs->addTab(w, tr("Add To Group"));
}

void GroupAddDialog::done ()
{
  saveSettings();
  
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (! sl.count())
  {
    reject();
    return;
  }
  
  Group g;
  QString s = _groups->currentText();
  g.setName(s);
  
  GroupDataBase db;
  db.getGroup(g);

  int loop;
  for (loop = 0; loop < sl.count(); loop++)
  {
    BarData bd;
    _group.getSymbol(sl.at(loop)->text(), bd);
    g.setSymbol(bd);
  }
    
  db.transaction();
  db.setGroup(g);
  db.commit();

  emit signalMessage(QString(tr("Symbol(s) added.")));
  
  emit signalGroupChanged();

  accept();
}

void GroupAddDialog::newGroup ()
{
  GroupNewDialog *dialog = new GroupNewDialog;
  connect(dialog, SIGNAL(signalNew()), this, SLOT(updateGroups()));
  dialog->show();
}

void GroupAddDialog::updateGroups ()
{
  QStringList l;
  GroupDataBase db;
  db.getAllGroupsList(l);

  _groups->clear();
  _groups->addItems(l);
}

void GroupAddDialog::updateList (Group g)
{
  _group = g;

  QStringList l;
  _group.getStringList(l);

  _list->clear();
  _list->addItems(l);
}

void GroupAddDialog::selectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (sl.count())
    status = 1;

  QPushButton *b = _buttonBox->button(QDialogButtonBox::Ok);
  b->setEnabled(status);
}

