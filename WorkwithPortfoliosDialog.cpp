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

#include "WorkwithPortfoliosDialog.h"
#include "EditDialog.h"
#include "PortfolioDialog.h"
#include "Setting.h"
#include "open.xpm"
#include "newchart.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "rename.xpm"
#include "done.xpm"
#include <qinputdialog.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qdir.h>
#include <qmessagebox.h>

WorkwithPortfoliosDialog::WorkwithPortfoliosDialog (Config *c) : QDialog (0, 0, FALSE)
{
  config = c;

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QGridLayout *grid = new QGridLayout(vbox, 1, 7);
  grid->setSpacing(1);

  openButton = new QToolButton(this);
  QToolTip::add(openButton, tr("Open Portfolio"));
  openButton->setPixmap(QPixmap(open));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openPortfolio()));
  openButton->setMaximumWidth(30);
  grid->addWidget(openButton, 0, 0);

  newButton = new QToolButton(this);
  QToolTip::add(newButton, tr("New Portfolio"));
  newButton->setPixmap(QPixmap(newchart));
  connect(newButton, SIGNAL(clicked()), this, SLOT(newPortfolio()));
  newButton->setMaximumWidth(30);
  grid->addWidget(newButton, 0, 1);

  editButton = new QToolButton(this);
  QToolTip::add(editButton, tr("Edit Portfolio"));
  editButton->setPixmap(QPixmap(edit));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editPortfolio()));
  editButton->setMaximumWidth(30);
  grid->addWidget(editButton, 0, 2);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete Portfolio"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deletePortfolio()));
  deleteButton->setMaximumWidth(30);
  grid->addWidget(deleteButton, 0, 3);

  renameButton = new QToolButton(this);
  QToolTip::add(renameButton, tr("Rename Portfolio"));
  renameButton->setPixmap(QPixmap(renam));
  connect(renameButton, SIGNAL(clicked()), this, SLOT(renamePortfolio()));
  renameButton->setMaximumWidth(30);
  grid->addWidget(renameButton, 0, 4);

  QToolButton *button = new QToolButton(this);
  QToolTip::add(button, tr("Done"));
  button->setPixmap(QPixmap(finished));
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));
  button->setMaximumWidth(30);
  grid->addWidget(button, 0, 5);
  
  QFrame *sep = new QFrame(this);
  sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  vbox->addWidget(sep);

  list = new QListView(this);
  list->addColumn(0, 200);
  list->setSelectionMode(QListView::Single);
  connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));
  connect(list, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(openPortfolio()));
  vbox->addWidget(list);

  buttonStatus();
}

WorkwithPortfoliosDialog::~WorkwithPortfoliosDialog ()
{
}

void WorkwithPortfoliosDialog::updateList ()
{
  list->clear();
  
  QStringList l = config->getPortfolioList();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    item = new QListViewItem(list, l[loop]);
}

void WorkwithPortfoliosDialog::openPortfolio ()
{
  item = list->selectedItem();
  if (! item)
    return;

  PortfolioDialog *dialog = new PortfolioDialog(config, item->text(0));
  dialog->show();
  reject();
}

void WorkwithPortfoliosDialog::newPortfolio()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Portfolio"), tr("Enter new portfolio name."),
  					    QLineEdit::Normal, tr("New Portfolio"), &ok, this);
  if ((ok) && (! selection.isNull()))
  {
    QStringList l = config->getPortfolio(selection);

    if (l.count())
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    PortfolioDialog *dialog = new PortfolioDialog(config, selection);
    dialog->show();
    reject();
  }
}

void WorkwithPortfoliosDialog::editPortfolio()
{
  item = list->selectedItem();
  if (! item)
    return;

  PortfolioDialog *dialog = new PortfolioDialog(config, item->text(0));
  dialog->show();
  reject();
}

void WorkwithPortfoliosDialog::deletePortfolio()
{
  item = list->selectedItem();
  if (! item)
    return;

  config->deletePortfolio(item->text(0));
  delete item;
  buttonStatus();
}

void WorkwithPortfoliosDialog::renamePortfolio ()
{
  item = list->selectedItem();
  if (! item)
    return;

  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Portfolio"), tr("Enter new portfolio name."),
  					    QLineEdit::Normal, item->text(0), &ok, this);
  if ((ok) && (! selection.isNull()))
  {
    QStringList l = config->getPortfolio(selection);
    if (l.count())
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    l = config->getPortfolio(item->text(0));
    config->deletePortfolio(item->text(0));
    config->setPortfolio(selection, l);

    updateList();
    buttonStatus();
  }
}

void WorkwithPortfoliosDialog::buttonStatus ()
{
  item = list->selectedItem();
  if (item)
  {
    openButton->setEnabled(TRUE);
    deleteButton->setEnabled(TRUE);
    editButton->setEnabled(TRUE);
    renameButton->setEnabled(TRUE);
  }
  else
  {
    openButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
    editButton->setEnabled(FALSE);
    renameButton->setEnabled(FALSE);
  }
}



