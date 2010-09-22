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

#include "CSVDialog.h"
#include "CSVDataBase.h"
#include "CSVRuleDialog.h"
#include "CSVConfig.h"
#include "Dialog.h"

#include "../pics/edit.xpm"
#include "../pics/newchart.xpm"

#include <QLayout>
#include <QLabel>
#include <QInputDialog>
#include <QGroupBox>

CSVDialog::CSVDialog () : QDialog (0, 0)
{
  setWindowTitle(tr("Qtstalker: CSV"));
  
  createMainPage();
  
  loadSettings();

  _thread = new CSVThread(this);
  connect(_thread, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
  connect(_thread, SIGNAL(finished()), this, SLOT(done()));
}

void CSVDialog::createMainPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(10);
  setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // rule parm
  QLabel *label = new QLabel(tr("Rule"));
  grid->addWidget(label, row,col++);
  
  _rules = new QComboBox;
  grid->addWidget(_rules, row++, col--);

  // message log
  QTabWidget *tab = new QTabWidget;
  vbox->addWidget(tab);

  QWidget *w = new QWidget;

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(2);
  w->setLayout(tvbox);

  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  tvbox->addWidget(_log);

  tab->addTab(w, tr("Message Log"));
  

  // button box
  _buttonBox = new QDialogButtonBox;
  vbox->addWidget(_buttonBox);

  _runButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _runButton->setText(tr("&Run"));
  connect(_runButton, SIGNAL(clicked()), this, SLOT(run()));

  _newButton = _buttonBox->addButton(QDialogButtonBox::Open);
  _newButton->setText(tr("&New"));
  _newButton->setIcon(QPixmap(newchart_xpm));
  connect(_newButton, SIGNAL(clicked()), this, SLOT(newRule()));

  _editButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _editButton->setText(tr("&Edit"));
  _editButton->setIcon(QPixmap(edit_xpm));
  connect(_editButton, SIGNAL(clicked()), this, SLOT(editRule()));

  _deleteButton = _buttonBox->addButton(QDialogButtonBox::Discard);
  _deleteButton->setText(tr("&Delete"));
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteRule()));

  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancelButton()));
}

void CSVDialog::loadSettings ()
{
  _rules->clear();

  CSVDataBase db;
  QStringList l;
  db.getRules(l);
  _rules->addItems(l);

  CSVConfig config;

  // restore the size of the window
  QSize sz;
  config.getData(CSVConfig::Size, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(CSVConfig::Pos, p);
  if (! p.isNull())
    move(p);

  QString s;
  config.getData(CSVConfig::LastRule, s);
  if (! s.isEmpty())
    _rules->setCurrentIndex(_rules->findText(s, Qt::MatchExactly));
}

void CSVDialog::saveSettings ()
{
  CSVConfig config;
  config.transaction();
  
  // save app size and position
  QSize sz = size();
  config.setData(CSVConfig::Size, sz);

  QPoint pt = pos();
  config.setData(CSVConfig::Pos, pt);

  QString s = _rules->currentText();
  config.setData(CSVConfig::LastRule, s);

  config.commit();
}

void CSVDialog::newRule ()
{
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle(tr("Qtstalker: New CSV Rule"));
  dialog->setLabelText(tr("Enter rule name"));
  dialog->setInputMode(QInputDialog::TextInput);
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(newRule2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void CSVDialog::newRule2 (QString name)
{
  if (name.isEmpty())
    return;

  if (_rules->findText(name) != -1)
  {
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle(tr("Qtstalker: Error New CSV Rule"));
    dialog->setMessage(tr("Duplicate rule name. Request denied."));
    dialog->show();
    return;
  }

  editRule(name);
}

void CSVDialog::editRule ()
{
  if (! _rules->count())
    return;
  
  editRule(_rules->currentText());
}

void CSVDialog::editRule (QString name)
{
  CSVRuleDialog *dialog = new CSVRuleDialog(name);
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void CSVDialog::deleteRule ()
{
  if (! _rules->count())
    return;

  Dialog *dialog = new Dialog(Dialog::_Message, 0);
  dialog->setWindowTitle(tr("Qtstalker: CSV Delete Rule"));
  dialog->setMessage(tr("Are you sure you want to delete CSV rule?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteRule2()));
  dialog->show();
}

void CSVDialog::deleteRule2 ()
{
  CSVDataBase db;
  QString s = _rules->currentText();
  db.deleteRule(s);
  
  _rules->removeItem(_rules->currentIndex());
}

void CSVDialog::cancelButton ()
{
  if (_thread->isRunning())
    _thread->stop();
  else
  {
    saveSettings();
    accept();
  }
}

void CSVDialog::run ()
{
  if (! _rules->count())
    return;
  
  _rule.name = _rules->currentText();
  
  CSVDataBase db;
  db.getRule(_rule);

  _log->append("*** " + tr("Import started") + " ***");
  
  _runButton->setEnabled(FALSE);
  _newButton->setEnabled(FALSE);
  _editButton->setEnabled(FALSE);
  _deleteButton->setEnabled(FALSE);

  _thread->setRule(&_rule);
  _thread->start();
}

void CSVDialog::done ()
{
  _log->append("*** " + tr("Import finished") + " ***");
  
  _runButton->setEnabled(TRUE);
  _newButton->setEnabled(TRUE);
  _editButton->setEnabled(TRUE);
  _deleteButton->setEnabled(TRUE);

  emit signalChartRefresh();
}

