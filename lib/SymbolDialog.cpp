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
#include "Dialog.h"
#include "Globals.h"
#include "../pics/search.xpm"

#include <QLabel>
#include <QLayout>
#include <QtDebug>
#include <QGroupBox>
#include <QTreeWidgetItem>

SymbolDialog::SymbolDialog () : Dialog (Dialog::_Dialog, 0)
{
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Select Symbols"));

  createMainPage();
}

void SymbolDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);

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
  
  _exchanges = new QComboBox;
  _exchanges->setToolTip(QString(tr("Exchange")));
  thbox->addWidget(_exchanges);
  
  _search = new QLineEdit("*");
  _search->setToolTip(QString(tr("Symbol pattern")));
  thbox->addWidget(_search);
  
  _searchButton = new QToolButton;
  _searchButton->setIcon(QIcon(search_xpm));
  _searchButton->setToolTip(QString(tr("Search")));
  connect(_searchButton, SIGNAL(clicked()), this, SLOT(searchButtonPressed()));
  thbox->addWidget(_searchButton);

  QStringList l;
  l << tr("Symbol") << tr("Name") << tr("Exchange");

  _leftSymbols = new QTreeWidget;
  _leftSymbols->setSortingEnabled(TRUE);
  _leftSymbols->setRootIsDecorated(FALSE);
  _leftSymbols->setHeaderLabels(l);
  _leftSymbols->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tvbox->addWidget(_leftSymbols);
  
  _tabs->addTab(w, tr("Settings"));

  loadExchanges();
}

void SymbolDialog::setSymbols (QString &ex, QString &ss)
{
  _exchanges->setCurrentIndex(_exchanges->findText(ex, Qt::MatchExactly));
  _search->setText(ss);
}

void SymbolDialog::getSymbols (Group &l)
{
  int loop;
  for (loop = 0; loop < _leftSymbols->columnCount(); loop++)
  {
    QTreeWidgetItem *item = _leftSymbols->topLevelItem(loop);
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
  ex = _exchanges->currentText();
  ss = _search->text();
}

void SymbolDialog::searchButtonPressed ()
{
  QStringList l;
  l << "Search" << "S";
  
  l << _exchanges->currentText();
  
  QString s = _search->text();
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
  _leftSymbols->clear();
  
  QStringList l = data.split(":");

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() != 2)
      continue;

    QTreeWidgetItem *item = new QTreeWidgetItem(_leftSymbols);
    
    QString name;
    item->setText(0, l2.at(1));
    item->setText(1, name);
    item->setText(2, l2.at(0));
  }
  
  for (loop = 0; loop < _leftSymbols->columnCount(); loop++)
    _leftSymbols->resizeColumnToContents(loop);
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
  
  _exchanges->clear();
  _exchanges->addItems(l);
  _exchanges->setCurrentIndex(0);
}

void SymbolDialog::done ()
{
  QList<QTreeWidgetItem *> sel = _leftSymbols->selectedItems();
  if (! sel.count())
    return;
   
  Group g;
  int loop = 0;
  for (; loop < sel.count(); loop++)
  {
    QTreeWidgetItem *item = sel.at(loop);

    BarData bd;
    QString s = item->text(0);
    bd.setSymbol(s);

    s = item->text(1);
    bd.setName(s);

    s = item->text(2);
    bd.setExchange(s);

    g.setSymbol(bd);
  }
  
  emit signalSymbols(g);
  
  emit signalResults(_exchanges->currentText(), _search->text());
  
  accept();  
}

