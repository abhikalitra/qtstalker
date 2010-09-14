/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "ScriptPage.h"
#include "ScriptDataBase.h"
#include "ScriptDialog.h"
#include "Config.h"
#include "ScriptLaunchButton.h"
#include "ScriptNewDialog.h"
#include "ScriptDeleteDialog.h"
#include "ScriptSearchDialog.h"

#include "../pics/asterisk.xpm"
#include "../pics/search.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
#include "../pics/ok.xpm"
#include "../pics/script.xpm"

#include <QCursor>
#include <QPixmap>
#include <QLayout>
#include <QIcon>
#include <QtDebug>
#include <QLabel>
#include <QListWidgetItem>
#include <QToolButton>
#include <QFileInfo>

ScriptPage::ScriptPage ()
{
  createActions();
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);
  
  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);
  tb->setIconSize(QSize(18, 18));
  
  createButtonMenu(tb);
  
  list = new QListWidget;
  list->setContextMenuPolicy(Qt::CustomContextMenu);
  list->setSortingEnabled(TRUE);
  connect(list, SIGNAL(itemSelectionChanged()), this, SLOT(listStatus()));
  connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(listDoubleClick(QListWidgetItem *)));
  connect(list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(listRightClick(const QPoint &)));
  vbox->addWidget(list);


  // create button grid
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(0);
  grid->setSpacing(0);
//  grid->setColumnStretch(5, 1);
  vbox->addLayout(grid);


  int row = 0;
  int col = 0;
  ScriptLaunchButton *b = new ScriptLaunchButton(Config::ScriptLaunchButton1, 1);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  b = new ScriptLaunchButton(Config::ScriptLaunchButton2, 2);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  b = new ScriptLaunchButton(Config::ScriptLaunchButton3, 3);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  b = new ScriptLaunchButton(Config::ScriptLaunchButton4, 4);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  row++;
  col = 0;

  b = new ScriptLaunchButton(Config::ScriptLaunchButton5, 5);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  b = new ScriptLaunchButton(Config::ScriptLaunchButton6, 6);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);

  b = new ScriptLaunchButton(Config::ScriptLaunchButton7, 7);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  b = new ScriptLaunchButton(Config::ScriptLaunchButton8, 8);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  row++;
  col = 0;

  b = new ScriptLaunchButton(Config::ScriptLaunchButton9, 9);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  b = new ScriptLaunchButton(Config::ScriptLaunchButton10, 10);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  b = new ScriptLaunchButton(Config::ScriptLaunchButton11, 11);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);
  
  b = new ScriptLaunchButton(Config::ScriptLaunchButton12, 12);
  connect(b, SIGNAL(signalButtonClicked(Script *)), this, SLOT(runScript(Script *)));
  grid->addWidget(b, row, col++);


  
  vbox->addSpacing(5);
  
  QLabel *label = new QLabel(tr("Active Scripts"));
  vbox->addWidget(label);
  
  queList = new QListWidget;
  queList->setContextMenuPolicy(Qt::CustomContextMenu);
  queList->setSortingEnabled(TRUE);
  queList->setMaximumHeight(100);
  connect(queList, SIGNAL(itemSelectionChanged()), this, SLOT(queStatus()));
  connect(queList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(queRightClick(const QPoint &)));
  vbox->addWidget(queList);

  showAllScripts();
  
  listStatus();
  queStatus();
}

ScriptPage::~ScriptPage ()
{
  qDeleteAll(scripts);
}

void ScriptPage::createActions ()
{
  QAction *action  = new QAction(QIcon(asterisk_xpm), tr("Show &All Scripts"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Show All Scripts"));
  connect(action, SIGNAL(activated()), this, SLOT(showAllScripts()));
  actions.insert(ShowAllScripts, action);

  action  = new QAction(QIcon(search_xpm), tr("&Search"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  action->setToolTip(tr("Search"));
  connect(action, SIGNAL(activated()), this, SLOT(search()));
  actions.insert(SearchScript, action);

  action  = new QAction(QIcon(newchart_xpm), tr("&New Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setToolTip(tr("New Script"));
  connect(action, SIGNAL(activated()), this, SLOT(newScript()));
  actions.insert(NewScript, action);

  action  = new QAction(QIcon(edit), tr("&Edit Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
  action->setToolTip(tr("Edit Script"));
  connect(action, SIGNAL(activated()), this, SLOT(editScript()));
  actions.insert(EditScript, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Delete Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete Script"));
  connect(action, SIGNAL(activated()), this, SLOT(deleteScript()));
  actions.insert(DeleteScript, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Cancel Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_C));
  action->setToolTip(tr("Cancel script"));
  connect(action, SIGNAL(activated()), this, SLOT(removeScriptQueue()));
  actions.insert(CancelScript, action);

  action  = new QAction(QIcon(script_xpm), tr("&Run Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  action->setToolTip(tr("Run script"));
  connect(action, SIGNAL(activated()), this, SLOT(runScriptDialog()));
  actions.insert(RunScript, action);
}

void ScriptPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(actions.value(ShowAllScripts));
  tb->addAction(actions.value(SearchScript));
  tb->addSeparator();
  tb->addAction(actions.value(NewScript));
  tb->addSeparator();
  tb->addAction(actions.value(RunScript));

  listMenu = new QMenu(this);
  listMenu->addAction(actions.value(NewScript));
  listMenu->addAction(actions.value(EditScript));
  listMenu->addAction(actions.value(DeleteScript));
  listMenu->addSeparator();
  listMenu->addAction(actions.value(RunScript));
  listMenu->addSeparator();
  listMenu->addAction(actions.value(ShowAllScripts));
  listMenu->addAction(actions.value(SearchScript));

  queMenu = new QMenu(this);
  queMenu->addAction(actions.value(CancelScript));
}

void ScriptPage::newScript ()
{
  ScriptNewDialog *dialog = new ScriptNewDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalNewScript(QString)), this, SLOT(editScript(QString)));
  dialog->show();
}

void ScriptPage::editScript ()
{
  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  editScript(item->text());
}

void ScriptPage::editScript (QString d)
{
  ScriptDialog *dialog = new ScriptDialog(d);
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ScriptPage::deleteScript ()
{
  ScriptDeleteDialog *dialog = new ScriptDeleteDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalDelete()), this, SLOT(showAllScripts()));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ScriptPage::listDoubleClick (QListWidgetItem *item)
{
  if (! item)
    return;

  Dialog *dialog = new Dialog(Dialog::_Message, 0);
  dialog->setWindowTitle(tr("Qtstalker: Run Script"));
  dialog->setMessage(tr("Are you sure you want to execute this script?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(listDoubleClick2()));
  dialog->show();
}

void ScriptPage::listDoubleClick2 ()
{
  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;
      
  QString name = item->text();

  Script *script = new Script;
  script->setName(name);
  ScriptDataBase db;
  db.getScript(script);
      
  runScript(script);
}

void ScriptPage::listRightClick (const QPoint &)
{
  listMenu->exec(QCursor::pos());
}

void ScriptPage::queRightClick (const QPoint &)
{
  queMenu->exec(QCursor::pos());
}

void ScriptPage::search ()
{
  ScriptSearchDialog *dialog = new ScriptSearchDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalSearch(QString, QStringList)), this, SLOT(search2(QString, QStringList)));
  dialog->show();
}

void ScriptPage::search2 (QString ss, QStringList l)
{
  searchString = ss;
  
  list->clear();
  list->addItems(l);
}

void ScriptPage::showAllScripts ()
{
  list->clear();

  ScriptDataBase db;
  QStringList l;
  db.getScripts(l);
  list->addItems(l);
}

void ScriptPage::updateQueList ()
{
  queList->clear();

  QHashIterator<QString, Script *> it(scripts);
  while (it.hasNext())
  {
    it.next();
    Script *script = it.value();

    QString name = script->getName();
    
    QListWidgetItem *item = new QListWidgetItem(QIcon(ok), name, queList);
    item->setToolTip(QString(tr("Running...")));
  }
}

void ScriptPage::scriptDone (QString name)
{
  if (! scripts.contains(name))
    return;
  
  Script *script = scripts.value(name);
  scripts.remove(name);
  delete script;

  updateQueList();

  emit signalScriptDone();
}

void ScriptPage::removeScriptQueue ()
{
  QListWidgetItem *item = queList->currentItem();
  if (! item)
    return;

  Dialog *dialog = new Dialog(Dialog::_Message, 0);
  dialog->setWindowTitle(tr("Qtstalker: Stop running script"));
  dialog->setMessage(tr("Script currently executing.\nAre you sure you want to remove this script?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(removeScriptQueue2()));
  dialog->show();
}

void ScriptPage::removeScriptQueue2 ()
{
  QListWidgetItem *item = queList->currentItem();
  if (! item)
    return;

  QString name = item->text();
  Script *script = scripts.value(name);
  script->stop();
  scripts.remove(name);
  delete script;

  updateQueList();

  emit signalMessage(QString(tr("Script removed.")));
}

void ScriptPage::runScript (Script *script)
{
  QString name = script->getName();
  
  if (scripts.contains(name))
  {
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle(tr("Qtstalker: Error run script"));
    dialog->setMessage(tr("This script is currently running. Request denied."));
    dialog->show();
    return;
  }
	
  scripts.insert(name, script);
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(scriptDone(QString)));
  connect(script, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  script->start();
  
  updateQueList();
}

void ScriptPage::runScriptDialog ()
{
  // create a dialog to obtain script settings
  ScriptDialog *dialog = new ScriptDialog(QString());
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalRunScript(QString, QString)), this, SLOT(runScriptDialog2(QString, QString)));
  dialog->show();
}

void ScriptPage::runScriptDialog2 (QString command, QString file)
{
  // get the script file name
  QFileInfo fi(file);
  QString name = fi.fileName();
  
  // save the script path
  Config config;
  config.setData(Config::LastScriptPath, file);
  
  // create the script and settings
  Script *script = new Script;
  script->setName(name);
  script->setCommand(command);
  script->setFile(file);
  
  // run the script
  runScript(script);
}

void ScriptPage::listStatus ()
{
  int status = list->count();
  actions.value(EditScript)->setEnabled(status); 
  actions.value(DeleteScript)->setEnabled(status); 
}

void ScriptPage::queStatus ()
{
  int status = queList->count();
  actions.value(CancelScript)->setEnabled(status); 
}

