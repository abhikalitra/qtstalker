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
#include "CSVThread.h"
#include "CSVRule.h"
#include "CSVConfig.h"

#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QInputDialog>
#include <QGroupBox>

CSVDialog::CSVDialog () : QDialog (0, 0)
{
  setWindowTitle(tr("Configure CSV"));
  createMainPage();
  loadSettings();
}

void CSVDialog::createMainPage ()
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

  // rule parm
  QLabel *label = new QLabel(tr("Rule"));
  grid->addWidget(label, row,col++);
  
  _rules = new QComboBox;
  grid->addWidget(_rules, row++, col--);

  // message log
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Message Log"));
  vbox->addWidget(gbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  gbox->setLayout(hbox);

  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  hbox->addWidget(_log);

  _buttonBox = new QDialogButtonBox;
  vbox->addWidget(_buttonBox);

  _runButton = new QPushButton(tr("&Run"));
  _buttonBox->addButton(_runButton, QDialogButtonBox::ActionRole);
  connect(_runButton, SIGNAL(clicked()), this, SLOT(run()));

  _newButton = new QPushButton(tr("&New"));
  _buttonBox->addButton(_newButton, QDialogButtonBox::ActionRole);
  connect(_newButton, SIGNAL(clicked()), this, SLOT(newRule()));

  _editButton = new QPushButton(tr("&Edit"));
  _buttonBox->addButton(_editButton, QDialogButtonBox::ActionRole);
  connect(_editButton, SIGNAL(clicked()), this, SLOT(editRule()));

  _deleteButton = new QPushButton(tr("&Delete"));
  _buttonBox->addButton(_deleteButton, QDialogButtonBox::ActionRole);
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteRule()));

  _cancelButton = new QPushButton(tr("&Cancel"));
  _buttonBox->addButton(_cancelButton, QDialogButtonBox::ActionRole);
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
  bool ok;
  QString name = QInputDialog::getText(this,
                                       tr("New CSV Rule"),
                                       tr("Enter rule name"),
                                       QLineEdit::Normal,
                                       QString(),
                                       &ok,
                                       0);
  if (! ok || name.isEmpty())
    return;

  if (_rules->findText(name) != -1)
  {
    int rc = QMessageBox::warning(this,
                                  tr("Warning"),
                                  tr("Duplicate rule. Overwrite?"),
                                  QMessageBox::Yes,
                                  QMessageBox::No,
                                  QMessageBox::NoButton);

    if (rc == QMessageBox::No)
      return;
  }

  editRule(name);
}

void CSVDialog::editRule ()
{
  editRule(_rules->currentText());
}

void CSVDialog::editRule (QString name)
{
  CSVRuleDialog *dialog = new CSVRuleDialog(name);
  dialog->exec();
  loadSettings();  
}

void CSVDialog::deleteRule ()
{
  int rc = QMessageBox::warning(this,
                                tr("Warning"),
                                tr("Are you sure you want to delete rule?"),
                                QMessageBox::Yes,
                                QMessageBox::No,
                                QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  CSVDataBase db;
  QString s = _rules->currentText();
  db.deleteRule(s);
  
  _rules->removeItem(_rules->currentIndex());
}

void CSVDialog::cancelButton ()
{
  accept();
}

void CSVDialog::run ()
{
  CSVRule rule;
  rule.setName(_rules->currentText());
  
  CSVDataBase db;
  db.getRule(rule);

  CSVThread thread(&rule);
  connect(&thread, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
  thread.run();
}

