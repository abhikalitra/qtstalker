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
#include "Database.h"

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
#include <QGroupBox>



CSVRuleDialog::CSVRuleDialog (QWidget *w, QString &d) : QDialog (w, 0)
{
  rule = d;

  createRulePage();
  loadRule();

  QString s = tr("Editing CSV Rule: ") + rule;  
  setWindowTitle(s);
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

  int col = 0;
  int row = 0;
  
  QLabel *label = new QLabel(tr("Delimiter"));
  grid->addWidget(label, row, col++);
  
  delimiter = new QLineEdit;
  delimiter->setText(",");
  grid->addWidget(delimiter, row++, col--);

  label = new QLabel(tr("Date Format"));
  grid->addWidget(label, row, col++);
  
  dateFormat = new QLineEdit;
  grid->addWidget(dateFormat, row++, col--);

  label = new QLabel(tr("Time Format"));
  grid->addWidget(label, row, col++);
  
  timeFormat = new QLineEdit;
  grid->addWidget(timeFormat, row++, col--);

  label = new QLabel(tr("Time Interval"));
  grid->addWidget(label, row, col++);

  seconds = new QSpinBox;
  seconds->setRange(0, 9999);
  grid->addWidget(seconds, row++, col--);

  label = new QLabel(tr("CSV Files"));
  grid->addWidget(label, row, col++);

  fileButton = new QPushButton;
  fileButton->setText("0 " + tr("Files"));
  connect(fileButton, SIGNAL(clicked()), this, SLOT(importFileDialog()));
  grid->addWidget(fileButton, row++, col--);

  useFileName = new QCheckBox;
  useFileName->setText(tr("Use File Name For Symbol"));
  vbox->addWidget(useFileName);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(5);
  hbox->setMargin(0);
  vbox->addLayout(hbox);

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Fields"));
  hbox->addWidget(gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);
  
  fieldList = new QListWidget;
  fieldList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QStringList tl;
  CSVRule r;
  r.getFieldList(tl);
  fieldList->addItems(tl);
  connect(fieldList, SIGNAL(itemSelectionChanged()), this, SLOT(fieldListSelected()));
  tvbox->addWidget(fieldList);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  hbox->addLayout(tvbox);
  
  insertButton = new QPushButton;
  insertButton->setText(tr("Insert Field->>"));
  QObject::connect(insertButton, SIGNAL(clicked()), this, SLOT(insertField()));
  tvbox->addWidget(insertButton);
  
  customButton = new QPushButton;
  customButton->setText(tr("Custom Field->>"));
//  connect(customButton, SIGNAL(clicked()), this, SLOT(customDialog()));
  customButton->setEnabled(FALSE);
  tvbox->addWidget(customButton);
  
  deleteButton = new QPushButton;
  deleteButton->setText(tr("<<-Delete Field"));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteField()));
  tvbox->addWidget(deleteButton);

  tvbox->addStretch();
  
  gbox = new QGroupBox;
  gbox->setTitle(tr("Rule Format"));
  hbox->addWidget(gbox);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);
  
  ruleList = new QListWidget;
  connect(ruleList, SIGNAL(itemSelectionChanged()), this, SLOT(ruleFieldSelected()));
  tvbox->addWidget(ruleList);

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

  if (format.contains("Time") && timeFormat->text().isEmpty())
  {
    QMessageBox::information(this, tr("Error"), tr("Time format missing."));
    return;
  }

  CSVRule r;
  r.setName(rule);
  r.setFormat(format);

  QString s = delimiter->text();
  r.setDelimiter(s);
 
  r.setFileNameSymbol(useFileName->isChecked());

  s = dateFormat->text();
  r.setDateFormat(s);

  s = seconds->text();
  r.setInterval(s);

  r.setFileList(fileList);

  s = timeFormat->text();
  r.setTimeFormat(s);

  Database db;
  db.saveRule(r);

  accept();
}

void CSVRuleDialog::loadRule ()
{
  CSVRule r;
  r.setName(rule);

  Database db;
  db.loadRule(r);

  QString s;
  r.getFormat(s);
  if (s.length())
  {
    QStringList l = s.split(",");
    ruleList->clear();
    ruleList->addItems(l);
  }
    
  r.getDelimiter(s);
  delimiter->setText(s);

  useFileName->setChecked(r.getFileNameSymbol());

  r.getDateFormat(s);
  dateFormat->setText(s);

  r.getInterval(s);
  seconds->setValue(s.toInt());

  r.getFileList(fileList);
  fileButton->setText(QString::number(fileList.count()) + " " + tr("Files"));

  r.getTimeFormat(s);
  timeFormat->setText(s);
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
  QString s = QDir::homePath();
  if (fileList.count())
  {
    QFileInfo fi(fileList[0]);
    s = fi.absolutePath();
  }
  
  fileList = QFileDialog::getOpenFileNames(this,
					   QString(tr("Select CSV Files To Import")),
					   s,
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

