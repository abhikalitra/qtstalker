/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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



#include "MainWindow.h"
#include "CSVRuleDialog.h"
#include "Database.h"


#include <QtSql>
#include <QtDebug>
#include <QLayout>
#include <QIcon>
#include <QDir>
#include <QWidget>
#include <QMenu>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QTreeWidgetItem>
#include <QCoreApplication>

#include "../pics/newchart.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/done.xpm"



MainWindow::MainWindow ()
{
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  QWidget *w = new QWidget;
  setCentralWidget(w);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(10);
  vbox->setSpacing(10);
  w->setLayout(vbox);
  
  ruleList = new QTreeWidget;
  QStringList l;
  l << tr("Rule") << tr("Status");
  ruleList->setHeaderLabels(l);
  ruleList->setSortingEnabled(TRUE);
  ruleList->resizeColumnToContents(0);
  connect(ruleList, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
  vbox->addWidget(ruleList);

  log = new QTextEdit;
  log->setReadOnly(TRUE);
  vbox->addWidget(log);

  setUnifiedTitleAndToolBarOnMac(true);

  setWindowTitle(tr("Import CSV Quotes"));

  loadRules();
}

MainWindow::~MainWindow ()
{
  qDeleteAll(CSVList);
}

void MainWindow::createActions ()
{
  QAction * action = new QAction(QIcon(newchart), tr("&New Rule"), this);
  action->setShortcuts(QKeySequence::New);
  action->setStatusTip(tr("Create a new import rule"));
  connect(action, SIGNAL(triggered()), this, SLOT(newRule()));
  actionList.insert(NewRule, action);

  action = new QAction(QIcon(edit), tr("&Edit Rule..."), this);
  action->setShortcut(QKeySequence(Qt::ALT+Qt::Key_E));
  action->setStatusTip(tr("Edit an existing rule"));
  connect(action, SIGNAL(triggered()), this, SLOT(editRule()));
  actionList.insert(EditRule, action);

  action = new QAction(QIcon(deleteitem), tr("&Delete Rule..."), this);
  action->setShortcuts(QKeySequence::Delete);
  action->setStatusTip(tr("Delete an existing rule"));
  connect(action, SIGNAL(triggered()), this, SLOT(deleteRule()));
  actionList.insert(DeleteRule, action);

  action = new QAction(QIcon(finished), tr("E&xit"), this);
  action->setShortcut(tr("Ctrl+Q"));
  action->setStatusTip(tr("Exit the application"));
  connect(action, SIGNAL(triggered()), this, SLOT(close()));
  actionList.insert(Exit, action);

  action = new QAction(tr("&About"), this);
  action->setStatusTip(tr("About CSV"));
  connect(action, SIGNAL(triggered()), this, SLOT(about()));
  actionList.insert(About, action);
}

void MainWindow::createMenus ()
{
  QMenu *menu = menuBar()->addMenu(tr("&File"));
  menu->addAction(actionList.value(NewRule));
  menu->addSeparator();
  menu->addAction(actionList.value(Exit));

  menu = menuBar()->addMenu(tr("&Edit"));
  menu->addAction(actionList.value(EditRule));
  menu->addAction(actionList.value(DeleteRule));

  menuBar()->addSeparator();

  menu = menuBar()->addMenu(tr("&Help"));
  menu->addAction(actionList.value(About));
}

void MainWindow::createToolBars ()
{
  QToolBar *toolBar = addToolBar(tr("File"));
  toolBar->addAction(actionList.value(Exit));
  toolBar->addAction(actionList.value(NewRule));

  toolBar->addSeparator();

  toolBar->addAction(actionList.value(EditRule));
  toolBar->addAction(actionList.value(DeleteRule));
}

void MainWindow::createStatusBar ()
{
  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::closeEvent (QCloseEvent *event)
{
  event->accept();
}

void MainWindow::about ()
{
  QString versionString = "CSV\nVersion 0.37-dev (working title)\nBuilt using Qt ";
  versionString += QT_VERSION_STR;
  versionString += "\n(C) 2009 by Stefan Stratigakos\nqtstalker.sourceforge.net";
  QMessageBox::about(this, tr("About CSV"), versionString);
}

void MainWindow::newRule ()
{
  bool ok;
  QString s = QInputDialog::getText(this, tr("New Rule Name"), tr("Enter new rule name"), QLineEdit::Normal, tr("New Rule"), &ok, 0);
  if (! ok)
    return;

  QStringList l;
  Database db;
  db.getRules(l);
  if (l.contains(s))
  {
    QMessageBox::information(this, tr("Error"), s + tr(" already exists."));
    return;
  }

  CSVRuleDialog *dialog = new CSVRuleDialog(this, s);
  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  QTreeWidgetItem *item = new QTreeWidgetItem(ruleList);
  item->setText(0, s);
  ruleList->resizeColumnToContents(0);

  itemList.insert(s, item);

  addRule(s);

  delete dialog;
}

void MainWindow::editRule ()
{
  QTreeWidgetItem *item = ruleList->currentItem();
  if (! item)
    return;

  QString s = item->text(0);
  CSVRuleDialog *dialog = new CSVRuleDialog(this, s);
  dialog->exec();

  CSV *csv = CSVList.value(s);
  delete csv;
  CSVList.remove(s);
  addRule(s);

  delete dialog;
}

void MainWindow::deleteRule ()
{
  QTreeWidgetItem *item = ruleList->currentItem();
  if (! item)
    return;

  int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Are you sure you want to permanently delete this rule ?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  QString name = item->text(0);

  Database db;
  db.deleteRule(name);

  delete item;
  itemList.remove(name);

  CSV *csv = CSVList.value(name);
  delete csv;
  CSVList.remove(name);
}

void MainWindow::message (QString m)
{
  log->append(m);
  QCoreApplication::processEvents(QEventLoop::AllEvents);
}

void MainWindow::itemDoubleClicked (QTreeWidgetItem *, int col)
{
  if (col == 0)
    editRule();
  else
    statusChanged();
}

void MainWindow::statusChanged ()
{
  QTreeWidgetItem *item = ruleList->currentItem();
  if (! item)
    return;

  if (item->text(1) == tr("Active"))
  {
    QMessageBox::information(this, tr("Error"), " " + tr("Cannot start an active rule."));
    return;
  }

  int rc = QMessageBox::question(this,
    			         tr("CSV: Activate Rule"),
			         tr("Are you sure you want to activate this rule ?"),
			         QMessageBox::Yes,
			         QMessageBox::No,
			         QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  QString s = item->text(0);
  CSV *csv = CSVList.value(s);
  csv->importSingleShot();
}

void MainWindow::loadRules ()
{
  QStringList l;
  Database db;
  db.getRules(l);
  ruleList->clear();

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(ruleList);
    item->setText(0, l[loop]);
    ruleList->resizeColumnToContents(0);

    itemList.insert(l[loop], item);

    addRule(l[loop]);
  }
}

void MainWindow::CSVActive (QString rule)
{
  QTreeWidgetItem *item = itemList.value(rule);
  if (! item)
    return;

  item->setText(1, tr("Active"));
  QCoreApplication::processEvents(QEventLoop::AllEvents);
}

void MainWindow::CSVInactive (QString rule)
{
  QTreeWidgetItem *item = itemList.value(rule);
  if (! item)
    return;

  item->setText(1, tr("Inactive"));
  QCoreApplication::processEvents(QEventLoop::AllEvents);
}

void MainWindow::addRule (QString &name)
{
  CSVRule rule;
  rule.setName(name);

  Database db;
  db.loadRule(rule);

  CSV *csv = new CSV(rule);
  connect(csv, SIGNAL(signalMessage(QString)), this, SLOT(message(QString)));
  connect(csv, SIGNAL(signalActive(QString)), this, SLOT(CSVActive(QString)));
  connect(csv, SIGNAL(signalInactive(QString)), this, SLOT(CSVInactive(QString)));
  CSVList.insert(name, csv);

  csv->status();    
}

