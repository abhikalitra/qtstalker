/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "CSVDialog.h"
#include "../../../src/newchart.xpm"
#include "../../../src/insert.xpm"
#include "../../../src/edit.xpm"
#include "../../../src/delete.xpm"
#include "../../../src/filesave.xpm"
#include "Setting.h"
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qinputdialog.h>
#include <qsettings.h>

CSVDialog::CSVDialog () : QTabDialog (0, "CSVDialog", TRUE)
{
  createMainPage();
  createRulePage();
  setOkButton();
  setCancelButton();
  resize(325, 325);
  updateRules();
}

CSVDialog::~CSVDialog ()
{
}

void CSVDialog::createMainPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  QGridLayout *grid = new QGridLayout(vbox, 3, 1);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Input:"), w);
  grid->addWidget(label, 0, 0);
  
  file = new FileButton(w);
  grid->addWidget(file, 0, 1);

  label = new QLabel(tr("Symbol:"), w);
  grid->addWidget(label, 1, 0);
  
  symbol = new QLineEdit(w);
  grid->addWidget(symbol, 1, 1);
  
  label = new QLabel(tr("Rule:"), w);
  grid->addWidget(label, 2, 0);

  ruleCombo = new QComboBox(w);
  grid->addWidget(ruleCombo, 2, 1);
  
  vbox->addSpacing(30);
  
  dateRange = new QCheckBox(tr("Select Date Range"), w);
  QObject::connect(dateRange, SIGNAL(toggled(bool)), this, SLOT(dateRangeChanged(bool)));
  vbox->addWidget(dateRange);
  
  grid = new QGridLayout(vbox, 3, 1);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  label = new QLabel(tr("Date Start:"), w);
  grid->addWidget(label, 0, 0);
  
  sdate = new QDateEdit(QDate::currentDate(), w);
  sdate->setAutoAdvance(TRUE);
  sdate->setOrder(QDateEdit::YMD);
  grid->addWidget(sdate, 0, 1);
  
  label = new QLabel(tr("Date End:"), w);
  grid->addWidget(label, 1, 0);
  
  edate = new QDateEdit(QDate::currentDate(), w);
  edate->setAutoAdvance(TRUE);
  edate->setOrder(QDateEdit::YMD);
  grid->addWidget(edate, 1, 1);
  
  addTab(w, tr("General"));
}

void CSVDialog::createRulePage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  ruleToolbar = new Toolbar(w, 30, 30);
  vbox->addWidget(ruleToolbar);
  
  ruleToolbar->addButton("new", newchart, tr("New Rule"));
  QObject::connect(ruleToolbar->getButton("new"), SIGNAL(clicked()), this, SLOT(newRule()));
  
  ruleToolbar->addButton("edit", edit, tr("Edit Rule"));
  QObject::connect(ruleToolbar->getButton("edit"), SIGNAL(clicked()), this, SLOT(editRule()));
  
  ruleToolbar->addButton("delete", deleteitem, tr("Delete Rule"));
  QObject::connect(ruleToolbar->getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteRule()));
  ruleToolbar->setButtonStatus("delete", FALSE);
  
  ruleToolbar->addButton("save", filesave, tr("Save Rule"));
  QObject::connect(ruleToolbar->getButton("save"), SIGNAL(clicked()), this, SLOT(saveRule()));
  ruleToolbar->setButtonStatus("save", FALSE);
  
  ruleToolbar->addButton("insert", insert, tr("Insert Field"));
  QObject::connect(ruleToolbar->getButton("insert"), SIGNAL(clicked()), this, SLOT(insertField()));
  ruleToolbar->setButtonStatus("insert", FALSE);
  
  ruleToolbar->addButton("deleteitem", deleteitem, tr("Delete Field"));
  QObject::connect(ruleToolbar->getButton("deleteitem"), SIGNAL(clicked()), this, SLOT(deleteField()));
  ruleToolbar->setButtonStatus("deleteitem", FALSE);
  
  vbox->addSpacing(10);
  
  QGridLayout *grid = new QGridLayout(vbox, 5, 2);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Rule:"), w);
  grid->addWidget(label, 0, 0);
  
  ruleName = new QLineEdit(w);
  grid->addWidget(ruleName, 0, 1);
  ruleName->setReadOnly(TRUE);
  
  label = new QLabel(tr("Chart Type:"), w);
  grid->addWidget(label, 1, 0);
  
  type = new QComboBox(w);
  type->insertItem(tr(tr("Stock")), -1);
  type->insertItem(tr(tr("Futures")), -1);
  grid->addWidget(type, 1, 1);
  
  label = new QLabel(tr("Delimiter:"), w);
  grid->addWidget(label, 2, 0);
  
  delimiter = new QComboBox(w);
  delimiter->insertItem(tr("Comma"), -1);
  delimiter->insertItem(tr("Tab"), -1);
  delimiter->insertItem(tr("Space"), -1);
  grid->addWidget(delimiter, 2, 1);

  label = new QLabel(tr("Fields:"), w);
  grid->addWidget(label, 3, 0);
  
  fieldCombo = new QComboBox(w);
  fieldCombo->insertItem(tr("Date:YYYYMMDD"), -1);
  fieldCombo->insertItem(tr("Date:YYMMDD"), -1);
  fieldCombo->insertItem(tr("Date:MMDDYY"), -1);
  fieldCombo->insertItem(tr("Date:MMDDYYYY"), -1);
  fieldCombo->insertItem(tr("Date:DDMMYYYY"), -1);
  fieldCombo->insertItem(tr("Time"), -1);
  fieldCombo->insertItem(tr("Symbol"), -1);
  fieldCombo->insertItem(tr("Open"), -1);
  fieldCombo->insertItem(tr("High"), -1);
  fieldCombo->insertItem(tr("Low"), -1);
  fieldCombo->insertItem(tr("Close"), -1);
  fieldCombo->insertItem(tr("Volume"), -1);
  fieldCombo->insertItem(tr("OI"), -1);
  fieldCombo->insertItem(tr("Ignore"), -1);
  grid->addWidget(fieldCombo, 3, 1);
  
  vbox->addSpacing(10);

  label = new QLabel(tr("Rule Contents:"), w);
  vbox->addWidget(label);
  
  ruleList = new QListBox(w);
  QObject::connect(ruleList, SIGNAL(highlighted(int)), this, SLOT(fieldSelected(int)));
  vbox->addWidget(ruleList);
  
  addTab(w, tr("Rule"));
}

void CSVDialog::newRule ()
{
  bool ok = FALSE;
  QString name = QInputDialog::getText(tr("New Rule"),
  				       tr("Enter new rule name."),
				       QLineEdit::Normal,
				       tr("New Rule"),
				       &ok,
				       this);
  if ((! ok) || (name.isNull()))
    return;
    
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");
  QStringList l = QStringList::split(",", settings.readEntry("/RuleList"), FALSE);
  
  if (l.findIndex(name) != -1)
  {
    QMessageBox::information(this, tr("Warning"), tr("This rule already exists."));
    settings.endGroup();
    return;
  }
  
  l.append(name);
  settings.writeEntry("/RuleList", l.join(","));
  settings.endGroup();
  
  ruleName->setText(name);
  ruleList->clear();
  
  saveRule();
  
  ruleToolbar->setButtonStatus("delete", TRUE);
  ruleToolbar->setButtonStatus("save", FALSE);
  ruleToolbar->setButtonStatus("insert", TRUE);
  
  updateRules();
}

void CSVDialog::editRule ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");
  QStringList l = QStringList::split(",", settings.readEntry("/RuleList"), FALSE);

  bool ok = FALSE;
  QString name = QInputDialog::getItem(tr("Edit Rule"),
  				       tr("Select rule to edit."),
				       l,
				       0,
				       FALSE,
				       &ok,
				       this);
  if (! ok)
  {
    settings.endGroup();
    return;
  }
  
  Setting *set = new Setting;
  QString s = "/Rule_" + name;
  set->parse(settings.readEntry(s));
    
  ruleName->setText(name);
  
  delimiter->setCurrentText(set->getData("Delimiter"));
  
  type->setCurrentText(set->getData("Type"));
  
  l = QStringList::split(",", set->getData("Rule"));
  ruleList->insertStringList(l, -1);
  
  ruleToolbar->setButtonStatus("delete", TRUE);
  ruleToolbar->setButtonStatus("save", FALSE);
  ruleToolbar->setButtonStatus("insert", TRUE);

  settings.endGroup();
  delete set;
}

void CSVDialog::deleteRule ()
{
  int rc = QMessageBox::warning(this,
  				tr("Warning"),
				tr("Are you sure you want to delete this rule?"),
				QMessageBox::Yes,
				QMessageBox::No,
				QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");
  QString s = "/Rule_" + ruleName->text();
  settings.removeEntry(s);
  
  QStringList l = QStringList::split(",", settings.readEntry("/RuleList"), FALSE);
  l.remove(ruleName->text());
  settings.writeEntry("/RuleList", l.join(","));
  
  settings.endGroup();
  
  ruleName->clear();
  ruleList->clear();
  
  ruleToolbar->setButtonStatus("delete", FALSE);
  ruleToolbar->setButtonStatus("save", FALSE);
  ruleToolbar->setButtonStatus("insert", FALSE);
  ruleToolbar->setButtonStatus("deleteitem", FALSE);
  
  updateRules();
}

void CSVDialog::saveRule ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");
  QString key = "/Rule_" + ruleName->text();
  
  Setting *set = new Setting;
  set->setData("Delimiter", delimiter->currentText());
  set->setData("Type", type->currentText());
  int loop;
  QStringList l;
  for (loop = 0; loop < (int) ruleList->count(); loop++)
    l.append(ruleList->text(loop));
  set->setData("Rule", l.join(","));
  
  settings.writeEntry(key, set->getString());
  
  ruleToolbar->setButtonStatus("save", FALSE);
  
  settings.endGroup();
  delete set;
}

void CSVDialog::insertField ()
{
  ruleList->insertItem(fieldCombo->currentText(), ruleList->currentItem());
  ruleToolbar->setButtonStatus("save", TRUE);
}

void CSVDialog::deleteField ()
{
  if (ruleList->currentItem() != -1)
  {
    ruleList->removeItem(ruleList->currentItem());
    ruleToolbar->setButtonStatus("save", TRUE);
  }    
}

void CSVDialog::fieldSelected (int)
{
  if (ruleList->currentItem() != -1)
  {
    ruleToolbar->setButtonStatus("deleteitem", TRUE);
  }
  else
  {
    ruleToolbar->setButtonStatus("deleteitem", FALSE);
  }
}

void CSVDialog::dateRangeChanged (bool d)
{
  sdate->setEnabled(d);
  edate->setEnabled(d);
}

QStringList CSVDialog::getFiles ()
{
  return file->getFile();
}

QString CSVDialog::getSymbol ()
{
  return symbol->text();
}

void CSVDialog::setStartDate (QDateTime dt)
{
  sdate->setDate(dt.date());
}

QDateTime CSVDialog::getStartDate ()
{
  QDateTime dt;
  dt.setDate(sdate->date());
  return dt;
}

void CSVDialog::setEndDate (QDateTime dt)
{
  edate->setDate(dt.date());
}

QDateTime CSVDialog::getEndDate ()
{
  QDateTime dt;
  dt.setDate(edate->date());
  return dt;
}

void CSVDialog::setDateRange (bool d)
{
  dateRange->setChecked(d);
  dateRangeChanged(d);
}

bool CSVDialog::getDateRange ()
{
  return dateRange->isChecked();
}

void CSVDialog::updateRules ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");
  QStringList l = QStringList::split(",", settings.readEntry("/RuleList"), FALSE);
  if (l.count())
    ruleCombo->insertStringList(l, -1);
  settings.endGroup();
}

void CSVDialog::setRuleName (QString d)
{
  if (d.length())
    ruleCombo->setCurrentText(d);
}

QString CSVDialog::getRuleName ()
{
  return ruleCombo->currentText();
}



