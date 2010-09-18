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
#include "ExchangeSearchDialog.h"

#include <QLayout>
#include <QLabel>
#include <QInputDialog>
#include <QDir>
#include <QList>
#include <QDebug>
#include <QToolButton>

CSVRuleDialog::CSVRuleDialog (QString &name) : Dialog (Dialog::_Dialog, 0)
{
  _saveFlag = FALSE;
  _name = name;

  _fieldList << "Exchange" << "Symbol" << "Date" << "Time" << "Open" << "High" << "Low" << "Close";
  _fieldList << "Volume" << "OI" << "Name" << "Ignore";
  
  QString s = tr("Qtstalker: CSV Editing Rule ") + _name;
  setWindowTitle(s);

  createMainPage();
  createRulePage();

  loadRule();

  selectionChanged();
}

void CSVRuleDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // type parm
  QLabel *label = new QLabel(tr("Type"));
  grid->addWidget(label, row, col++);

  Config config;
  QStringList l;
  l << "S" << "F";
  
  _type = new QComboBox;
  _type->addItems(l);
  _type->setToolTip(tr("S = Stock, F = Futures"));
  connect(_type, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_type, row++, col--);

  // delimiter parm
  label = new QLabel(tr("Delimiter"));
  grid->addWidget(label, row, col++);
  
  _delimeter = new QComboBox;
  _delimeter->addItem(QString(","));
  _delimeter->addItem(QString(";"));
  connect(_delimeter, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_delimeter, row++, col--);

  // CSV file parm
  label = new QLabel(tr("CSV File"));
  grid->addWidget(label, row, col++);

  QString s = QDir::homePath();
  _file = new FileButton(this, s);
  connect(_file, SIGNAL(signalFileChanged()), this, SLOT(ruleChanged()));
  grid->addWidget(_file, row++, col--);

  // exchange parm
  label = new QLabel(tr("Exchange Code"));
  grid->addWidget(label, row, col++);

  ExchangeDataBase edb;
  l.clear();
  edb.getExchanges(l);
  
  _exchange = new QComboBox;
  _exchange->addItems(l);
  connect(_exchange, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_exchange, row, col++);

  QToolButton *tb = new QToolButton;
  tb->setText(QString("..."));
  tb->setToolTip(tr("Search for exchange code"));
  connect(tb, SIGNAL(clicked()), this, SLOT(searchExchange()));
  grid->addWidget(tb, row++, col);

  // filename parm
  _fileSymbol = new QCheckBox(tr("Use filename as symbol"));
  connect(_fileSymbol, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  vbox->addWidget(_fileSymbol);

  // remove suffix
  _removeSuffix = new QCheckBox(tr("Try to remove suffix from file name"));
  vbox->addWidget(_removeSuffix);

  _tabs->addTab(w, tr("Settings"));
}

void CSVRuleDialog::createRulePage ()
{
  QWidget *w = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(5);
  hbox->setSpacing(2);
  w->setLayout(hbox);

  _ruleList = new QListWidget;
  connect(_ruleList, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  hbox->addWidget(_ruleList);

  QDialogButtonBox *bbox = new QDialogButtonBox;
  bbox->setOrientation(Qt::Vertical);
  hbox->addWidget(bbox);

  _insertButton = bbox->addButton(QDialogButtonBox::Apply);
  connect(_insertButton, SIGNAL(clicked()), this, SLOT(insertClicked()));
  _insertButton->setText(tr("Insert"));

  _deleteButton = bbox->addButton(QDialogButtonBox::Discard);
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
  _deleteButton->setText(tr("Delete"));

  _tabs->addTab(w, tr("Rule Format"));
}

void CSVRuleDialog::saveRule ()
{
  if (! _saveFlag)
    return;

  CSVRule rule;
  rule.name = _name;
  rule.type = _type->currentText();
  rule.delimeter = _delimeter->currentText();
  rule.file = _file->getFile();

  QStringList l;
  int loop = 0;
  for (; loop < _ruleList->count(); loop++)
    l.append(_ruleList->item(loop)->text());
  rule.rule = l.join(",");

  rule.fileSymbol = _fileSymbol->isChecked();
  rule.exchange = _exchange->currentText();
  rule.removeSuffix = _removeSuffix->isChecked();

  CSVDataBase db;
  db.setRule(rule);
  
  ruleChanged();
}

void CSVRuleDialog::loadRule ()
{
  clear();
  
  CSVRule rule;
  rule.name = _name;
  
  CSVDataBase db;
  if (db.getRule(rule))
    return;

  _type->setCurrentIndex(_type->findText(rule.type, Qt::MatchExactly));
  _delimeter->setCurrentIndex(_delimeter->findText(rule.delimeter, Qt::MatchExactly));
  _file->setFile(rule.file);

  QStringList l = rule.rule.split(",");
  int loop = 0;
  for (; loop < l.count(); loop++)
    new QListWidgetItem(l[loop], _ruleList, 0);

  _fileSymbol->setChecked(rule.fileSymbol);
  _exchange->setCurrentIndex(_exchange->findText(rule.exchange, Qt::MatchExactly));
  _removeSuffix->setChecked(rule.removeSuffix);
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

void CSVRuleDialog::insertClicked ()
{
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle(tr("Qtstalker: CSV Rule Insert Field"));
  dialog->setLabelText(tr("Select CSV field to insert"));
  dialog->setInputMode(QInputDialog::TextInput);
  dialog->setComboBoxEditable(FALSE);
  dialog->setComboBoxItems(_fieldList);
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(insertClicked2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void CSVRuleDialog::insertClicked2 (QString item)
{
  if (item.isEmpty())
    return;

  if (item == "Date")
  {
    dateDialog();
    return;
  }
  
  if (item == "Time")
  {
    timeDialog();
    return;
  }

  _ruleList->insertItem(_ruleList->currentRow(), item);

  ruleChanged();
}

void CSVRuleDialog::dateDialog ()
{
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle(tr("Qtstalker: CSV Rule Insert Date Field"));
  dialog->setLabelText(tr("Enter date mask format"));
  dialog->setInputMode(QInputDialog::TextInput);
  dialog->setTextValue(QString("yyyy-MM-dd"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(dateDialog2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void CSVRuleDialog::dateDialog2 (QString item)
{
  if (item.isEmpty())
    return;

  QString s = "Date=" + item;
  _ruleList->insertItem(_ruleList->currentRow(), s);

  ruleChanged();
}

void CSVRuleDialog::timeDialog ()
{
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle(tr("Qtstalker: CSV Rule Insert Time Field"));
  dialog->setLabelText(tr("Enter time mask format"));
  dialog->setInputMode(QInputDialog::TextInput);
  dialog->setTextValue(QString("HH:mm:ss"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(timeDialog2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void CSVRuleDialog::timeDialog2 (QString item)
{
  if (item.isEmpty())
    return;

  QString s = "Time=" + item;
  _ruleList->insertItem(_ruleList->currentRow(), s);

  ruleChanged();
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

void CSVRuleDialog::searchExchange ()
{
  ExchangeSearchDialog *dialog = new ExchangeSearchDialog;
  connect(dialog, SIGNAL(signalExchangeCode(QString)), this, SLOT(setExchangeCode(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void CSVRuleDialog::setExchangeCode (QString d)
{
  _exchange->setCurrentIndex(_exchange->findText(d, Qt::MatchExactly));
}

void CSVRuleDialog::selectionChanged ()
{
  QList<QListWidgetItem *> sel = _ruleList->selectedItems();
  if (! sel.count())
    _deleteButton->setEnabled(FALSE);
  else
    _deleteButton->setEnabled(TRUE);
}

