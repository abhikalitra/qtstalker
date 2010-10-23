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
#include "CSVConfig.h"
#include "Globals.h"

#include "../pics/delete.xpm"
#include "../pics/search.xpm"

#include <QLayout>
#include <QLabel>
#include <QInputDialog>
#include <QDir>
#include <QList>
#include <QDebug>
#include <QFormLayout>

CSVRuleDialog::CSVRuleDialog (QString name)
{
  _name = name;

  _fields << "Exchange" << "Symbol" << "Open" << "High" << "Low" << "Close";
  _fields << "Volume" << "OI" << "Name" << "Ignore";
  
  QString s = "Qtstalker" + g_session + ": CSV " + tr("Rule") + " - " + _name;
  setWindowTitle(s);

  createMainPage();
  createRulePage();

  loadRule();

  ruleSelectionChanged();
  fieldSelectionChanged();

  loadSettings();

  _saveFlag = FALSE;
  _okButton->setEnabled(FALSE);
}

CSVRuleDialog::~CSVRuleDialog ()
{
  saveSettings();
}

void CSVRuleDialog::loadSettings ()
{
  CSVConfig config;

  // restore the size of the window
  QSize sz;
  config.getData(CSVConfig::RuleDialogSize, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(CSVConfig::RuleDialogPosition, p);
  if (! p.isNull())
    move(p);
}

void CSVRuleDialog::saveSettings ()
{
  CSVConfig config;
  config.transaction();

  // save app size and position
  QSize sz = size();
  config.setData(CSVConfig::RuleDialogSize, sz);

  QPoint pt = pos();
  config.setData(CSVConfig::RuleDialogPosition, pt);

  config.commit();
}

void CSVRuleDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // type parm
  Config config;
  QStringList l;
  l << "S" << "F";
  
  _type = new QComboBox;
  _type->addItems(l);
  _type->setToolTip(tr("S = Stock, F = Futures"));
  connect(_type, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Type"), _type);

  // delimiter parm
  _delimeter = new QComboBox;
  _delimeter->addItem(QString(","));
  _delimeter->addItem(QString(";"));
  connect(_delimeter, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Delimiter"), _delimeter);

  // CSV file parm
  QString s = QDir::homePath();
  _file = new FileButton(this, s);
  connect(_file, SIGNAL(signalFileChanged()), this, SLOT(ruleChanged()));
  form->addRow(tr("CSV File"), _file);

  // exchange parm
  ExchangeDataBase edb;
  l.clear();
  edb.getExchanges(l);
  
  _exchange = new QComboBox;
  _exchange->addItems(l);
  connect(_exchange, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Exchange Code"), _exchange);

  // exchange search
  QPushButton *b = new QPushButton;
  b->setText(QString("..."));
  b->setToolTip(tr("Search for exchange code"));
  b->setIcon(QIcon(search_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(searchExchange()));
  form->addRow(tr("Exchange Search"), b);

  // filename parm
  _fileSymbol = new QCheckBox;
  _fileSymbol->setToolTip(tr("Use filename as symbol"));
  connect(_fileSymbol, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Filename As Symbol"), _fileSymbol);

  // remove suffix
  _removeSuffix = new QCheckBox;
  _removeSuffix->setToolTip(tr("Try to remove suffix from file name"));
  connect(_removeSuffix, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Remove Suffix"), _removeSuffix);

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
  _ruleList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_ruleList, SIGNAL(itemSelectionChanged()), this, SLOT(ruleSelectionChanged()));
  hbox->addWidget(_ruleList);

  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setMargin(0);
  bbox->setSpacing(2);
  hbox->addLayout(bbox);

  _insertButton = new QPushButton;
  connect(_insertButton, SIGNAL(clicked()), this, SLOT(insertClicked()));
  _insertButton->setText(tr("Insert"));
  bbox->addWidget(_insertButton);

  _insertDateButton = new QPushButton;
  connect(_insertDateButton, SIGNAL(clicked()), this, SLOT(dateDialog()));
  _insertDateButton->setText(tr("Insert Date"));
  bbox->addWidget(_insertDateButton);

  _insertTimeButton = new QPushButton;
  connect(_insertTimeButton, SIGNAL(clicked()), this, SLOT(timeDialog()));
  _insertTimeButton->setText(tr("Insert Time"));
  bbox->addWidget(_insertTimeButton);

  _deleteButton = new QPushButton;
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
  _deleteButton->setText(tr("Delete"));
  _deleteButton->setIcon(QIcon(delete_xpm));
  bbox->addWidget(_deleteButton);

  bbox->addStretch();

  _fieldList = new QListWidget;
  _fieldList->addItems(_fields);
  _fieldList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_fieldList, SIGNAL(itemSelectionChanged()), this, SLOT(fieldSelectionChanged()));
  hbox->addWidget(_fieldList);

  _tabs->addTab(w, tr("Rule Format"));
}

void CSVRuleDialog::saveRule ()
{
  if (! _saveFlag)
    return;

  CSVRule rule;
  rule.setName(_name);
  rule.setType(_type->currentText());
  rule.setDelimiter(_delimeter->currentText());
  rule.setFile(_file->getFile());

  QStringList l;
  int loop = 0;
  for (; loop < _ruleList->count(); loop++)
    l.append(_ruleList->item(loop)->text());
  rule.setRule(l.join(","));

  rule.setFileSymbol(_fileSymbol->isChecked());
  rule.setExchange(_exchange->currentText());
  rule.setRemoveSuffix(_removeSuffix->isChecked());

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
  {
    ruleChanged();
    return;
  }

  _type->setCurrentIndex(_type->findText(rule.type(), Qt::MatchExactly));
  _delimeter->setCurrentIndex(_delimeter->findText(rule.delimiter(), Qt::MatchExactly));
  _file->setFile(rule.file());

  QStringList l = rule.rule().split(",");
  int loop = 0;
  for (; loop < l.count(); loop++)
    new QListWidgetItem(l[loop], _ruleList, 0);

  _fileSymbol->setChecked(rule.fileSymbol());
  _exchange->setCurrentIndex(_exchange->findText(rule.exchange(), Qt::MatchExactly));
  _removeSuffix->setChecked(rule.removeSuffix());
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
  _okButton->setEnabled(_saveFlag);
}

void CSVRuleDialog::insertClicked ()
{
  QList<QListWidgetItem *> sel = _fieldList->selectedItems();
  if (! sel.count())
    return;

  QStringList l;
  int loop = 0;
  for (; loop < sel.count(); loop++)
  {
    if (sel.at(loop)->text().length())
      l.append(sel.at(loop)->text());
  }
  
  _ruleList->insertItems(_ruleList->currentRow() + 1, l);

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
  if (! item.length())
    return;

  QString s = "Date=" + item;
  _ruleList->insertItem(_ruleList->currentRow() + 1, s);

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
  _ruleList->insertItem(_ruleList->currentRow() + 1, s);

  ruleChanged();
}

void CSVRuleDialog::confirmYes ()
{
  done();
}

void CSVRuleDialog::cancel ()
{
  if (_saveFlag)
  {
    setMessage(tr("Settings modified. Save changes?"));
    setConfirm();
    _okButton->setEnabled(FALSE);
    _cancelButton->setEnabled(FALSE);
  }
  else
    reject();
}

void CSVRuleDialog::done ()
{
  if (_saveFlag)
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
  dialog->show();
}

void CSVRuleDialog::setExchangeCode (QString d)
{
  _exchange->setCurrentIndex(_exchange->findText(d, Qt::MatchExactly));
  
  ruleChanged();
}

void CSVRuleDialog::ruleSelectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sel = _ruleList->selectedItems();
  if (sel.count())
    status = 1;
  
  _deleteButton->setEnabled(status);
}

void CSVRuleDialog::fieldSelectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sel = _fieldList->selectedItems();
  if (sel.count())
    status = 1;

  _insertButton->setEnabled(status);
}
