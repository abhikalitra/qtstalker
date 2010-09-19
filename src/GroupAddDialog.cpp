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
#include "Globals.h"
#include "GroupNewDialog.h"
#include "SymbolDialog.h"

#include "../pics/newchart.xpm"
#include "../pics/search.xpm"

#include <QtDebug>
#include <QLabel>
#include <QString>
#include <QToolButton>
#include <QIcon>
#include <QPushButton>

GroupAddDialog::GroupAddDialog (Group group) : Dialog (Dialog::_Dialog, 0)
{
  _group = group;
  
  setWindowTitle(tr("QtStalker: Add To Group"));

  createMainPage();
}

void GroupAddDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // name
  QLabel *label = new QLabel(tr("Groups"));
  grid->addWidget(label, row, col++);

  QStringList gl;
  _db.getAllGroupsList(gl);
  
  _groups = new QComboBox;
  _groups->addItems(gl);
  grid->addWidget(_groups, row, col++);

  QToolButton *_newButton = new QToolButton;
  _newButton->setIcon(QIcon(newchart_xpm));
  _newButton->setText("...");
  _newButton->setToolTip(tr("New Group"));
  connect(_newButton, SIGNAL(clicked()), this, SLOT(newGroup()));
  grid->addWidget(_newButton, row++, col--);

  // list
  QStringList l;
  _group.getStringList(l);

  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _list->setSortingEnabled(TRUE);
  vbox->addWidget(_list);
  _list->addItems(l);

  QPushButton *b = _buttonBox->addButton(tr("Search..."), QDialogButtonBox::ActionRole);
  b->setIcon(QIcon(search_xpm));
  b->setToolTip(tr("Search symbols"));
  connect(b, SIGNAL(clicked()), this, SLOT(search()));

  _tabs->addTab(w, QString());
}

void GroupAddDialog::done ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (! sl.count())
  {
    reject();
    return;
  }
  
  Group g;
  QString s = _groups->currentText();
  g.setName(s);
  _db.getGroup(g);

  int loop;
  for (loop = 0; loop < sl.count(); loop++)
  {
    BarData bd;
    _group.getSymbol(sl.at(loop)->text(), bd);
    g.setSymbol(bd);
  }
    
  g_mutex.lock();
  _db.transaction();
  _db.setGroup(g);
  _db.commit();
  g_mutex.unlock();

  emit signalMessage(QString(tr("Symbol(s) added.")));
  
  emit signalGroupChanged();

  accept();
}

void GroupAddDialog::newGroup ()
{
  GroupNewDialog *dialog = new GroupNewDialog;
  connect(dialog, SIGNAL(signalNewGroup()), this, SLOT(updateGroups()));
  dialog->show();
}

void GroupAddDialog::updateGroups ()
{
  QStringList l;
  _db.getAllGroupsList(l);

  _groups->clear();
  _groups->addItems(l);
}

void GroupAddDialog::search ()
{
  SymbolDialog *dialog = new SymbolDialog;
  connect(dialog, SIGNAL(signalSymbols(Group)), this, SLOT(updateList(Group)));
  dialog->show();
}

void GroupAddDialog::updateList (Group g)
{
  _group = g;

  QStringList l;
  _group.getStringList(l);

  _list->clear();
  _list->addItems(l);
}

