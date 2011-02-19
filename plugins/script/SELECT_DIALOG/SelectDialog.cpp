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
#include "Doc.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QStringList>
#include <QSettings>

SelectDialog::SelectDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";

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

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void SelectDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QLabel *label = new QLabel(_command->parm("TITLE"));
  vbox->addWidget(label);

  // list
  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  vbox->addWidget(_list);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // select all button
  _selectButton = bbox->addButton(tr("Select All"), QDialogButtonBox::ActionRole);
  connect(_selectButton, SIGNAL(clicked()), _list, SLOT(selectAll()));

  // unselect all button
  _unselectButton = bbox->addButton(tr("Unselect All"), QDialogButtonBox::ActionRole);
  connect(_unselectButton, SIGNAL(clicked()), _list, SLOT(clearSelection()));

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setDefault(TRUE);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void SelectDialog::selectionChanged ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  _okButton->setEnabled(sl.count());
}

void SelectDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
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

void SelectDialog::cancel ()
{
  saveSettings();
  reject();
}

void SelectDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("select_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("select_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void SelectDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("select_dialog_window_size", size());
  settings.setValue("select_dialog_window_position", pos());
  settings.sync();
}
