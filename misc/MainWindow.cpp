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

  loadSettings();

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

void MainWindow::loadSettings ()
{
  QDir dir(QDir::homePath());
  QString home = dir.absolutePath();
  home.append("/.CSV");
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home))
      qDebug() << "CSV::Unable to create ~/.CSV directory.";
  }

  QString s = home + "/data.sqlite";
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "data");
  db.setHostName("localhost");
  db.setDatabaseName(s);
  db.setUserName("CSV");
  db.setPassword("CSV");
  if (! db.open())
  {
    qDebug() << "CSV::loadSettings:setup: data db open failed";
    return;
  }

  // create the config table
  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS config (key INT PRIMARY KEY, setting VARCHAR(50))";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSV::loadSettings:createConfigTable: " << q.lastError().text();

  // create the import rules table
  s = "CREATE TABLE IF NOT EXISTS importRules (";
  s.append(" name TEXT PRIMARY KEY");
  s.append(", format TEXT");
  s.append(", delimeter TEXT");
  s.append(", fileNameSymbol BOOL");
  s.append(", dateFormat TEXT");
  s.append(", interval INT");
  s.append(", fileList TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSV::loadSettings:createImportRulesTable: " << q.lastError().text();

  
  
  QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE", "quotes");
  db2.setHostName("localhost");
  s = home + "/quotes.sqlite";
  db2.setDatabaseName(s);
  db2.setUserName("CSV");
  db2.setPassword("CSV");
  if (! db2.open())
    qDebug() << "CSV::loadSettings:setupQuoteBase: quotes db open failed";

  // mysql testing
//  QSqlDatabase db2 = QSqlDatabase::addDatabase("QMYSQL", "quotes");
//  db2.setHostName("localhost");
//  db2.setDatabaseName("test");
//  db2.setUserName("anonymous");
//  db2.setPassword(QString());
//  if (! db2.open())
//    qDebug() << "CSV::loadSettings:setupQuoteBase: quotes db open failed";


  
  
  QSqlQuery q2(db2);
  s = "CREATE TABLE IF NOT EXISTS symbolIndex (";
  s.append(" symbol VARCHAR(50) PRIMARY KEY UNIQUE");
  s.append(", name VARCHAR(100)");
  s.append(", exchange VARCHAR(25)");
  s.append(", data VARCHAR(250)");
  s.append(")");
  q2.exec(s);
  if (q2.lastError().isValid())
    qDebug() << "CSV::loadSettings:createSymbolIndexTable: " << q2.lastError().text();
}

void MainWindow::setConfig (QString &k, QString &d)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO config (key, setting) VALUES (" + k + ",'" + d + "')";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CSV::setConfig: " << q.lastError().text();
    return;
  }
}

void MainWindow::getConfig (QString &k, QString &d)
{
  d.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT setting FROM config WHERE key=" + k;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CSV::getConfig: " << q.lastError().text();
    return;
  }

  if (q.next())
    d = q.value(0).toString();
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

void MainWindow::getRules (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM importRules";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CSV::getRules: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void MainWindow::newRule ()
{
  bool ok;
  QString s = QInputDialog::getText(this, tr("New Rule Name"), tr("Enter new rule name"), QLineEdit::Normal, tr("New Rule"), &ok, 0);
  if (! ok)
    return;

  QStringList l;
  getRules(l);
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

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM importRules WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSV::deleteRule: " << q.lastError().text();

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
  getRules(l);
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

void MainWindow::loadRule (CSVRule &rule)
{
  QString ts;
  rule.getName(ts);
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM importRules WHERE name='" + ts + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "MainWindow::loadRule: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    s = q.value(1).toString();
    rule.setFormat(s);
    
    s = q.value(2).toString();
    rule.setDelimiter(s);

    rule.setFileNameSymbol(q.value(3).toBool());

    s = q.value(4).toString();
    rule.setDateFormat(s);

    s = q.value(5).toString();
    rule.setInterval(s);

    s = q.value(6).toString();
    QStringList l = s.split("|");
    rule.setFileList(l);
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
  loadRule(rule);

  CSV *csv = new CSV(rule);
  connect(csv, SIGNAL(signalMessage(QString)), this, SLOT(message(QString)));
  connect(csv, SIGNAL(signalActive(QString)), this, SLOT(CSVActive(QString)));
  connect(csv, SIGNAL(signalInactive(QString)), this, SLOT(CSVInactive(QString)));
  CSVList.insert(name, csv);

  csv->status();    
}

