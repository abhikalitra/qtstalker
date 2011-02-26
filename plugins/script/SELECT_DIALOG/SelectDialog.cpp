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

#include <QtDebug>

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

  // list
  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  _vbox->insertWidget(pos++, _list);

  // select all button
  _selectButton = _buttonBox->addButton(tr("Select All"), QDialogButtonBox::ActionRole);
  connect(_selectButton, SIGNAL(clicked()), _list, SLOT(selectAll()));

  // unselect all button
  _unselectButton = _buttonBox->addButton(tr("Unselect All"), QDialogButtonBox::ActionRole);
  connect(_unselectButton, SIGNAL(clicked()), _list, SLOT(clearSelection()));
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
