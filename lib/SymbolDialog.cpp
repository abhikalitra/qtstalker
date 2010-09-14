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

#include "SymbolDialog.h"
#include "BarData.h"
#include "QuoteServerRequestThread.h"

#include "../pics/search.xpm"
#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QtDebug>
#include <QMessageBox>
#include <QGroupBox>
#include <QTreeWidgetItem>

SymbolDialog::SymbolDialog (int flag) : QDialog (0, 0)
{
  modified = 0;
  setWindowTitle(tr("Select Symbols"));

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  vbox->addLayout(hbox);
  
  // construct left box
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Symbols"));
  hbox->addWidget(gbox);
  
  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  gbox->setLayout(tvbox);

  QHBoxLayout *thbox = new QHBoxLayout;
  thbox->setSpacing(2);
  tvbox->addLayout(thbox);
  
  exchanges = new QComboBox;
  exchanges->setToolTip(QString(tr("Exchange")));
  thbox->addWidget(exchanges);
  
  search = new QLineEdit;
  search->setToolTip(QString(tr("Symbol pattern")));
  thbox->addWidget(search);
  
  searchButton = new QToolButton;
  searchButton->setIcon(QIcon(search_xpm));
  searchButton->setToolTip(QString(tr("Search")));
  connect(searchButton, SIGNAL(clicked()), this, SLOT(searchButtonPressed()));
  thbox->addWidget(searchButton);

  QStringList l;
  l << tr("Symbol") << tr("Name") << tr("Exchange");

  leftSymbols = new QTreeWidget;
  leftSymbols->setRootIsDecorated(FALSE);
  leftSymbols->setHeaderLabels(l);
  leftSymbols->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tvbox->addWidget(leftSymbols);
  
  // construct right box
  gbox = new QGroupBox;
  gbox->setTitle(tr("Symbols Selected"));
  hbox->addWidget(gbox);
  
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  gbox->setLayout(tvbox);

  thbox = new QHBoxLayout;
  thbox->setSpacing(2);
  tvbox->addLayout(thbox);
  
  addButton = new QToolButton;
  addButton->setIcon(QIcon(add_xpm));
  addButton->setToolTip(QString(tr("Add selected symbols")));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
  thbox->addWidget(addButton);

  deleteButton = new QToolButton;
  deleteButton->setIcon(QIcon(delete_xpm));
  deleteButton->setToolTip(QString(tr("Remove selected symbols")));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteButtonPressed()));
  thbox->addWidget(deleteButton);
  
  thbox->addStretch();
  
  rightSymbols = new QTreeWidget;
  rightSymbols->setRootIsDecorated(FALSE);
  rightSymbols->setHeaderLabels(l);
  rightSymbols->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tvbox->addWidget(rightSymbols);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));
  vbox->addWidget(buttonBox);
  
  if (flag)
    gbox->hide();

  loadExchanges();
}

void SymbolDialog::setSymbols (QString &ex, QString &ss)
{
  exchanges->setCurrentIndex(exchanges->findText(ex, Qt::MatchExactly));
  search->setText(ss);
}

void SymbolDialog::getSymbols (Group &l)
{
  int loop;
  for (loop = 0; loop < rightSymbols->columnCount(); loop++)
  {
    QTreeWidgetItem *item = rightSymbols->topLevelItem(loop);
    if (! item)
      continue;
    
    BarData bd;
    QString s = item->text(0);
    bd.setSymbol(s);
    
    s = item->text(1);
    bd.setName(s);
    
    s = item->text(2);
    bd.setExchange(s);
    
    l.setSymbol(bd);
  }
}

void SymbolDialog::getSymbolSearch (QString &ex, QString &ss)
{
  ex = exchanges->currentText();
  ss = search->text();
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

void SymbolDialog::deleteButtonPressed ()
{
  QList<QTreeWidgetItem *> l = rightSymbols->selectedItems();
  if (! l.count())
    return;
  
  int loop;
  for (loop = 0; loop < l.count(); loop++)
    delete l.at(loop);

  for (loop = 0; loop < rightSymbols->columnCount(); loop++)
    rightSymbols->resizeColumnToContents(loop);

  modified++;
}

void SymbolDialog::addButtonPressed ()
{
  QList<QTreeWidgetItem *> l = leftSymbols->selectedItems();
  if (! l.count())
    return;
  
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    QTreeWidgetItem *item = l.at(loop);
    QTreeWidgetItem *item2 = new QTreeWidgetItem(rightSymbols);
    int loop;
    for (loop = 0; loop < 3; loop++)
      item2->setText(loop, item->text(loop));
  }

  for (loop = 0; loop < rightSymbols->columnCount(); loop++)
    rightSymbols->resizeColumnToContents(loop);

  modified++;
}

void SymbolDialog::searchButtonPressed ()
{
  QStringList l;
  l << "Search" << "S";
  
  l << exchanges->currentText();
  
  QString s = search->text();
  if (s.isEmpty())
    s = "*";
  l << s;
    
  QString command = l.join(",") + "\n";

  QuoteServerRequestThread *r = new QuoteServerRequestThread(this, command);
  connect(r, SIGNAL(signalDone(QString)), this, SLOT(searchRequestDone(QString)), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}

void SymbolDialog::searchRequestDone (QString data)
{
  leftSymbols->clear();
  
  QStringList l = data.split(":");

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() != 2)
      continue;

    QTreeWidgetItem *item = new QTreeWidgetItem(leftSymbols);
    
    QString name;
    item->setText(0, l2.at(1));
    item->setText(1, name);
    item->setText(2, l2.at(0));
  }
  
  for (loop = 0; loop < leftSymbols->columnCount(); loop++)
    leftSymbols->resizeColumnToContents(loop);
}

void SymbolDialog::loadExchanges ()
{
  QString command = "Search,E\n";

  QuoteServerRequestThread *r = new QuoteServerRequestThread(this, command);
  connect(r, SIGNAL(signalDone(QString)), this, SLOT(exchangeRequestDone(QString)), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}

void SymbolDialog::exchangeRequestDone (QString data)
{
  QStringList l = data.split(":");
  l.prepend("*");
  
  exchanges->clear();
  exchanges->addItems(l);
  exchanges->setCurrentIndex(0);
}

void SymbolDialog::done ()
{
  emit signalResults(exchanges->currentText(), search->text());
  
  accept();  
}

