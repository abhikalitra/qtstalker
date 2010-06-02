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

#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QInputDialog>
#include <QPushButton>
#include <QDir>
#include <QGroupBox>
#include <QList>
#include <QDebug>

CSVRuleDialog::CSVRuleDialog (QString &name) : QDialog (0, 0)
{
  _saveFlag = FALSE;
  _name = name;
  
  createGUI();

  loadRule();

  QString s = tr("Editing CSV Rule: ") + _name;
  setWindowTitle(s);
}

CSVRuleDialog::~CSVRuleDialog ()
{
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
  
  _type = new QComboBox;
  _type->addItem(QString("Futures"));
  _type->addItem(QString("Stocks"));
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

  _fileSymbol = new QCheckBox(tr("Use filename as symbol name"));
  connect(_fileSymbol, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_fileSymbol, row++, col);

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("CSV Rule Format"));
  vbox->addWidget(gbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  gbox->setLayout(hbox);
  
  _ruleList = new QListWidget;
  connect(_ruleList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(deleteRuleField(QListWidgetItem *)));
  hbox->addWidget(_ruleList);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  QPushButton *b = new QPushButton;
  b->setText(tr("Exchange"));
  connect(b, SIGNAL(clicked()), this, SLOT(exchangeClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Symbol"));
  connect(b, SIGNAL(clicked()), this, SLOT(symbolClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Date"));
  connect(b, SIGNAL(clicked()), this, SLOT(dateClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Time"));
  connect(b, SIGNAL(clicked()), this, SLOT(timeClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Open"));
  connect(b, SIGNAL(clicked()), this, SLOT(openClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("High"));
  connect(b, SIGNAL(clicked()), this, SLOT(highClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Low"));
  connect(b, SIGNAL(clicked()), this, SLOT(lowClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Close"));
  connect(b, SIGNAL(clicked()), this, SLOT(closeClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Volume"));
  connect(b, SIGNAL(clicked()), this, SLOT(volumeClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("OI"));
  connect(b, SIGNAL(clicked()), this, SLOT(oiClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Ignore"));
  connect(b, SIGNAL(clicked()), this, SLOT(ignoreClicked()));
  tvbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("Name"));
  connect(b, SIGNAL(clicked()), this, SLOT(nameClicked()));
  tvbox->addWidget(b);

  tvbox->addStretch(1);

  _buttonBox = new QDialogButtonBox;
  vbox->addWidget(_buttonBox);

  b = new QPushButton(tr("&OK"));
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

  Setting set;
  QString k = "Name";
  QString d = _name;
  set.setData(k, d);

  k = "Plugin";
  d = _type->currentText();
  set.setData(k, d);

  k = "Delimeter";
  d = _delimeter->currentText();
  set.setData(k, d);

  k = "File";
  d = _file->getFile();
  set.setData(k, d);

  QStringList l;
  int loop = 0;
  for (; loop < _ruleList->count(); loop++)
    l.append(_ruleList->item(loop)->text());

  k = "Rule";
  d = l.join(",");
  set.setData(k, d);

  k = "FileSymbol";
  d = QString::number(_fileSymbol->isChecked());
  set.setData(k, d);

  CSVDataBase db;
  db.setRule(set);
  
  ruleChanged();
}

void CSVRuleDialog::loadRule ()
{
  _ruleList->clear();
  
  Setting set;
  QString k = "Name";
  QString d = _name;
  set.setData(k, d);
  
  CSVDataBase db;
  db.getRule(set);

  k = "Plugin";
  d.clear();
  set.getData(k, d);
  if (d.isEmpty())
    return;
  _type->setCurrentIndex(_type->findText(d, Qt::MatchExactly));

  k = "Delimeter";
  d.clear();
  set.getData(k, d);
  if (d.isEmpty())
    return;
  _delimeter->setCurrentIndex(_delimeter->findText(d, Qt::MatchExactly));

  k = "File";
  d.clear();
  set.getData(k, d);
  if (d.isEmpty())
    return;
  _file->setFile(d);

  k = "Rule";
  d.clear();
  set.getData(k, d);
  if (d.isEmpty())
    return;

  QStringList l = d.split(",");
  int loop = 0;
  for (; loop < l.count(); loop++)
    new QListWidgetItem(l[loop], _ruleList, 0);

  k = "FileSymbol";
  d.clear();
  set.getData(k, d);
  if (d.isEmpty())
    return;
  _fileSymbol->setChecked(d.toInt());
}

void CSVRuleDialog::deleteRuleField (QListWidgetItem *)
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

void CSVRuleDialog::symbolClicked ()
{
  new QListWidgetItem(QString("Symbol"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::dateClicked ()
{
  bool ok;
  QString date = QInputDialog::getText(this,
                                       tr("Add CSV Date Field"),
                                       tr("Enter date mask format"),
                                       QLineEdit::Normal,
                                       QString("yyyy-MM-dd"),
                                       &ok,
                                       0);
  if (! ok || date.isEmpty())
    return;
  
  new QListWidgetItem(QString("Date:" + date), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::timeClicked ()
{
  bool ok;
  QString time = QInputDialog::getText(this,
                                       tr("Add CSV Time Field"),
                                       tr("Enter time mask format"),
                                       QLineEdit::Normal,
                                       QString("HH:mm:ss"),
                                       &ok,
                                       0);
  if (! ok || time.isEmpty())
    return;

  new QListWidgetItem(QString("Time:" + time), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::openClicked ()
{
  new QListWidgetItem(QString("Open"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::highClicked ()
{
  new QListWidgetItem(QString("High"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::lowClicked ()
{
  new QListWidgetItem(QString("Low"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::closeClicked ()
{
  new QListWidgetItem(QString("Close"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::volumeClicked ()
{
  new QListWidgetItem(QString("Volume"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::oiClicked ()
{
  new QListWidgetItem(QString("OI"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::ignoreClicked ()
{
  new QListWidgetItem(QString("Ignore"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::nameClicked ()
{
  new QListWidgetItem(QString("Name"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::exchangeClicked ()
{
  new QListWidgetItem(QString("Exchange"), _ruleList, 0);
  ruleChanged();
}

void CSVRuleDialog::done ()
{
  saveRule();
  accept();
}


