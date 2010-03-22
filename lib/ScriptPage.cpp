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

#include "../pics/asterisk.xpm"
#include "../pics/search.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
#include "../pics/ok.xpm"
#include "../pics/refresh.xpm"

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
  connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(listDoubleClick(QListWidgetItem *)));
  connect(list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(listRightClick(const QPoint &)));
  vbox->addWidget(list);

  vbox->addSpacing(5);
  
  QLabel *label = new QLabel(tr("Active Scripts"));
  vbox->addWidget(label);
  
  queList = new QListWidget;
  queList->setContextMenuPolicy(Qt::CustomContextMenu);
  queList->setSortingEnabled(TRUE);
  connect(queList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(queRightClick(const QPoint &)));
  vbox->addWidget(queList);

  menu = new QMenu(this);
  
  showAllScripts();
}

ScriptPage::~ScriptPage ()
{
  qDeleteAll(scripts);
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
  
  // check if script is refreshing
  if (scripts.contains(script->getName()))
  {
    Script *tscript = scripts.value(script->getName());
    scripts.remove(script->getName());
    delete tscript;
    
    if (script->getRefresh() > 0)
    {
      scripts.insert(script->getName(), script);
      script->start();
    }
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

  // check if script is in the refresh que
  if (scripts.contains(name))
  {
    Script *tscript = scripts.value(name);
    scripts.remove(name); // remove the refresh que hash list
    delete tscript;
    updateQueList();
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
      
  if (scripts.contains(name))
  {
    QMessageBox mb;
    mb.setText(tr("This script is currently running. Request denied."));
    mb.exec();
    return;
  }
	
  scripts.insert(name, script);
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(scriptDone(QString)));
  script->start();
  
  updateQueList();
}

void ScriptPage::listRightClick (const QPoint &)
{
  menu->clear();

  menu->addAction(QIcon(newchart_xpm), tr("&New Script"), this, SLOT(newScript()), QKeySequence(Qt::CTRL+Qt::Key_N));
  menu->addAction(QIcon(edit), tr("&Edit Script"), this, SLOT(editScript()), QKeySequence(Qt::CTRL+Qt::Key_E));
  menu->addAction(QIcon(delete_xpm), tr("&Delete Script"), this, SLOT(deleteScript()), QKeySequence(Qt::CTRL+Qt::Key_D));

  menu->exec(QCursor::pos());
}

void ScriptPage::queRightClick (const QPoint &)
{
  menu->clear();

  menu->addAction(QIcon(delete_xpm), tr("&Remove Script From Queue"), this, SLOT(removeScriptQueue()), QKeySequence(Qt::CTRL+Qt::Key_R));

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
      new QListWidgetItem(QIcon(refresh_xpm), name, queList);
    else
      new QListWidgetItem(QIcon(ok), name, queList);
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

