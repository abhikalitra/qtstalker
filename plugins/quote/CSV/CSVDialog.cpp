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
#include "../../../pics/newchart.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/delete.xpm"
#include "Setting.h"
#include "HelpWindow.h"
#include "SymbolDialog.h"
#include "CSVRuleDialog.h"
#include "Config.h"
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qinputdialog.h>
#include <qdir.h>

CSVDialog::CSVDialog (QWidget *p, QString &d, QString &lp) : QTabDialog (p, "CSVDialog", TRUE)
{
  helpFile = d;
  lastPath = lp;
  
  Config config;
  ruleDir = config.getData(Config::QuotePluginStorage) + "/CSV";
  QDir dir;
  if (! dir.exists(ruleDir))
  {
    if (! dir.mkdir(ruleDir, TRUE))
      qDebug("CSVDialog::could not create storage directory %s", ruleDir.latin1());
  }
  
  createMainPage();
  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
  
//  resize(325, 325);
  
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
  
  toolbar = new Toolbar(w, 30, 30, FALSE);
  vbox->addWidget(toolbar);

  QString s = "new";
  QString s2 = tr("New Rule");
  toolbar->addButton(s, newchart, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(newRule()));
  
  s = "edit";
  s2 = tr("Edit Rule");
  toolbar->addButton(s, edit, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(editRule()));
  
  s = "delete";
  s2 = tr("Delete Rule");
  toolbar->addButton(s, deleteitem, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(deleteRule()));
  
  vbox->addSpacing(10);
  
  QGridLayout *grid = new QGridLayout(vbox, 4, 1);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Rule:"), w);
  grid->addWidget(label, 0, 0);

  ruleCombo = new QComboBox(w);
  grid->addWidget(ruleCombo, 0, 1);
  
  label = new QLabel(tr("Input:"), w);
  grid->addWidget(label, 1, 0);
  
  QStringList l;
  file = new FileButton(w, l, lastPath);
  grid->addWidget(file, 1, 1);

  label = new QLabel(tr("Symbol:"), w);
  grid->addWidget(label, 2, 0);
  
  symbol = new QLineEdit(w);
  grid->addWidget(symbol, 2, 1);
  
  label = new QLabel(tr("Auto Reload:"), w);
  grid->addWidget(label, 3, 0);
  
  minutes = new QSpinBox(w);
  minutes->setMinValue(0);
  minutes->setMaxValue(99);
  minutes->setLineStep(0);
  grid->addWidget(minutes, 3, 1);
  
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

void CSVDialog::newRule ()
{
  bool ok = FALSE;
  QString s = QInputDialog::getText(tr("New Rule"),
  				    tr("Enter new rule name."),
				    QLineEdit::Normal,
				    tr("NewRule"),
				    &ok,
				    this);
  if ((! ok) || (s.isNull()))
    return;
    
  // remove any bad chars
  int loop;
  QString name;
  for (loop = 0; loop < (int) s.length(); loop++)
  {
    QChar c = s.at(loop);
    if (c.isLetterOrNumber())
      name.append(c);
  }
    
  QDir dir;
  s = ruleDir + "/" + name;
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Warning"), tr("This rule already exists."));
    return;
  }
    
  CSVRuleDialog *dialog = new CSVRuleDialog(this, s);
          
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    updateRules();
  }
  
  delete dialog;
}

void CSVDialog::editRule ()
{
  QString s("*");
  SymbolDialog *dialog = new SymbolDialog(this,
  				          ruleDir,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Rule To Edit"));

  int rc = dialog->exec();

  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  QStringList l = dialog->selectedFiles();
  delete dialog;
  if (! l.count())
    return;

  CSVRuleDialog *rdialog = new CSVRuleDialog(this, l[0]);
          
  rc = rdialog->exec();
  
  delete rdialog;
}

void CSVDialog::deleteRule ()
{
  QString s("*");
  SymbolDialog *dialog = new SymbolDialog(this,
  				          ruleDir,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Rules To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure you want to delete this rule?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QStringList l = dialog->selectedFiles();
    delete dialog;
    if (! l.count())
      return;
    
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
      dir.remove(l[loop]);
    
    updateRules();
  }
  else
    delete dialog;
}

void CSVDialog::dateRangeChanged (bool d)
{
  sdate->setEnabled(d);
  edate->setEnabled(d);
}

void CSVDialog::getFiles (QStringList &l)
{
  l.clear();
  file->getFile(l);
}

void CSVDialog::setFiles (QStringList &l)
{
  file->setFile(l);
}

QString CSVDialog::getSymbol ()
{
  return symbol->text();
}

void CSVDialog::setSymbol (QString &d)
{
  symbol->setText(d);
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
  QString current = ruleCombo->currentText();
  
  ruleCombo->clear();
  
  QStringList l;
  QDir dir(ruleDir);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (! fi.isDir())
      l.append(dir[loop]);
  }
  ruleCombo->insertStringList(l, -1);

  ruleCombo->setCurrentItem(l.findIndex(current));
}

void CSVDialog::setRuleName (QString &d)
{
  if (d.length())
    ruleCombo->setCurrentText(d);
}

QString CSVDialog::getRuleName ()
{
  return ruleCombo->currentText();
}

void CSVDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
  reject();
}

void CSVDialog::setReloadInterval (int d)
{
  minutes->setValue(d);
}

int CSVDialog::getReloadInterval ()
{
  return minutes->value();
}

