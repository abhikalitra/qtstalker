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
#include "Globals.h"

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
  
  _list = new QListWidget;
  _list->setContextMenuPolicy(Qt::CustomContextMenu);
  _list->setSortingEnabled(TRUE);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(listStatus()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(listDoubleClick(QListWidgetItem *)));
  connect(_list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(listRightClick(const QPoint &)));
  vbox->addWidget(_list);


  // create button grid
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(0);
  grid->setSpacing(0);
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
  
  _queList = new QListWidget;
  _queList->setContextMenuPolicy(Qt::CustomContextMenu);
  _queList->setSortingEnabled(TRUE);
  _queList->setMaximumHeight(100);
  connect(_queList, SIGNAL(itemSelectionChanged()), this, SLOT(queStatus()));
  connect(_queList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(queRightClick(const QPoint &)));
  vbox->addWidget(_queList);

  Config config;
  config.transaction();

  config.getData(Config::LastScriptSearch, _searchString);
  if (_searchString.isEmpty())
  {
    _searchString = "%";
    config.setData(Config::LastScriptSearch, _searchString);
  }

  config.commit();

  updateList();
  
  listStatus();
  
  queStatus();
}

ScriptPage::~ScriptPage ()
{
  qDeleteAll(_scripts);
}

void ScriptPage::createActions ()
{
  QAction *action = new QAction(QIcon(asterisk_xpm), tr("Show &All Scripts"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Show All Scripts"));
  connect(action, SIGNAL(activated()), this, SLOT(showAllScripts()));
  _actions.insert(ShowAllScripts, action);

  action = new QAction(QIcon(search_xpm), tr("&Search..."), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  action->setToolTip(tr("Search"));
  connect(action, SIGNAL(activated()), this, SLOT(search()));
  _actions.insert(SearchScript, action);

  action = new QAction(QIcon(newchart_xpm), tr("&New Script..."), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setToolTip(tr("New Script"));
  connect(action, SIGNAL(activated()), this, SLOT(newScript()));
  _actions.insert(NewScript, action);

  action = new QAction(QIcon(edit_xpm), tr("&Edit Script..."), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
  action->setToolTip(tr("Edit Script"));
  connect(action, SIGNAL(activated()), this, SLOT(editScript()));
  _actions.insert(EditScript, action);

  action = new QAction(QIcon(delete_xpm), tr("&Delete Script..."), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete Script"));
  connect(action, SIGNAL(activated()), this, SLOT(deleteScript()));
  _actions.insert(DeleteScript, action);

  action = new QAction(QIcon(delete_xpm), tr("&Cancel Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_C));
  action->setToolTip(tr("Cancel script"));
  connect(action, SIGNAL(activated()), this, SLOT(removeScriptQueue()));
  _actions.insert(CancelScript, action);

  action = new QAction(QIcon(script_xpm), tr("&Run Script..."), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  action->setToolTip(tr("Run script"));
  connect(action, SIGNAL(activated()), this, SLOT(runScriptDialog()));
  _actions.insert(RunScript, action);
}

void ScriptPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(_actions.value(ShowAllScripts));
  tb->addAction(_actions.value(SearchScript));
  tb->addSeparator();
  tb->addAction(_actions.value(NewScript));
  tb->addSeparator();
  tb->addAction(_actions.value(RunScript));

  _listMenu = new QMenu(this);
  _listMenu->addAction(_actions.value(NewScript));
  _listMenu->addAction(_actions.value(EditScript));
  _listMenu->addAction(_actions.value(DeleteScript));
  _listMenu->addSeparator();
  _listMenu->addAction(_actions.value(RunScript));
  _listMenu->addSeparator();
  _listMenu->addAction(_actions.value(ShowAllScripts));
  _listMenu->addAction(_actions.value(SearchScript));

  _queMenu = new QMenu(this);
  _queMenu->addAction(_actions.value(CancelScript));
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
  QListWidgetItem *item = _list->currentItem();
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
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Run Script"));
  dialog->setMessage(tr("Are you sure you want to execute this script?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(listDoubleClick2()));
  dialog->show();
}

void ScriptPage::listDoubleClick2 ()
{
  QListWidgetItem *item = _list->currentItem();
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
  _listMenu->exec(QCursor::pos());
}

void ScriptPage::queRightClick (const QPoint &)
{
  _queMenu->exec(QCursor::pos());
}

void ScriptPage::search ()
{
  ScriptSearchDialog *dialog = new ScriptSearchDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalSearch(QString)), this, SLOT(search2(QString)));
  dialog->show();
}

void ScriptPage::search2 (QString ss)
{
  _searchString = ss;

  Config config;
  config.transaction();
  config.setData(Config::LastScriptSearch, _searchString);
  config.commit();
  
  updateList();
}

void ScriptPage::updateList ()
{
  _list->clear();

  ScriptDataBase db;
  QStringList l;
  db.getScriptSearch(_searchString, l);

  _list->addItems(l);
}

void ScriptPage::showAllScripts ()
{
  _searchString = "%";

  Config config;
  config.transaction();
  config.setData(Config::LastScriptSearch, _searchString);
  config.commit();

  updateList();
}

void ScriptPage::updateQueList ()
{
  _queList->clear();

  QHashIterator<QString, Script *> it(_scripts);
  while (it.hasNext())
  {
    it.next();
    Script *script = it.value();

    QString name = script->getName();
    
    QListWidgetItem *item = new QListWidgetItem(QIcon(ok), name, _queList);
    item->setToolTip(QString(tr("Running...")));
  }
}

void ScriptPage::scriptDone (QString name)
{
  if (! _scripts.contains(name))
    return;
  
  Script *script = _scripts.value(name);
  _scripts.remove(name);
  delete script;

  updateQueList();

  emit signalScriptDone();
}

void ScriptPage::removeScriptQueue ()
{
  QListWidgetItem *item = _queList->currentItem();
  if (! item)
    return;

  Dialog *dialog = new Dialog(Dialog::_Message, 0);
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Stop running script"));
  dialog->setMessage(tr("Script currently executing.\nAre you sure you want to remove this script?"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(removeScriptQueue2()));
  dialog->show();
}

void ScriptPage::removeScriptQueue2 ()
{
  QListWidgetItem *item = _queList->currentItem();
  if (! item)
    return;

  QString name = item->text();
  Script *script = _scripts.value(name);
  script->stop();
  _scripts.remove(name);
  delete script;

  updateQueList();

  emit signalMessage(QString(tr("Script removed.")));
}

void ScriptPage::runScript (Script *script)
{
  QString name = script->getName();
  
  if (_scripts.contains(name))
  {
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Error run script"));
    dialog->setMessage(tr("This script is currently running. Request denied."));
    dialog->show();
    delete script;
    return;
  }

  _scripts.insert(name, script);
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
  config.transaction();
  config.setData(Config::LastScriptPath, file);
  config.commit();
  
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
  int status = _list->count();
  _actions.value(EditScript)->setEnabled(status); 
  _actions.value(DeleteScript)->setEnabled(status); 
}

void ScriptPage::queStatus ()
{
  int status = _queList->count();
  _actions.value(CancelScript)->setEnabled(status); 
}

