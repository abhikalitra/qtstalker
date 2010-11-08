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

#include "ScriptDialog.h"
#include "ScriptDataBase.h"
#include "ScriptConfig.h"
#include "Globals.h"
#include "ScriptNewDialog.h"
#include "ScriptEditDialog.h"
#include "ScriptSearchDialog.h"
#include "ListDialog.h"

#include "../pics/edit.xpm"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "../pics/disable.xpm"

#include <QLayout>

ScriptDialog::ScriptDialog ()
{
//  _helpFile = "Script.html";
  
  setWindowTitle("QtStalker" + g_session + ": " + tr("Script Manager"));

  // buttons
  _buttonBox->removeButton(_okButton);
  _buttonBox->removeButton(_cancelButton);

  QPushButton *b = new QPushButton;
  b->setText(tr("Close"));
  b->setIcon(QIcon(disable_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(closeDialog()));
  _buttonBox->addButton(b, QDialogButtonBox::ActionRole);

  createMainPage();

  createLogPage();

  loadSettings();
}

void ScriptDialog::createMainPage ()
{
  QWidget *w = new QWidget;
  
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  w->setLayout(hbox);

  QStringList l;
  l << tr("Name"); // << tr("Status") << tr("Last Run");
  
  _list = new QTreeWidget;
  _list->setSortingEnabled(TRUE);
  _list->setRootIsDecorated(FALSE);
  _list->setHeaderLabels(l);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  hbox->addWidget(_list);

  // create button box
  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setMargin(0);
  bbox->setSpacing(2);
  hbox->addLayout(bbox);

  QPushButton *b = new QPushButton;
  b->setText(tr("&Run..."));
//  b->setIcon(QPixmap(newchart_xpm));
  b->setToolTip(tr("Run script..."));
  connect(b, SIGNAL(clicked()), this, SLOT(runScript()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&New..."));
  b->setIcon(QPixmap(newchart_xpm));
  b->setToolTip(tr("Create a new script..."));
  connect(b, SIGNAL(clicked()), this, SLOT(newScript()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Edit..."));
  b->setIcon(QPixmap(edit_xpm));
  b->setToolTip(tr("Edit script..."));
  connect(b, SIGNAL(clicked()), this, SLOT(editScript()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Delete"));
  b->setToolTip(tr("Delete script..."));
  b->setIcon(QPixmap(delete_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(deleteScript()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Cancel"));
  b->setToolTip(tr("Cancel script..."));
//  b->setIcon(QPixmap(delete_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(cancelScript()));
  bbox->addWidget(b);

  bbox->addStretch();

  _tabs->addTab(w, tr("Scripts"));
}

void ScriptDialog::createLogPage ()
{
  QWidget *w = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  w->setLayout(hbox);

  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  hbox->addWidget(_log);

  _tabs->addTab(w, tr("Log"));
}

void ScriptDialog::loadSettings ()
{
  ScriptConfig config;

  // restore the size of the window
  QSize sz;
  config.getData(ScriptConfig::_Size, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(ScriptConfig::_Pos, p);
  if (! p.isNull())
    move(p);
}

void ScriptDialog::saveSettings ()
{
  ScriptConfig config;
  config.transaction();
  
  // save app size and position
  QSize sz = size();
  config.setData(ScriptConfig::_Size, sz);

  QPoint pt = pos();
  config.setData(ScriptConfig::_Pos, pt);

  config.commit();
}

void ScriptDialog::newScript ()
{
  ScriptNewDialog *dialog = new ScriptNewDialog;
  dialog->show();
}

void ScriptDialog::editScript ()
{
  ScriptSearchDialog *dialog = new ScriptSearchDialog;
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(editScript2(QStringList)));
  dialog->show();
}

void ScriptDialog::editScript2 (QStringList l)
{
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    ScriptEditDialog *dialog = new ScriptEditDialog(l.at(loop));
    dialog->show();
  }
}

void ScriptDialog::deleteScript ()
{
  ScriptSearchDialog *dialog = new ScriptSearchDialog;
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(deleteScript2(QStringList)));
  dialog->show();
}

void ScriptDialog::deleteScript2 (QStringList l)
{
  ScriptDataBase db;
  
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    ScriptItem script;
    script.setName(l.at(loop));
    db.deleteScript(&script);
  }
}

void ScriptDialog::runScript ()
{
  ScriptSearchDialog *dialog = new ScriptSearchDialog;
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(runScript2(QStringList)));
  dialog->show();
}

void ScriptDialog::runScript2 (QStringList l)
{
  ScriptDataBase db;

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    if (_running.contains(l.at(loop)))
      continue;

    ScriptItem *script = new ScriptItem;
    script->setName(l.at(loop));
    connect(script, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
    connect(script, SIGNAL(signalStopped(QString)), this, SLOT(scriptStopped(QString)));
    connect(script, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
    
    ScriptDataBase db;
    db.getScript(script);

    _scripts.insert(l.at(loop), script);
    
    script->start();

    QTreeWidgetItem *item = new QTreeWidgetItem(_list);
    item->setText(0, l.at(loop));
    _running.insert(l.at(loop), item);
  }
}

void ScriptDialog::cancelScript ()
{
  ListDialog *dialog = new ListDialog;
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(cancelScript2(QStringList)));

  QStringList l;
  QHashIterator<QString, ScriptItem *> it(_scripts);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }

  dialog->setList(l);
  
  dialog->show();
}

void ScriptDialog::cancelScript2 (QStringList l)
{
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    ScriptItem *script = _scripts.value(l.at(loop));
    if (! script)
      continue;
  
    script->stop();
  }
}

void ScriptDialog::done (QString name)
{
  QTreeWidgetItem *item = _running.value(name);
  if (! item)
    return;

  _running.remove(name);
  delete item;
  
  ScriptItem *script = _scripts.value(name);

  ScriptDataBase db;
  db.transaction();
  db.setScript(script);
  db.commit();

  _scripts.remove(name);
  delete script;
}

void ScriptDialog::closeDialog ()
{
  saveSettings();
  hide();
}

void ScriptDialog::resizeColumns ()
{
  int loop = 0;
  for (; loop < _list->topLevelItemCount(); loop++)
    _list->resizeColumnToContents(loop);
}

