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

#include "ScannerNewDialog.h"
#include "Globals.h"
#include "Config.h"
#include "ScannerDataBase.h"
#include "ScannerItem.h"

#include <QtDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

ScannerNewDialog::ScannerNewDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Scanner"));

  createMainPage();

  buttonStatus(QString());
}

void ScannerNewDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(2, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // name
  QLabel *label = new QLabel(tr("Name"));
  grid->addWidget(label, row, col++);

  QStringList l;
  ScannerDataBase db;
  db.getScanners(l);
  
  _name = new QComboBox;
  _name->setEditable(TRUE);
  _name->addItems(l);
  connect(_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(buttonStatus(QString)));
  grid->addWidget(_name, row++, col--);

  // indicator
  label = new QLabel(tr("Indicator"));
  grid->addWidget(label, row, col++);

  l.clear();
  Config config;
  config.getData(Config::IndicatorPluginList, l);

  _indicator = new QComboBox;
  _indicator->addItems(l);
  grid->addWidget(_indicator, row++, col--);

  vbox->addStretch();
  
  _tabs->addTab(w, tr("New Scanner"));
}

void ScannerNewDialog::done ()
{
  QString name = _name->lineEdit()->text();

  // remove any forbidden sql characters
  name = name.remove(QString("'"), Qt::CaseSensitive);
  
  ScannerDataBase db;
  QStringList l;
  db.getScanners(l);
  if (l.indexOf(name) != -1)
  {
    setMessage(tr("A scanner with this name already exists."));
    return;
  }

  ScannerItem scanner;
  scanner.setName(name);
  scanner.setIndicator(_indicator->currentText());

  db.transaction();
  db.setScanner(scanner);
  db.commit();

  emit signalNew(name);

  accept();
}

void ScannerNewDialog::buttonStatus (QString d)
{
  int status = 0;
  if (d.length())
    status = 1;

  QPushButton *b = _buttonBox->button(QDialogButtonBox::Ok);
  if (! b)
    return;

  b->setEnabled(status);
}

