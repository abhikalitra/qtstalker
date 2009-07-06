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
#include "CSVRule.h"
#include <QDir>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QToolTip>
#include <QInputDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtSql>



CSVRuleDialog::CSVRuleDialog (QWidget *w, QString &d) : QDialog (w, 0)
{
  rule = d;

  createRulePage();
  loadRule();

  QString s = tr("Editing CSV Rule: ") + rule;  
  setWindowTitle(s);
}

CSVRuleDialog::~CSVRuleDialog ()
{
}

void CSVRuleDialog::createRulePage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(10);
  vbox->setSpacing(10);
  setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
//  grid->setColumnStretch(1, 1);
  grid->setColumnStretch(3, 1);
  vbox->addLayout(grid);
  
  QLabel *label = new QLabel(tr("Delimiter"));
  grid->addWidget(label, 0, 0);
  
  delimiter = new QLineEdit;
  delimiter->setText(",");
  grid->addWidget(delimiter, 0, 1);

  label = new QLabel(tr("Date Format"));
  grid->addWidget(label, 1, 0);
  
  dateFormat = new QLineEdit;
  grid->addWidget(dateFormat, 1, 1);

  label = new QLabel(tr("Time Interval"));
  grid->addWidget(label, 2, 0);

  seconds = new QSpinBox;
  seconds->setRange(0, 9999);
  grid->addWidget(seconds, 2, 1);

  label = new QLabel(tr("Import Files"));
  grid->addWidget(label, 3, 0);

  fileButton = new QPushButton;
  fileButton->setText("0 " + tr("Files"));
  QObject::connect(fileButton, SIGNAL(clicked()), this, SLOT(importFileDialog()));
  grid->addWidget(fileButton, 3, 1);

  useFileName = new QCheckBox;
  useFileName->setText(tr("Use File Name For Symbol"));
  vbox->addWidget(useFileName);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(5);
  vbox->addLayout(hbox);
  
  fieldList = new QListWidget;
  fieldList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QStringList tl;
  CSVRule r;
  r.getFieldList(tl);
  fieldList->addItems(tl);
  QObject::connect(fieldList, SIGNAL(itemSelectionChanged()), this, SLOT(fieldListSelected()));
  hbox->addWidget(fieldList);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  hbox->addLayout(tvbox);
  
  insertButton = new QPushButton;
  insertButton->setText(tr("Insert Field->>"));
  QObject::connect(insertButton, SIGNAL(clicked()), this, SLOT(insertField()));
  tvbox->addWidget(insertButton);
  
  customButton = new QPushButton;
  customButton->setText(tr("Custom Field->>"));
  QObject::connect(customButton, SIGNAL(clicked()), this, SLOT(customDialog()));
  tvbox->addWidget(customButton);
  
  deleteButton = new QPushButton;
  deleteButton->setText(tr("<<-Delete Field"));
  QObject::connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteField()));
  tvbox->addWidget(deleteButton);

  tvbox->addStretch();
  
  ruleList = new QListWidget;
  QObject::connect(ruleList, SIGNAL(itemSelectionChanged()), this, SLOT(ruleFieldSelected()));
  hbox->addWidget(ruleList);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveRule()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(buttonBox);
}

void CSVRuleDialog::saveRule ()
{
  int loop;
  QStringList l;
  for (loop = 0; loop < (int) ruleList->count(); loop++)
    l.append(ruleList->item(loop)->text());
  QString format = l.join(",");

  if (! format.contains("Date"))
  {
    QMessageBox::information(this, tr("Error"), tr("Rule missing a date field."));
    return;
  }

  if (dateFormat->text().isEmpty())
  {
    QMessageBox::information(this, tr("Error"), tr("Date format missing."));
    return;
  }

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO importRules VALUES (";
  s.append("'" + rule + "'");
  s.append(",'" + format + "'");
  s.append(",'" + delimiter->text() + "'");
  s.append("," + QString::number(useFileName->isChecked()));
  s.append(",'" + dateFormat->text() + "'");
  s.append("," + seconds->text());
  s.append(",'" + fileList.join("|") + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CSVRuleDialog::saveRule: " << q.lastError().text();
    return;
  }

  accept();
}

void CSVRuleDialog::loadRule ()
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM importRules WHERE name='" + rule + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CSVRuleDialog::loadRule: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    s = q.value(1).toString();
    if (s.length())
    {
      QStringList l = s.split(",");
      ruleList->clear();
      ruleList->addItems(l);
    }
    
    s = q.value(2).toString();
    delimiter->setText(s);

    useFileName->setChecked(q.value(3).toBool());

    s = q.value(4).toString();
    dateFormat->setText(s);

    seconds->setValue(q.value(5).toInt());

    fileList = q.value(6).toString().split("|");
  }
}

void CSVRuleDialog::insertField ()
{
  QList<QListWidgetItem *> l = fieldList->selectedItems();
  for (int loop = 0; loop < l.size(); ++loop)
    ruleList->addItem(l.at(loop)->text());
}

void CSVRuleDialog::deleteField ()
{
  QListWidgetItem *item = ruleList->currentItem();
  if (item)
    delete item;
}

void CSVRuleDialog::ruleFieldSelected ()
{
  QListWidgetItem *item = ruleList->currentItem();
  if (item)
    deleteButton->setEnabled(TRUE);
  else
    deleteButton->setEnabled(FALSE);
}

void CSVRuleDialog::fieldListSelected ()
{
  QListWidgetItem *item = fieldList->currentItem();
  if (item)
    insertButton->setEnabled(TRUE);
  else
    insertButton->setEnabled(FALSE);
}


void CSVRuleDialog::importFileDialog ()
{
  fileList = QFileDialog::getOpenFileNames(this,
					   QString(tr("Select CSV Files To Import")),
					   QString(QDir::homePath()),
					   QString("*"),
					   0,
					   0);

  fileButton->setText(QString::number(fileList.count()) + tr(" Files"));
}

void CSVRuleDialog::customDialog ()
{
  bool ok;
  QString s = QInputDialog::getText(this, tr("Custom Field"), tr("Enter field name"), QLineEdit::Normal, QString(), &ok, 0);
  if (s.isEmpty())
    return;

  if (s.contains(","))
  {
    QMessageBox::information(this, tr("Error"), tr("No commas allowed."));
    return;
  }

  ruleList->addItem(s);
}

