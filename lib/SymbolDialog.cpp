/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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
#include "QuoteDataBase.h"

#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QtDebug>
#include <QMessageBox>
#include <QGroupBox>

SymbolDialog::SymbolDialog () : QDialog (0, 0)
{
  modified = 0;
  setWindowTitle(tr("Select Symbols"));

  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  vbox->addLayout(hbox);
  
  // construct left box
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Symbols Available"));
  hbox->addWidget(gbox);
  
  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  gbox->setLayout(tvbox);

  leftSymbols = new QListWidget;
  leftSymbols->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tvbox->addWidget(leftSymbols);
  
  // construct middle button box
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  hbox->addLayout(tvbox);

  rightButton = new QPushButton;
  rightButton->setText(QString(">>"));
  connect(rightButton, SIGNAL(clicked()), this, SLOT(rightButtonPressed()));
  tvbox->addWidget(rightButton);

  leftButton = new QPushButton;
  leftButton->setText(QString("<<"));
  connect(leftButton, SIGNAL(clicked()), this, SLOT(leftButtonPressed()));
  tvbox->addWidget(leftButton);
  
//  tvbox->addStretch(1);

  // construct right box
  gbox = new QGroupBox;
  gbox->setTitle(tr("Symbols Selected"));
  hbox->addWidget(gbox);
  
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  gbox->setLayout(tvbox);

  rightSymbols = new QListWidget;
  rightSymbols->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tvbox->addWidget(rightSymbols);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));
  vbox->addWidget(buttonBox);

  updateLeftSymbols();
}

void SymbolDialog::setSymbols (QStringList &l)
{
  rightSymbols->clear();
  rightSymbols->addItems(l);
}

void SymbolDialog::getSymbols (QStringList &l)
{
  l.clear();
  
  int loop;
  for (loop = 0; loop < rightSymbols->count(); loop++)
  {
    QListWidgetItem *item = rightSymbols->item(loop);
    l.append(item->text());
  }
}

void SymbolDialog::cancelPressed()
{
  if (modified)
  {
    int rc = QMessageBox::warning(this,
    			        tr("Warning"),
			        tr("Items modified. Are you sure you want to discard changes?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
    if (rc == QMessageBox::No)
      return;
  }

  reject();
}

void SymbolDialog::leftButtonPressed ()
{
  QList<QListWidgetItem *> l = rightSymbols->selectedItems();
  if (! l.count())
    return;
  
  int loop;
  for (loop = 0; loop < l.count(); loop++)
    delete l.at(loop);

  modified++;
}

void SymbolDialog::rightButtonPressed ()
{
  QList<QListWidgetItem *> l = leftSymbols->selectedItems();
  if (! l.count())
    return;
  
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    QListWidgetItem *item = l.at(loop);
    rightSymbols->addItem(item->text());
  }
  
  modified++;
}

void SymbolDialog::updateLeftSymbols ()
{
  QuoteDataBase db;
  QStringList l;
  db.getAllChartsList(l);
  
  leftSymbols->clear();
  leftSymbols->addItems(l);
}




