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
#include "ExchangeDataBase.h"
#include "Dialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

ExchangeSearchDialog::ExchangeSearchDialog () : QDialog (0, 0)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Search For Exchange"));

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(5);
  setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // country
  QLabel *label = new QLabel(tr("Country"));
  grid->addWidget(label, row, col++);

  ExchangeDataBase db;
  QStringList countryList;
  QString k = "countryName";
  db.getFieldList(k, countryList);
  countryList.prepend(QString("<NONE>"));
  
  _country = new QComboBox;
  _country->addItems(countryList);
  grid->addWidget(_country, row++, col--);
  
  // city
  label = new QLabel(tr("City"));
  grid->addWidget(label, row, col++);

  QStringList cityList;
  k = "city";
  db.getFieldList(k, cityList);
  cityList.prepend(QString("<NONE>"));
  
  _city = new QComboBox;
  _city->addItems(cityList);
  grid->addWidget(_city, row++, col--);

  // exchange name
  label = new QLabel(tr("Exchange"));
  grid->addWidget(label, row, col++);

  _exchange = new QLineEdit;
  grid->addWidget(_exchange, row++, col--);

  _list = new QListWidget;
  connect(_list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(itemSelected(QListWidgetItem *)));
  vbox->addWidget(_list);
  
  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(_buttonBox);

  vbox->addStretch(1);
}

void ExchangeSearchDialog::done ()
{
  QString country = _country->currentText();
  if (country == "<NONE>")
    country.clear();

  QString city = _city->currentText();
  if (city == "<NONE>")
    city.clear();

  QString pattern = _exchange->text();

  ExchangeDataBase db;
  QStringList sl;
  if (db.search(country, city, pattern, sl))
  {
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Error Exchange Search"));
    dialog->setMessage(tr("Invalid search pattern.\n") + pattern);
    dialog->show();
    return;
  }

  _list->clear();
  
  if (! sl.count())
  {
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Exchange Search"));
    dialog->setMessage(tr("No items found."));
    dialog->show();
    return;
  }

  _list->addItems(sl);
}

void ExchangeSearchDialog::itemSelected (QListWidgetItem *item)
{
  if (! item)
    return;

  ExchangeDataBase db;
  QString code;
  QString d = item->text();
  db.codeFromName(d, code);
  if (code.isEmpty())
  {
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Exchange Search"));
    dialog->setMessage(tr("No item found."));
    dialog->show();
    return;
  }

  emit signalExchangeCode(code);
}

