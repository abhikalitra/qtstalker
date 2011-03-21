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

#include "SelectDialog.h"
#include "Globals.h"
#include "../../../pics/select_all.xpm"
#include "../../../pics/unselect_all.xpm"

#include <QtDebug>
#include <QToolBar>
#include <QToolButton>

SelectDialog::SelectDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _keySize = "select_dialog_window_size";
  _keyPos = "select_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Select") << _command->parm("TITLE");
  setWindowTitle(l.join(" "));

  createGUI();

  if (_command->parm("MODE").toInt())
    _list->setSelectionMode(QAbstractItemView::SingleSelection);

  l = _command->parm("ITEMS").split(";", QString::SkipEmptyParts);
  _list->addItems(l);

  loadSettings();

  selectionChanged();
}

void SelectDialog::createGUI ()
{
  int pos = 0;
  QLabel *label = new QLabel(_command->parm("TITLE"));
  _vbox->insertWidget(pos++, label);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  _vbox->insertLayout(pos++, hbox);

  // list
  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  hbox->addWidget(_list);

  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  hbox->addWidget(tb);

  // select all button
  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(select_all_xpm));
  b->setToolTip(tr("Select All"));
  connect(b, SIGNAL(clicked(bool)), _list, SLOT(selectAll()));
  tb->addWidget(b);

  // unselect all button
  b = new QToolButton;
  b->setIcon(QIcon(unselect_all_xpm));
  b->setToolTip(tr("Unselect All"));
  connect(b, SIGNAL(clicked(bool)), _list, SLOT(clearSelection()));
  tb->addWidget(b);

  // clear some uneeded space
  _message->hide();
}

void SelectDialog::selectionChanged ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  _okButton->setEnabled(sl.count());
}

void SelectDialog::done ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  QStringList l;
  int loop = 0;
  for (; loop < sl.count(); loop++)
    l << sl.at(loop)->text();
  
  _command->setReturnData("SELECT_DIALOG_SELECTED", l.join(";"));

  _command->setReturnCode("0");

  saveSettings();
  
  accept();
}
