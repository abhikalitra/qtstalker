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

#include "ScriptDialog.h"
#include "Globals.h"
#include "CommandFactory.h"
//#include "ScriptDataBase.h"
#include "NewDialog.h"
#include "SettingGroup.h"
#include "ScriptInsertDialog.h"
#include "CommandWidget.h"

#include "../pics/delete.xpm"
#include "../pics/insert.xpm"
//#include "../pics/rename.xpm"
#include "../pics/buyarrow.xpm"
#include "../pics/sellarrow.xpm"

#include <QtDebug>
#include <QSettings>
#include <QInputDialog>
#include <QToolBar>
#include <QPushButton>
#include <QGroupBox>
#include <QFileInfo>
#include <QFileDialog>

ScriptDialog::ScriptDialog (QWidget *p, QString name) : Dialog (p)
{
  _script = new Script(this);
  _script->setFile(name);
  QFileInfo fi(name);
  _script->setName(fi.baseName());

  _newFlag = 0;
  _keySize = "procedure_dialog_window_size";
  _keyPos = "procedure_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit") << _script->name();
  setWindowTitle(l.join(" "));

  // create base tabs area
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs, 1, 0);

  createActions();
//  createSettingsPage();
  createEditPage();
  loadSettings();
//  _script->loadIndex();
  _script->loadScript();
  setGUI();
  listSelectionChanged();
}

/*
void ScriptDialog::createSettingsPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs, 1, 0);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  QStringList l;
  l << tr("System") << tr("Indicator");

  _type = new QComboBox;
  _type->addItems(l);
  form->addRow(tr("Type"), _type);

  _startup = new QCheckBox;
  form->addRow(tr("Launch at application start"), _startup);

  _interval = new QSpinBox;
  form->addRow(tr("Launch every X seconds"), _interval);

  _comment = new QTextEdit;
  form->addRow(tr("Comment"), _comment);

  _tabs->addTab(w, tr("Settings"));
}
*/

void ScriptDialog::createEditPage ()
{
  QWidget *w = new QWidget;

  QHBoxLayout *thbox = new QHBoxLayout;
  thbox->setSpacing(0);
  w->setLayout(thbox);

  _splitter = new QSplitter;
  thbox->addWidget(_splitter);

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Commands"));
  _splitter->addWidget(gbox);

  thbox = new QHBoxLayout;
  thbox->setSpacing(0);
  gbox->setLayout(thbox);

  // left side list
  _list = new QListWidget;
  _list->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(listSelectionChanged()));
  connect(_list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  thbox->addWidget(_list);

  // list toolbar
  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  tb->addAction(_insertAction);
//  tb->addAction(_renameAction);
  tb->addSeparator();
  tb->addAction(_upAction);
  tb->addAction(_downAction);
  tb->addSeparator();
  tb->addAction(_deleteAction);
  thbox->addWidget(tb);

  gbox = new QGroupBox;
  gbox->setTitle(tr("Command Settings"));
  _splitter->addWidget(gbox);

  thbox = new QHBoxLayout;
  gbox->setLayout(thbox);

  // right side container
  _stack = new QStackedWidget;
  connect(_list, SIGNAL(currentRowChanged(int)), _stack, SLOT(setCurrentIndex(int)));
  thbox->addWidget(_stack);

  // apply button
  QPushButton *pb = _buttonBox->addButton(QString("A&pply"), QDialogButtonBox::ActionRole);
  pb->setToolTip(tr("Apply Changes"));
  connect(pb, SIGNAL(clicked(bool)), this, SLOT(apply()));

  // command context menu
  _menu = new QMenu(this);
  _menu->addAction(_insertAction);
//  _menu->addAction(_renameAction);
  _menu->addSeparator();
  _menu->addAction(_upAction);
  _menu->addAction(_downAction);
  _menu->addSeparator();
  _menu->addAction(_deleteAction);

  _tabs->addTab(w, tr("Script"));
}

void ScriptDialog::createActions ()
{
  // insert
  _insertAction = new QAction(QIcon(insert_xpm), tr("&Insert Command") + "...", this);
  _insertAction->setToolTip(tr("Insert Command") + "...");
  _insertAction->setStatusTip(tr("Insert Command") + "...");
  connect(_insertAction, SIGNAL(triggered(bool)), this, SLOT(insertItem()));

  // delete
  _deleteAction = new QAction(QIcon(delete_xpm), tr("&Remove Command"), this);
  _deleteAction->setToolTip(tr("Remove Command"));
  _deleteAction->setStatusTip(tr("Remove Command"));
  connect(_deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteItem()));

  // rename
//  _renameAction = new QAction(QIcon(rename_xpm), tr("&Rename Command") + "...", this);
//  _renameAction->setToolTip(tr("Rename Command") + "...");
//  _renameAction->setStatusTip(tr("Rename Command") + "...");
//  connect(_renameAction, SIGNAL(triggered(bool)), this, SLOT(renameItem()));

  // up
  _upAction = new QAction(QIcon(buyarrow_xpm), tr("Move &Up"), this);
  _upAction->setToolTip(tr("Move Command Up"));
  _upAction->setStatusTip(tr("Move Command Up"));
  connect(_upAction, SIGNAL(triggered(bool)), this, SLOT(upItem()));

  // down
  _downAction = new QAction(QIcon(sellarrow_xpm), tr("Move &Down"), this);
  _downAction->setToolTip(tr("Move Command Down"));
  _downAction->setStatusTip(tr("Move Command Down"));
  connect(_downAction, SIGNAL(triggered(bool)), this, SLOT(downItem()));
}

void ScriptDialog::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void ScriptDialog::loadSettings ()
{
  Dialog::loadSettings();

  QList<int> sizes;

  QSettings settings(g_localSettings);
  sizes.append(settings.value("procedure_dialog_commands_width", 150).toInt());
  sizes.append(settings.value("procedure_dialog_commands_width", 300).toInt());

  _splitter->setSizes(sizes);
}

void ScriptDialog::saveSettings ()
{
  Dialog::saveSettings();

  QList<int> sizes = _splitter->sizes();

  QSettings settings(g_localSettings);
  settings.setValue("procedure_dialog_commands_width", sizes.at(0));
  settings.setValue("procedure_dialog_commands_width", sizes.at(1));
  settings.sync();
}

void ScriptDialog::done ()
{
  if (_script->name().isEmpty())
  {
    newDialog();
    return;
  }

  apply();
  saveSettings();
  accept();
}

void ScriptDialog::insertItem ()
{
  ScriptInsertDialog *dialog = new ScriptInsertDialog(this, _script);
  connect(dialog, SIGNAL(signalDone(QString, QString)), this, SLOT(insertItem2(QString, QString)));
  dialog->show();
}

void ScriptDialog::insertItem2 (QString name, QString command)
{
  CommandFactory fac;
  Command *com = fac.command(this, command);
  if (! com)
  {
    qDebug() << "ScriptDialog::insertItem2: invalid command" << command;
    return;
  }

  int index = _stack->currentIndex();

  SettingGroup *sg = com->settings();
  delete com;

  sg->setStepName(name);

  _script->setSettingGroup(sg);

  CommandWidget *cw = new CommandWidget(this, sg, _script);
  _stack->insertWidget(index, cw);

  QListWidgetItem *item = new QListWidgetItem;
  item->setText(name);
  _list->insertItem(index, item);
}

void ScriptDialog::deleteItem ()
{
  int index = _stack->currentIndex();
  if (index == -1)
    return;

  QWidget *w = _stack->currentWidget();
  if (! w)
    return;

  _stack->removeWidget(w);

  QListWidgetItem *item = _list->item(index);
  _script->removeSettingGroup(item->text());

  delete item;
  delete w;
}

int ScriptDialog::apply ()
{
/*
  bool b = _startup->isChecked();
  if (b == FALSE)
    _script->setStartup(0);
  else
    _script->setStartup(1);

  _script->setInterval(_interval->value());

  _script->setType(_type->currentText());

  _script->setComment(_comment->toPlainText());
*/

  QStringList runOrder;
  int loop = 0;
  for (; loop < _stack->count(); loop++)
  {
    CommandWidget *w = (CommandWidget *) _stack->widget(loop);
    w->save();

    QListWidgetItem *item = _list->item(loop);
    if (! item)
      continue;

    runOrder << item->text();
  }

  _script->setRunOrder(runOrder);

//  _script->saveIndex();
  _script->saveScript();

  emit signalDone();

  return 0;
}

void ScriptDialog::setGUI ()
{
  if (_script->name().isEmpty())
    return;

/*
  bool b = FALSE;
  if (_script->startup() > 0)
    b = TRUE;
  _startup->setChecked(b);

  _interval->setValue(_script->interval());

  _type->setCurrentIndex(_type->findText(_script->type()));

  _comment->setPlainText(_script->comment());
*/

  QStringList runOrder = _script->runOrder();

  int loop = 0;
  for (; loop < runOrder.count(); loop++)
  {
    SettingGroup *sg = _script->settingGroup(runOrder.at(loop));
    if (! sg)
      continue;

    _list->addItem(runOrder.at(loop));

    CommandWidget *cw = new CommandWidget(this, sg, _script);
    _stack->addWidget(cw);
  }
}

/*
void ScriptDialog::renameItem ()
{
  int index = _stack->currentIndex();
  if (index == -1)
    return;

  QListWidgetItem *item = _list->item(index);

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setTextValue(item->text());
  dialog->setLabelText(tr("Enter new name"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(renameItem2(QString)));
  dialog->show();
}

void ScriptDialog::renameItem2 (QString d)
{
  int index = _stack->currentIndex();
  if (index == -1)
    return;

  QListWidgetItem *item = _list->item(index);

  _script->renameStep(item->text(), d);

  item->setText(d);
}
*/

void ScriptDialog::listSelectionChanged ()
{
  bool status = TRUE;
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    status = FALSE;

  _deleteAction->setEnabled(status);
//  _renameAction->setEnabled(status);

  if (_list->currentRow() > 0)
    _upAction->setEnabled(TRUE);
  else
    _upAction->setEnabled(FALSE);

  if (_list->currentRow() == _list->count() -1)
    _downAction->setEnabled(FALSE);
  else
    _downAction->setEnabled(TRUE);
}

void ScriptDialog::upItem ()
{
  int currentRow = _list->currentRow();
  if (currentRow == 0)
    return;
  int newRow = currentRow;
  newRow--;
  moveItem(currentRow, newRow);
}

void ScriptDialog::downItem ()
{
  int currentRow = _list->currentRow();
  if (currentRow == _list->count() - 1)
    return;
  int newRow = currentRow;
  newRow++;
  moveItem(currentRow, newRow);
}

void ScriptDialog::moveItem (int oldRow, int newRow)
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;

  QListWidgetItem *item = _list->takeItem(oldRow);
  _list->insertItem(newRow, item);

  QWidget *w = _stack->widget(oldRow);
  _stack->removeWidget(w);
  _stack->insertWidget(newRow, w);

  _list->setCurrentRow(newRow);
}

void ScriptDialog::newDialog ()
{
  QSettings settings(g_localSettings);

  QFileDialog *dialog = new QFileDialog(this);
  dialog->setAcceptMode(QFileDialog::AcceptSave);
  dialog->setWindowTitle("QtStalker" + g_session + ": " + tr("Save Script As..."));
  dialog->setDirectory(settings.value("script_panel_last_external_script").toString());
  connect(dialog, SIGNAL(fileSelected(const QString &)), this, SLOT(newDialog2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ScriptDialog::newDialog2 (QString d)
{
  QFileInfo fi(d);
  _script->setName(fi.baseName());
  _script->setFile(d);

  QSettings settings(g_localSettings);
  settings.setValue("script_panel_last_external_script", d);
  settings.sync();

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit") << _script->name();
  setWindowTitle(l.join(" "));

  _newFlag = 1;

  done();
}
