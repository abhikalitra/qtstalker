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
#include "CSVNewDialog.h"
#include "CSVDeleteDialog.h"
#include "CSVRule.h"
#include "CSVEditDialog.h"
#include "CSVRunDialog.h"
#include "Globals.h"

#include "../pics/edit.xpm"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "../pics/disable.xpm"

#include <QLayout>
#include <QDialogButtonBox>

CSVDialog::CSVDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": CSV ");

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);

  QPushButton *b = bbox->addButton(QDialogButtonBox::Close);
  connect(b, SIGNAL(clicked()), this, SLOT(closeDialog()));
  vbox->addWidget(bbox);

  createMainPage();
  createLogPage();

  loadSettings();

  rulesSelectionChanged();
}

void CSVDialog::createMainPage ()
{
  QWidget *w = new QWidget;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(0);
  w->setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(2);
  vbox->addLayout(hbox);

  _rules = new QListWidget;
  _rules->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_rules, SIGNAL(itemSelectionChanged()), this, SLOT(rulesSelectionChanged()));
  hbox->addWidget(_rules);

  // create button box
  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setMargin(0);
  bbox->setSpacing(2);
  hbox->addLayout(bbox);

  QPushButton *b = new QPushButton;
  b->setText(tr("&Run"));
  b->setToolTip(tr("Run CSV rules"));
  connect(b, SIGNAL(clicked()), this, SLOT(run()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&New"));
  b->setIcon(QPixmap(newchart_xpm));
  b->setToolTip(tr("Create a new CSV rule"));
  connect(b, SIGNAL(clicked()), this, SLOT(newRule()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Edit"));
  b->setIcon(QPixmap(edit_xpm));
  b->setToolTip(tr("Edit CSV rule"));
  connect(b, SIGNAL(clicked()), this, SLOT(editRule()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Delete"));
  b->setToolTip(tr("Delete CSV rules"));
  b->setIcon(QPixmap(delete_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(deleteRule()));
  bbox->addWidget(b);

  _cancelButton = new QPushButton;
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setToolTip(tr("Cancel running CSV rule"));
  _cancelButton->setIcon(QPixmap(disable_xpm));
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(stop()));
  bbox->addWidget(_cancelButton);

  bbox->addStretch();

  _tabs->addTab(w, tr("Monitor"));
}

void CSVDialog::createLogPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  vbox->setMargin(0);
  w->setLayout(vbox);

  // message log
  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  vbox->addWidget(_log);

  _tabs->addTab(w, tr("Log"));
}

void CSVDialog::loadSettings ()
{
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

  config.commit();
}

void CSVDialog::newRule ()
{
  CSVNewDialog *dialog = new CSVNewDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

void CSVDialog::editRule ()
{
  CSVEditDialog *dialog = new CSVEditDialog;
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(editRule2(QStringList)));
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

void CSVDialog::editRule2 (QStringList l)
{
  if (! l.count())
    return;
  
  CSVRuleDialog *dialog = new CSVRuleDialog(l.at(0));
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

void CSVDialog::deleteRule ()
{
  CSVDeleteDialog *dialog = new CSVDeleteDialog();
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

void CSVDialog::run ()
{
  CSVRunDialog *dialog = new CSVRunDialog();
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(run2(QStringList)));
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

void CSVDialog::run2 (QStringList l)
{
  CSVDataBase db;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    CSVRule rule;
    rule.setName(l.at(loop));
    db.getRule(rule);

    CSVThread *thread = new CSVThread(this, rule);
    connect(thread, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
    connect(thread, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));

    _log->append("*** " + rule.name() + " " + tr("started") + " ***");
  
    thread->start();

    _threads.insert(l.at(loop), thread);
  }

  updateRules();
}

void CSVDialog::done (QString name)
{
  _log->append("*** " + name + " " + tr("finished") + " ***");

  _threads.remove(name);

  updateRules();

  emit signalChartRefresh();
}

void CSVDialog::closeDialog ()
{
  saveSettings();
  hide();
}

void CSVDialog::stop ()
{
  QList<QListWidgetItem *> l = _rules->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    CSVThread *thread = _threads.value(l.at(loop)->text());
    if (! thread)
      continue;

    thread->stop();
  }
}

void CSVDialog::rulesSelectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> l = _rules->selectedItems();
  if (l.count())
    status = 1;

  _cancelButton->setEnabled(status);
}

void CSVDialog::updateRules ()
{
  _rules->clear();

  QStringList l;
  QHashIterator<QString, CSVThread *> it(_threads);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }

  _rules->addItems(l);

  rulesSelectionChanged();
}

