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

#include "ExchangeSearchDialog.h"
#include "Globals.h"
#include "Doc.h"

#include "../pics/search.xpm"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QSettings>
#include <QFormLayout>

ExchangeSearchDialog::ExchangeSearchDialog (Command *command)
{
  _command = command;
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("Exchange Search"));

  createGUI();

  loadSettings();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void ExchangeSearchDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(5);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(0);
  vbox->addLayout(form);

  // exchange name
  _exchange = new QComboBox;
  form->addRow(tr("Exchange"), _exchange);

  // country
  _country = new QComboBox;
  form->addRow(tr("Country"), _country);

  // city
  _city = new QComboBox;
  form->addRow(tr("City"), _city);

  // search button
  QPushButton *b = new QPushButton;
  b->setIcon(QIcon(search_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(search()));
  form->addRow(tr("Perform Search"), b);

  // search results
  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  vbox->addWidget(_list);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
//  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
//  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  connect(_okButton, SIGNAL(clicked()), this, SLOT(done()));
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
  _cancelButton->setDefault(TRUE);

  // help button
  b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void ExchangeSearchDialog::search ()
{
  QString country = _country->currentText();
  if (country == "<NONE>")
    country.clear();

  QString city = _city->currentText();
  if (city == "<NONE>")
    city.clear();

  QString pattern;
  QStringList sl;
  if (_db.search(country, city, pattern, sl))
  {
    _message->setText(tr("Invalid search pattern\n") + pattern);
    return;
  }

  _list->clear();

  if (! sl.count())
  {
    _message->setText(tr("No items found"));
    return;
  }

  _list->addItems(sl);
  _message->clear();
}


void ExchangeSearchDialog::selectionChanged ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (! sl.count())
    return;

  QString code;
  QString d = sl.at(0)->text();
  _db.codeFromName(d, code);
  if (code.isEmpty())
    return;

  _exchange->setCurrentIndex(_exchange->findText(code));
  _okButton->setEnabled(TRUE);
}

void ExchangeSearchDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ExchangeSearchDialog::done ()
{
  _command->setReturnData("EXCHANGE_SEARCH_DIALOG_SELECTED", _exchange->currentText());

  _command->setReturnCode("0");

  saveSettings();

  accept();
}

void ExchangeSearchDialog::cancel ()
{
  saveSettings();
  reject();
}

void ExchangeSearchDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("exchange_search_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("exchange_search_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);

  // exchange
  QStringList l;
  _db.getExchanges(l);
  _exchange->clear();
  _exchange->addItems(l);  
  _exchange->setCurrentIndex(_exchange->findText(settings.value("exchange_search_dialog_last_selection").toString()));

  // country
  QString k = "countryName";
  _db.getFieldList(k, l);
  l.prepend(QString("<NONE>"));
  _country->clear();
  _country->addItems(l);

  // city
  k = "city";
  _db.getFieldList(k, l);
  l.prepend(QString("<NONE>"));
  _city->clear();
  _city->addItems(l);
}

void ExchangeSearchDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("exchange_search_dialog_window_size", size());
  settings.setValue("exchange_search_dialog_window_position", pos());
  settings.setValue("exchange_search_dialog_last_selection", _exchange->currentText());
  settings.sync();
}
