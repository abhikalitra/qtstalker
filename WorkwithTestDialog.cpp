/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "WorkwithTestDialog.h"
#include "Tester.h"
#include "stop.xpm"
#include "open.xpm"
#include "newchart.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include <qinputdialog.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <qstring.h>

WorkwithTestDialog::WorkwithTestDialog (Config *c) : QDialog (0, "WorkwithTestDialog", TRUE)
{
  config = c;

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  toolbar = new QGridLayout(vbox, 1, 6);
  toolbar->setSpacing(1);

  cancelButton = new QToolButton(this);
  QToolTip::add(cancelButton, tr("Cancel"));
  cancelButton->setPixmap(QPixmap(stop));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  cancelButton->setMaximumWidth(30);
  cancelButton->setAutoRaise(TRUE);
  toolbar->addWidget(cancelButton, 0, 0);

  openButton = new QToolButton(this);
  QToolTip::add(openButton, tr("Open Backtest Rule"));
  openButton->setPixmap(QPixmap(open));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openTest()));
  openButton->setMaximumWidth(30);
  openButton->setAutoRaise(TRUE);
  toolbar->addWidget(openButton, 0, 1);
  openButton->setEnabled(FALSE);

  newButton = new QToolButton(this);
  QToolTip::add(newButton, tr("New Backtest Rule"));
  newButton->setPixmap(QPixmap(newchart));
  connect(newButton, SIGNAL(clicked()), this, SLOT(newTest()));
  newButton->setMaximumWidth(30);
  newButton->setAutoRaise(TRUE);
  toolbar->addWidget(newButton, 0, 2);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete Backtest Rule"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteTest()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 3);
  deleteButton->setEnabled(FALSE);

  renameButton = new QToolButton(this);
  QToolTip::add(renameButton, tr("Rename Backtest Rule"));
  renameButton->setPixmap(QPixmap(renam));
  connect(renameButton, SIGNAL(clicked()), this, SLOT(renameTest()));
  renameButton->setMaximumWidth(30);
  renameButton->setAutoRaise(TRUE);
  toolbar->addWidget(renameButton, 0, 4);
  renameButton->setEnabled(FALSE);

  list = new QListView(this);
  list->addColumn(tr("Backtest Rule"), -1);
  list->setSelectionMode(QListView::Single);
  connect(list, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(openTest()));
  connect(list, SIGNAL(clicked(QListViewItem *)), this, SLOT(testSelected(QListViewItem *)));
  vbox->addWidget(list);
}

WorkwithTestDialog::~WorkwithTestDialog ()
{
}

void WorkwithTestDialog::updateList ()
{
  list->clear();

  QStringList l = config->getTestList();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    item = new QListViewItem(list, l[loop]);
}

void WorkwithTestDialog::openTest ()
{
  item = list->selectedItem();
  if (! item)
    return;

  Tester *dialog = new Tester(config, item->text(0));
  dialog->show();
  accept();
}

void WorkwithTestDialog::newTest()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Backtest Rule"),
  					    tr("Enter new backtest rule name."),
  					    QLineEdit::Normal,
					    tr("New Rule"),
					    &ok,
					    this);
					    
  if ((! ok) || (selection.isNull()))
    return;

  QStringList l = config->getTest(selection);

  if (l.count())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  Tester *dialog = new Tester(config, selection);
  dialog->show();
  accept();
}

void WorkwithTestDialog::deleteTest()
{
  item = list->selectedItem();
  if (! item)
    return;

  config->deleteTest(item->text(0));

  delete item;
}

void WorkwithTestDialog::renameTest ()
{
  item = list->selectedItem();
  if (! item)
    return;

  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Backtest Rule"),
  					    tr("Enter new backtest rule name."),
  					    QLineEdit::Normal,
					    item->text(0),
					    &ok,
					    this);

  if ((! ok) || (selection.isNull()))
    return;

  QStringList l = config->getTest(selection);
  if (l.count())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This backtest rule already exists."));
    return;
  }

  l = config->getTest(item->text(0));

  config->deleteTest(item->text(0));

  config->setTest(selection, l);

  updateList();
}

void WorkwithTestDialog::testSelected (QListViewItem *i)
{
  if (! i)
  {
    openButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
    renameButton->setEnabled(FALSE);
  }
  else
  {
    openButton->setEnabled(TRUE);
    deleteButton->setEnabled(TRUE);
    renameButton->setEnabled(TRUE);
  }
}

