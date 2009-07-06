/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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


#include "SymbolDialog.h"
#include "DataBase.h"
#include "../pics/insert.xpm"
#include "../pics/delete.xpm"
#include "../pics/openchart.xpm"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QIcon>
#include <QInputDialog>
#include <QList>
#include <QListWidgetItem>





SymbolDialog::SymbolDialog (QWidget *w, QStringList &l) : QDialog (w, 0)
{
  QString s = "Select Symbols";
  setWindowTitle(s);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(10);
  vbox->setSpacing(10);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  vbox->addLayout(hbox);

  fileList = new QListWidget;
  fileList->setSortingEnabled(TRUE);
  fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileList->addItems(l);
  hbox->addWidget(fileList);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  addFileButton = new QToolButton;
  addFileButton->setIcon(QIcon(insert));
  addFileButton->setToolTip(tr("Add Symbols"));
  connect(addFileButton, SIGNAL(clicked()), this, SLOT(addSymbols()));
  tvbox->addWidget(addFileButton);

  addGroupButton = new QToolButton;
  addGroupButton->setIcon(QIcon(openchart));
  addGroupButton->setToolTip(tr("Add Group"));
  connect(addGroupButton, SIGNAL(clicked()), this, SLOT(addGroups()));
  tvbox->addWidget(addGroupButton);

  deleteFileButton = new QToolButton;
  deleteFileButton->setIcon(QIcon(deleteitem));
  deleteFileButton->setToolTip(tr("Remove Symbols"));
  connect(deleteFileButton, SIGNAL(clicked()), this, SLOT(deleteSymbols()));
  tvbox->addWidget(deleteFileButton);

  tvbox->addStretch(1);

  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(bbox);

  vbox->addStretch(1);
}

void SymbolDialog::getSymbols (QStringList &l)
{
  int loop;
  l.clear();
  for (loop = 0; loop < fileList->count(); loop++)
  {
    QListWidgetItem *item = fileList->item(loop);
    l.append(item->text());
  }
}

void SymbolDialog::addSymbols ()
{
  bool ok;
  QString s = QInputDialog::getText(this, tr("Select symbols"), tr("Symbol contains"), QLineEdit::Normal, QString(), &ok, 0);
  if (s.isEmpty())
    return;

  QStringList l;
  DataBase db;
  db.getAllChartsList(l);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    if (l[loop].contains(s))
      fileList->addItem(l[loop]);
  }
}

void SymbolDialog::addGroups ()
{
  QStringList l;
  DataBase db;
  db.getAllGroupsList(l);

  bool ok;
  QString s = QInputDialog::getItem(this, tr("Select Group"), tr("Group to add"), l, 0, FALSE, &ok, 0);
  if (s.isEmpty())
    return;

  db.getGroupList(s, l);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
    fileList->addItem(l[loop]);
}

void SymbolDialog::deleteSymbols ()
{
  QList<QListWidgetItem *> list = fileList->selectedItems();
  qDeleteAll(list);
}

