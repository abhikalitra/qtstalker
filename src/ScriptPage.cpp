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
#include "PrefDialog.h"
#include "Config.h"

#include "../pics/asterisk.xpm"
#include "../pics/search.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
#include "../pics/ok.xpm"
#include "../pics/refresh.xpm"
#include "../pics/script.xpm"

#include <QCursor>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolTip>
#include <QPixmap>
#include <QVBoxLayout>
#include <QIcon>
#include <QtDebug>
#include <QLabel>
#include <QListWidgetItem>
#include <QToolButton>
#include <QSize>

ScriptPage::ScriptPage (QWidget *w) : QWidget (w)
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
  
  // create first row of launch buttons
//  tb = new QToolBar;
//  vbox->addWidget(tb);
//  createLaunchButtons(tb);
  
  vbox->addSpacing(5);
  
  QLabel *label = new QLabel(tr("Active Scripts"));
  vbox->addWidget(label);
  
  queList = new QListWidget;
  queList->setContextMenuPolicy(Qt::CustomContextMenu);
  queList->setSortingEnabled(TRUE);
  connect(queList, SIGNAL(itemSelectionChanged()), this, SLOT(queStatus()));
  connect(queList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(queRightClick(const QPoint &)));
  vbox->addWidget(queList);

  showAllScripts();
  
  loadSavedScripts();

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

void ScriptPage::createLaunchButtons (QToolBar *tb)
{
  int loop;
  for (loop = 0; loop < 5; loop++)
  {
    QToolButton *b = new QToolButton;
    b->setText(QString::number(loop + 1));
    tb->addWidget(b);
  }
}

void ScriptPage::newScript ()
{
  bool ok;
  QString s = QInputDialog::getText(this,
                                    tr("New Script"),
                                    tr("Enter name of new script."),
                                    QLineEdit::Normal,
                                    QString(),
                                    &ok,
                                    0);
  if (! ok || s.isEmpty())
    return;

  // check is name already exists
  QStringList l;
  ScriptDataBase db;
  db.getScripts(l);
  if (l.contains(s))
  {
    int rc = QMessageBox::warning(this,
    			          tr("Qtstalker: Warning"),
			          tr("This script already exists. Replace it?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);
    if (rc == QMessageBox::No)
      return;
  }

  editScript(s);
}

void ScriptPage::editScript ()
{
  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  QString s = item->text();
  editScript(s);
}

void ScriptPage::editScript (QString &d)
{
  ScriptDataBase db;
  Script *script = new Script;
  script->setName(d);
  db.getScript(script);

  PrefDialog *dialog = new PrefDialog;
  QString s = tr("Edit Script ") + d;
  dialog->setWindowTitle(s);
  s = tr("Settings");
  int page = 0;
  dialog->addPage(page, s);

  int pos = 0;
  
  s = tr("Command");
  QString command = script->getCommand();
  if (command.isEmpty())
    command = "perl";
  dialog->addTextItem(pos++, page, s, command);

  s = tr("Script File");
  QString file = script->getFile();
  dialog->addFileItem(pos++, page, s, file);

  s = tr("Run every X seconds");
  int refresh = script->getRefresh();
  dialog->addIntItem(pos++, page, s, refresh, 0, 999999);

  s = tr("Comment");
  QString comment = script->getComment();
  dialog->addTextEditItem(pos++, page, s, comment);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pos = 0;
  dialog->getItem(pos++, command);
  dialog->getItem(pos++, file);
  refresh = dialog->getInt(pos++);
  dialog->getItem(pos++, comment);

  delete dialog;

  script->setCommand(command);
  script->setFile(file);
  script->setRefresh(refresh);
  script->setComment(comment);
  db.setScript(script);

  // check if script is running and restart it with the new parms
  if (scripts.contains(script->getName()))
  {
    int rc = QMessageBox::warning(this,
    			          tr("Qtstalker: Warning"),
			          tr("The script edited is currently running. Restart script?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);
    if (rc == QMessageBox::No)
      return;
    
    Script *tscript = scripts.value(script->getName());
    scripts.remove(script->getName());
    delete tscript;
    
    scripts.insert(script->getName(), script);
    script->start();
    updateQueList();
  }
  else
    delete script;
  
  emit signalMessage(QString(tr("Script saved.")));
}

void ScriptPage::deleteScript ()
{
  int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Are you sure you want to permanently delete this script?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  QString name = item->text();

  ScriptDataBase db;
  Script script;
  script.setName(name);
  db.deleteScript(&script);
  delete item;

  // check if script is running
  if (scripts.contains(name))
  {
    rc = QMessageBox::warning(this,
    			      tr("Qtstalker: Warning"),
			      tr("The script deleted is currently running. Terminate script?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);
    if (rc == QMessageBox::No)
      return;
    else
    {
      Script *tscript = scripts.value(name);
      scripts.remove(name); // remove the refresh que hash list
      delete tscript;
      updateQueList();
    }
  }

  emit signalMessage(QString(tr("Script deleted.")));
}

void ScriptPage::listDoubleClick (QListWidgetItem *item)
{
  if (! item)
    return;

  int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Are you sure you want to execute this script?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
  if (rc == QMessageBox::No)
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
  bool aok;
  QString s = QInputDialog::getText(this,
                                    tr("Script Search"),
                                    tr("Enter either a specific name like MyScript or\na partial match like %Script or %Scr%"),
                                    QLineEdit::Normal,
                                    QString(),
                                    &aok,
                                    0);
  if (! aok || s.isEmpty())
    return;

  searchString = s;
  
  list->clear();

  ScriptDataBase db;
  QStringList l;
  db.getScriptSearch(searchString, l);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
    list->addItem(l[loop]);
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
    
    if (script->getRefresh())
    {
      QListWidgetItem *item = new QListWidgetItem(QIcon(refresh_xpm), name, queList);
      item->setToolTip(QString(tr("Run every ") + QString::number(script->getRefresh()) + tr(" seconds.")));
    }
    else
    {
      QListWidgetItem *item = new QListWidgetItem(QIcon(ok), name, queList);
      item->setToolTip(QString(tr("Running...")));
    }
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
}

void ScriptPage::removeScriptQueue ()
{
  QListWidgetItem *item = queList->currentItem();
  if (! item)
    return;

  int rc = QMessageBox::warning(this,
   			        tr("Qtstalker: Warning"),
			        tr("Script currently executing.\nAre you sure you want to remove this script?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  QString name = item->text();
  Script *script = scripts.value(name);
  script->stop();
  scripts.remove(name);
  delete script;
  
  updateQueList();
  
  emit signalMessage(QString(tr("Script removed.")));
}

void ScriptPage::saveRunningScripts ()
{
  QStringList l;
  QHashIterator<QString, Script *> it(scripts);
  while (it.hasNext())
  {
    it.next();
    Script *script = it.value();
    if (script->getRefresh())
      l.append(script->getName());
  }
  
  Config config;
  config.setData(Config::SavedRunningScripts, l);
}

void ScriptPage::loadSavedScripts ()
{
  QStringList l;
  Config config;
  config.getData(Config::SavedRunningScripts, l);
  
  ScriptDataBase db;
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    Script *script = new Script;
    script->setName(l[loop]);
    if (db.getScript(script))
    {
      delete script;
      continue;
    }
      
    scripts.insert(l[loop], script);
    connect(script, SIGNAL(signalDone(QString)), this, SLOT(scriptDone(QString)));
    connect(script, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
    script->start();
  }
  
  updateQueList();
}

void ScriptPage::runScript (Script *script)
{
  QString name = script->getName();
  
  if (scripts.contains(name))
  {
    QMessageBox mb;
    mb.setText(tr("This script is currently running. Request denied."));
    mb.exec();
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
  
  PrefDialog *dialog = new PrefDialog;
  QString s = tr("Run Script");
  dialog->setWindowTitle(s);
  s = tr("Settings");
  int page = 0;
  dialog->addPage(page, s);

  int pos = 0;
  
  s = tr("Command");
  QString command = "perl";
  dialog->addTextItem(pos++, page, s, command);

  Config config;
  QString file;
  config.getData(Config::LastScriptPath, file);
  if (file.isEmpty())
    file = QDir::homePath();
  
  s = tr("Script File");
  dialog->addFileItem(pos++, page, s, file);

  s = tr("Run every X seconds");
  int refresh = 0;
  dialog->addIntItem(pos++, page, s, refresh, 0, 999999);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  // get the dialog settings
  pos = 0;
  dialog->getItem(pos++, command);
  dialog->getItem(pos++, file);
  refresh = dialog->getInt(pos++);

  delete dialog;

  // get the script file name
  QFileInfo fi(file);
  QString name = fi.fileName();
  
  // save the script path
  config.setData(Config::LastScriptPath, file);
  
  // create the script and settings
  Script *script = new Script;
  script->setName(name);
  script->setCommand(command);
  script->setFile(file);
  script->setRefresh(refresh);
  
  // run the script
  runScript(script);
}

void ScriptPage::listStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = list->selectedItems();
  if (l.count())
    status = TRUE;
  
  actions.value(EditScript)->setEnabled(status); 
  actions.value(DeleteScript)->setEnabled(status); 
}

void ScriptPage::queStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = queList->selectedItems();
  if (l.count())
    status = TRUE;
  
  actions.value(CancelScript)->setEnabled(status); 
}
