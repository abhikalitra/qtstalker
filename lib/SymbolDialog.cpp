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
#include "Globals.h"
#include "Config.h"

#include "../pics/search.xpm"

#include <QLabel>
#include <QLayout>
#include <QtDebug>
#include <QTreeWidgetItem>
#include <QFormLayout>

SymbolDialog::SymbolDialog ()
{
  _configSizeParm = Config::SymbolDialogSize;
  _configPosParm = Config::SymbolDialogPosition;
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Select Symbols"));

  createMainPage();

  loadExchanges();

  loadSettings();
}

SymbolDialog::~SymbolDialog ()
{
  saveSettings();
}

void SymbolDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  vbox->addLayout(form);
  
  _exchanges = new QComboBox;
  form->addRow(tr("Exchange"), _exchanges);
  
  _search = new QLineEdit("*");
  form->addRow(tr("Symbol pattern"), _search);
  
  _searchButton = new QPushButton;
  _searchButton->setIcon(QIcon(search_xpm));
  connect(_searchButton, SIGNAL(clicked()), this, SLOT(searchButtonPressed()));
  form->addRow(tr("Search"), _searchButton);

  QStringList l;
  l << tr("Symbol") << tr("Name") << tr("Exchange");

  _leftSymbols = new QTreeWidget;
  _leftSymbols->setSortingEnabled(FALSE);
  _leftSymbols->setRootIsDecorated(FALSE);
  _leftSymbols->setHeaderLabels(l);
  _leftSymbols->setSelectionMode(QAbstractItemView::ExtendedSelection);
  vbox->addWidget(_leftSymbols);
  
  _tabs->addTab(w, tr("Search"));
}

void SymbolDialog::setSymbols (QString exchange, QString symbol)
{
  _exchanges->setCurrentIndex(_exchanges->findText(exchange, Qt::MatchExactly));
  _search->setText(symbol);
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
  if (sel.count())
  {
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
  }
  
  emit signalResults(_exchanges->currentText(), _search->text());
  
  accept();  
}

