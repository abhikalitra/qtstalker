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
#include "Globals.h"
#include "Doc.h"
#include "Command.h"
#include "ScriptPluginFactory.h"

#include "../pics/search.xpm"

#include <QLayout>
#include <QtDebug>
#include <QTreeWidgetItem>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSettings>
#include <QGroupBox>

SymbolDialog::SymbolDialog ()
{
  _helpFile = "main.html";
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Select Symbols"));

  createGUI();

  loadExchanges();

  loadSettings();

  searchSelectionChanged();
  symbolSelectionChanged();
}

void SymbolDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  vbox->addLayout(form);

  _exchanges = new QComboBox;
  _exchanges->setToolTip(tr("Select a specific exchange or * for all"));
  form->addRow(tr("Exchange"), _exchanges);

  _search = new QLineEdit("*");
  _search->setToolTip(tr("Enter a partial search like %OOG% or * for all"));
  form->addRow(tr("Symbol pattern"), _search);

  _searchButton = new QPushButton;
  _searchButton->setIcon(QIcon(search_xpm));
  _searchButton->setToolTip(tr("Perform search"));
  connect(_searchButton, SIGNAL(clicked()), this, SLOT(searchButtonPressed()));
  form->addRow(tr("Search"), _searchButton);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  vbox->addLayout(hbox);

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Search Results"));
  hbox->addWidget(gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  gbox->setLayout(tvbox);

  QStringList l;
  l << tr("Symbol") << tr("Name");

  _searchList = new QTreeWidget;
  _searchList->setSortingEnabled(FALSE);
  _searchList->setRootIsDecorated(FALSE);
  _searchList->setHeaderLabels(l);
  _searchList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_searchList, SIGNAL(itemSelectionChanged()), this, SLOT(searchSelectionChanged()));
//  connect(_searchList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  tvbox->addWidget(_searchList);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  tvbox->addStretch(1);

  _addButton = new QPushButton;
  _addButton->setText(">>");
  connect(_addButton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
  tvbox->addWidget(_addButton);

  _deleteButton = new QPushButton;
  _deleteButton->setText("<<");
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteButtonPressed()));
  tvbox->addWidget(_deleteButton);

  tvbox->addStretch(1);

  gbox = new QGroupBox;
  gbox->setTitle(tr("Symbols"));
  hbox->addWidget(gbox);

  tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  gbox->setLayout(tvbox);

  _symbolList = new QTreeWidget;
  _symbolList->setSortingEnabled(FALSE);
  _symbolList->setRootIsDecorated(FALSE);
  _symbolList->setHeaderLabels(l);
  _symbolList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_symbolList, SIGNAL(itemSelectionChanged()), this, SLOT(symbolSelectionChanged()));
//  connect(_symbolList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  tvbox->addWidget(_symbolList);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

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

void SymbolDialog::searchSelectionChanged ()
{
  int status = 0;
  QList<QTreeWidgetItem *> sl = _searchList->selectedItems();
  if (sl.count())
    status = 1;

  _addButton->setEnabled(status);
}

void SymbolDialog::symbolSelectionChanged ()
{
  int status = 0;
  QList<QTreeWidgetItem *> sl = _symbolList->selectedItems();
  if (sl.count())
    status = 1;

  _deleteButton->setEnabled(status);
}

void SymbolDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void SymbolDialog::done ()
{
  QStringList l;
  symbols(l);
  if (l.count())
    emit signalDone(l.join(","));

  saveSettings();

  l.clear();
  l << _exchanges->currentText();
  if (! _search->text().length())
    l << "*";
  else
    l << _search->text();
  
  emit signalDone(l.join(","));
  
  accept();
}

void SymbolDialog::cancel ()
{
  saveSettings();
  reject();
}

QString SymbolDialog::stringData ()
{
  QString s = "0";
  QStringList l;
  symbols(l);
  if (l.count())
    s = l.join(",");
  return s;
}

void SymbolDialog::addButtonPressed ()
{
  QList<QTreeWidgetItem *> sl = _searchList->selectedItems();

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QTreeWidgetItem *item = sl.at(loop);

    QTreeWidgetItem *nitem = new QTreeWidgetItem(_symbolList);
    nitem->setText(0, item->text(0));
    nitem->setText(1, item->text(1));
  }

  for (loop = 0; loop < _symbolList->columnCount(); loop++)
    _symbolList->resizeColumnToContents(loop);
}

void SymbolDialog::deleteButtonPressed ()
{
  QList<QTreeWidgetItem *> sl = _symbolList->selectedItems();

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QTreeWidgetItem *item = sl.at(loop);
    delete item;
  }

  for (loop = 0; loop < _symbolList->columnCount(); loop++)
    _symbolList->resizeColumnToContents(loop);

  symbolSelectionChanged();
}

void SymbolDialog::searchButtonPressed ()
{
  QStringList l;
  l << "QUOTE_SYMBOL_SEARCH" << _exchanges->currentText();
  QString s = _search->text();
  if (s.isEmpty())
    s = "*";
  l << s;

  Command command(l.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "SYMBOL_DIALOG::searchButtonPressed: no plugin" << command.plugin();
    return;
  }
  
  connect(plug, SIGNAL(signalDone(QStringList)), this, SLOT(searchRequestDone(QStringList)));

  plug->command(command);
  
/*
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
*/
}

void SymbolDialog::searchRequestDone (QStringList data)
{
  _searchList->clear();

  int loop = 0;
  for (; loop < data.count(); loop++)
  {
    QStringList l = data.at(loop).split(",");
    if (l.count() != 2)
      continue;

    QTreeWidgetItem *item = new QTreeWidgetItem(_searchList);
    item->setText(0, l.at(0));
    item->setText(1, l.at(1));
  }

  for (loop = 0; loop < _searchList->columnCount(); loop++)
    _searchList->resizeColumnToContents(loop);
}

void SymbolDialog::loadExchanges ()
{
  Command command("QUOTE_EXCHANGE");

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "SYMBOL_DIALOG::loadExchanges: no plugin" << command.plugin();
    return;
  }

  connect(plug, SIGNAL(signalDone(QStringList)), this, SLOT(loadExchanges2(QStringList)));

  plug->command(command);

/*  
  QString command = "Search,E\n";

  QuoteServerRequestThread *r = new QuoteServerRequestThread(this, command);
  connect(r, SIGNAL(signalDone(QString)), this, SLOT(exchangeRequestDone(QString)), Qt::QueuedConnection);
  r->start();
*/
}

void SymbolDialog::loadExchanges2 (QStringList l)
{
  l.prepend("*");

  _exchanges->clear();
  _exchanges->addItems(l);
  _exchanges->setCurrentIndex(0);
}

void SymbolDialog::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("symbol_dialog_window_size", QSize(400,300)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("symbol_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void SymbolDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("symbol_dialog_window_size", size());
  settings.setValue("symbol_dialog_window_position", pos());
  settings.sync();
}

void SymbolDialog::symbols (QStringList &l)
{
  l.clear();
  
  QList<QTreeWidgetItem *> sel = _symbolList->selectedItems();
  if (sel.count())
  {
    int loop = 0;
    for (; loop < sel.count(); loop++)
    {
      QTreeWidgetItem *item = sel.at(loop);
      l.append(item->text(0));
    }
  }
}
