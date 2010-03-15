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
#include "Config.h"
#include "PrefDialog.h"

#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"
#include "../pics/script.xpm"
#include "../pics/que.xpm"
#include "../pics/ok.xpm"

#include <QCursor>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolTip>
#include <QPixmap>
#include <QVBoxLayout>
#include <QIcon>
#include <QtDebug>
#include <QLabel>
#include <QButtonGroup>
#include <QListWidgetItem>


ScriptPage::ScriptPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(5);
  hbox->setSpacing(2);
  vbox->addLayout(hbox);

  allButton = new QToolButton;
  allButton->setToolTip(tr("Show all scripts"));
  allButton->setIcon(QIcon(asterisk_xpm));
  connect(allButton, SIGNAL(clicked()), this, SLOT(showAllScripts()));
  allButton->setMaximumSize(25, 25);
  hbox->addWidget(allButton);

  queueButton = new QToolButton;
  queueButton->setToolTip(tr("Show script queue"));
  queueButton->setIcon(QIcon(script_xpm));
  connect(queueButton, SIGNAL(clicked()), this, SLOT(showQueue()));
  queueButton->setMaximumSize(25, 25);
  hbox->addWidget(queueButton);

  searchButton = new QToolButton;
  searchButton->setToolTip(tr("Search"));
  searchButton->setIcon(QIcon(search_xpm));
  connect(searchButton, SIGNAL(clicked()), this, SLOT(search()));
  searchButton->setMaximumSize(25, 25);
  hbox->addWidget(searchButton);

  hbox->addStretch(1);

  list = new QListWidget;
  list->setContextMenuPolicy(Qt::CustomContextMenu);
  list->setSortingEnabled(TRUE);
  connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(doubleClick(QListWidgetItem *)));
  connect(list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(list);

  // setup the script server
  Config config;
  QString ipp, dbpp;
  config.getData(Config::IndicatorPluginPath, ipp);
  config.getData(Config::DBPluginPath, dbpp);
  scriptServer = new ExScript(ipp, dbpp);
  scriptServer->setDeleteFlag(TRUE);
  connect(scriptServer, SIGNAL(signalDone()), this, SLOT(scriptDone()));

  menu = new QMenu(this);

  status = StatusAll;

  update();
}

ScriptPage::~ScriptPage ()
{
  scriptServer->stop();
  delete scriptServer;
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
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This script already exists."));
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
  Script script;
  script.setName(d);
  db.getScript(script);

  PrefDialog *dialog = new PrefDialog;
  QString s = tr("Edit Script ") + d;
  dialog->setWindowTitle(s);
  s = tr("Settings");
  dialog->addPage(0, s);

  s = tr("Command");
  QString command = script.getCommand();
  dialog->addTextItem(0, 0, s, command);

  s = tr("Script File");
  QString file = script.getFile();
  dialog->addFileItem(1, 0, s, file);

  s = tr("Comment");
  QString comment = script.getComment();
  dialog->addTextEditItem(2, 0, s, comment);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  dialog->getItem(0, command);
  dialog->getItem(1, file);
  dialog->getItem(2, comment);

  delete dialog;

  script.setCommand(command);
  script.setFile(file);
  script.setComment(comment);
  db.setScript(script);

  if (status == StatusAll)
    update();
  
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

  QString s = item->text();

  ScriptDataBase db;
  Script script;
  script.setName(s);
  db.deleteScript(script);
  delete item;
  
  emit signalMessage(QString(tr("Script deleted.")));
}

void ScriptPage::doubleClick (QListWidgetItem *item)
{
  if (! item)
    return;

  switch (status)
  {
    case StatusQueue:
      removeScriptQueue();
      break;
    default:
    {
      int rc = QMessageBox::warning(this,
    			            tr("Qtstalker: Warning"),
			            tr("Are you sure you want to execute this script?"),
			            QMessageBox::Yes,
			            QMessageBox::No,
			            QMessageBox::NoButton);
      if (rc == QMessageBox::No)
        return;
      QString s = item->text();
      Script script;
      script.setName(s);
      ScriptDataBase db;
      db.getScript(script);
      addScriptQueue(script);
      break;
    }
  }
}

void ScriptPage::rightClick (const QPoint &)
{
  menu->clear();

  switch (status)
  {
    case StatusQueue:
      menu->addAction(QIcon(delete_xpm), tr("&Remove Script From Queue"), this, SLOT(removeScriptQueue()), QKeySequence(Qt::CTRL+Qt::Key_R));
      break;
    default:
      menu->addAction(QIcon(newchart_xpm), tr("&New Script"), this, SLOT(newScript()), QKeySequence(Qt::CTRL+Qt::Key_N));
      menu->addAction(QIcon(edit), tr("&Edit Script"), this, SLOT(editScript()), QKeySequence(Qt::CTRL+Qt::Key_E));
      menu->addAction(QIcon(delete_xpm), tr("&Delete Script"), this, SLOT(deleteScript()), QKeySequence(Qt::CTRL+Qt::Key_D));
      break;
  }

  menu->exec(QCursor::pos());
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

  status = StatusSearch;
  searchString = s;
  update();
}

void ScriptPage::update ()
{
  switch (status)
  {
    case StatusQueue:
      showQueue();
      break;
    case StatusSearch:
      showQueue();
      break;
    default:
      showAllScripts();
      break;
  }
}

void ScriptPage::showAllScripts ()
{
  status = StatusAll;

  list->clear();

  ScriptDataBase db;
  QStringList l;
  db.getScripts(l);
  list->addItems(l);
}

void ScriptPage::showQueue ()
{
  status = StatusQueue;

  list->clear();

  int loop;
  for (loop = 0; loop < scriptList.count(); loop++)
  {
    Script script = scriptList.at(loop);
    QString s = script.getName();
    if (loop == 0)
      new QListWidgetItem(QIcon(ok), s, list);
    else
      new QListWidgetItem(QIcon(que_xpm), s, list);
  }
}

void ScriptPage::showSearch ()
{
  status = StatusSearch;

  list->clear();

  ScriptDataBase db;
  QStringList l;
  db.getScriptSearch(searchString, l);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
    list->addItem(l[loop]);
}

void ScriptPage::addScriptQueue (Script &script)
{
  scriptList.append(script);
  
  emit signalMessage(QString(tr("Script ")) + script.getName() + tr(" added to que."));

  if (scriptList.count() > 1)
    return;

  if (status == StatusQueue)
    update();

  startScript();
}

void ScriptPage::scriptDone ()
{
  if (! scriptList.count())
    return;

  Script script = scriptList.at(0);
  emit signalMessage(QString(tr("Script ")) + script.getName() + tr(" completed."));
  
  scriptList.removeAt(0);
  if (! scriptList.count())
    return;

  if (status == StatusQueue)
    update();

  startScript();
}

void ScriptPage::startScript ()
{
  Script script = scriptList.at(0);
  QString command = script.getCommand() + " " + script.getFile();
  scriptServer->calculate2(command);
  emit signalMessage(QString(tr("Script ")) + script.getName() + tr(" started."));
}

void ScriptPage::removeScriptQueue ()
{
  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  QString s = item->text();
  int index = list->currentRow();

  if (index == 0)
  {
    int rc = QMessageBox::warning(this,
    			          tr("Qtstalker: Warning"),
			          tr("Script currently executing.\nAre you sure you want to remove this script?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);
    if (rc == QMessageBox::No)
      return;

    scriptServer->stop();
    
    emit signalMessage(QString(tr("Script cancelled.")));
  }

  scriptList.removeAt(index);
  delete item;
  
  emit signalMessage(QString(tr("Script removed.")));
}

