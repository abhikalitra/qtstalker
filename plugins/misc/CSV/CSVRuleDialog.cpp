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

#include "CSVRuleDialog.h"
#include "CSVDataBase.h"
#include "ExchangeDataBase.h"
#include "Config.h"
#include "CSVRule.h"

#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QInputDialog>
#include <QDir>
#include <QGroupBox>
#include <QList>
#include <QDebug>

CSVRuleDialog::CSVRuleDialog (QString &name) : QDialog (0, 0)
{
  _saveFlag = FALSE;
  _name = name;

  _fieldList << "Exchange" << "Symbol" << "Date" << "Time" << "Open" << "High" << "Low" << "Close";
  _fieldList << "Volume" << "OI" << "Name" << "Ignore";
  
  createGUI();

  loadRule();

  QString s = tr("Editing CSV Rule: ") + _name;
  setWindowTitle(s);
}

void CSVRuleDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;
  
  QLabel *label = new QLabel(tr("Type"));
  grid->addWidget(label, row, col++);

  Config config;
  QStringList l;
  config.getData(Config::DBPluginList, l);
  
  _type = new QComboBox;
  _type->addItems(l);
  connect(_type, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_type, row++, col--);
  
  label = new QLabel(tr("Delimiter"));
  grid->addWidget(label, row, col++);
  
  _delimeter = new QComboBox;
  _delimeter->addItem(QString(","));
  _delimeter->addItem(QString(";"));
  connect(_delimeter, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_delimeter, row++, col--);
  
  label = new QLabel(tr("CSV File"));
  grid->addWidget(label, row, col++);

  QString s = QDir::homePath();
  _file = new FileButton(this, s);
  connect(_file, SIGNAL(signalFileChanged()), this, SLOT(ruleChanged()));
  grid->addWidget(_file, row++, col--);

  label = new QLabel(tr("Exchange"));
  grid->addWidget(label, row, col++);

  ExchangeDataBase edb;
  l.clear();
  edb.getExchanges(l);
  
  _exchange = new QComboBox;
  _exchange->addItems(l);
  connect(_exchange, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_exchange, row++, col--);

//  label = new QLabel(tr("Use filename as symbol"));
//  grid->addWidget(label, row, col++);

  col = 1;
  _fileSymbol = new QCheckBox(tr("Use filename as symbol"));
  connect(_fileSymbol, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_fileSymbol, row++, col--);

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("CSV Rule Format"));
  vbox->addWidget(gbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  gbox->setLayout(hbox);
  
  _ruleList = new QListWidget;
//  connect(_ruleList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(deleteRuleField(QListWidgetItem *)));
  hbox->addWidget(_ruleList);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  _addButton = new QPushButton;
  _addButton->setText(tr("Add"));
  connect(_addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  tvbox->addWidget(_addButton);

  _insertButton = new QPushButton;
  _insertButton->setText(tr("Insert"));
  connect(_insertButton, SIGNAL(clicked()), this, SLOT(insertClicked()));
  tvbox->addWidget(_insertButton);

  _deleteButton = new QPushButton;
  _deleteButton->setText(tr("Delete"));
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
  tvbox->addWidget(_deleteButton);

  tvbox->addStretch(1);

  _buttonBox = new QDialogButtonBox;
  vbox->addWidget(_buttonBox);

  QPushButton *b = new QPushButton(tr("&OK"));
  _buttonBox->addButton(b, QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked()), this, SLOT(done()));

  b = new QPushButton(tr("&Cancel"));
  _buttonBox->addButton(b, QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked()), this, SLOT(reject()));
}

void CSVRuleDialog::saveRule ()
{
  if (! _saveFlag)
    return;

  CSVRule rule;
  rule.setName(_name);
  rule.setType(_type->currentText());
  rule.setDelimeter(_delimeter->currentText());
  rule.setFile(_file->getFile());

  QStringList l;
  int loop = 0;
  for (; loop < _ruleList->count(); loop++)
    l.append(_ruleList->item(loop)->text());
  rule.setRule(l.join(","));

  rule.setFileSymbol(_fileSymbol->isChecked());
  rule.setExchange(_exchange->currentText());

  CSVDataBase db;
  db.setRule(rule);
  
  ruleChanged();
}

void CSVRuleDialog::loadRule ()
{
  clear();
  
  CSVRule rule;
  rule.setName(_name);
  
  CSVDataBase db;
  if (db.getRule(rule))
    return;

  _type->setCurrentIndex(_type->findText(rule.type(), Qt::MatchExactly));
  _delimeter->setCurrentIndex(_delimeter->findText(rule.delimeter(), Qt::MatchExactly));
  _file->setFile(rule.file());

  QStringList l = rule.rule().split(",");
  int loop = 0;
  for (; loop < l.count(); loop++)
    new QListWidgetItem(l[loop], _ruleList, 0);

  _fileSymbol->setChecked(rule.fileSymbol());
  _exchange->setCurrentIndex(_exchange->findText(rule.exchange(), Qt::MatchExactly));
}

void CSVRuleDialog::deleteClicked ()
{
  QList<QListWidgetItem *> l = _ruleList->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);

  ruleChanged();
}

void CSVRuleDialog::ruleChanged ()
{
  _saveFlag = TRUE;
}

void CSVRuleDialog::addClicked ()
{
  bool ok;
  QString item = QInputDialog::getItem(this,
                                       tr("Add Field"),
                                       tr("Select CSV field to add"),
                                       _fieldList,
                                       0,
                                       FALSE,
                                       &ok,
                                       0);

  if (! ok || item.isEmpty())
    return;

  if (item == "Date")
  {
    item.clear();
    dateDialog(item);
    if (item.isEmpty())
      return;

    item = "Date=" + item;
  }
                                       
  if (item == "Time")
  {
    item.clear();
    timeDialog(item);
    if (item.isEmpty())
      return;

    item = "Time=" + item;
  }

  new QListWidgetItem(item, _ruleList, 0);

  ruleChanged();
}

void CSVRuleDialog::insertClicked ()
{
  bool ok;
  QString item = QInputDialog::getItem(this,
                                       tr("Add Field"),
                                       tr("Select CSV field to add"),
                                       _fieldList,
                                       0,
                                       FALSE,
                                       &ok,
                                       0);

  if (! ok || item.isEmpty())
    return;

  if (item == "Date")
  {
    item.clear();
    dateDialog(item);
    if (item.isEmpty())
      return;
    
    item = "Date=" + item;
  }

  if (item == "Time")
  {
    item.clear();
    timeDialog(item);
    if (item.isEmpty())
      return;
    
    item = "Time=" + item;
  }

  _ruleList->insertItem(_ruleList->currentRow(), item);

  ruleChanged();
}

void CSVRuleDialog::dateDialog (QString &item)
{
  bool ok;
  item = QInputDialog::getText(this,
                               tr("Add CSV Date Field"),
                               tr("Enter date mask format"),
                               QLineEdit::Normal,
                               QString("yyyy-MM-dd"),
                               &ok,
                               0);
}

void CSVRuleDialog::timeDialog (QString &item)
{
  bool ok;
  item = QInputDialog::getText(this,
                               tr("Add CSV Time Field"),
                               tr("Enter time mask format"),
                               QLineEdit::Normal,
                               QString("HH:mm:ss"),
                               &ok,
                               0);
}

void CSVRuleDialog::done ()
{
  saveRule();

  accept();
}

void CSVRuleDialog::clear ()
{
  _ruleList->clear();
}


