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

#include "PluginEditDialog.h"
#include "Globals.h"
#include "PluginFactory.h"
#include "DataDataBase.h"
#include "NewDialog.h"
#include "PluginWidget.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"
#include "../pics/insert.xpm"
#include "../pics/rename.xpm"
#include "../pics/buyarrow.xpm"
#include "../pics/sellarrow.xpm"

#include <QtDebug>
#include <QSettings>
#include <QInputDialog>
#include <QToolBar>
#include <QPushButton>
#include <QGroupBox>

PluginEditDialog::PluginEditDialog (QWidget *p, QString n, QString t) : Dialog (p)
{
  _name = n;
  _table = t;
  _keySize = "plugin_edit_dialog_window_size";
  _keyPos = "plugin_edit_dialog_window_position";
  _status = _NONE;
  _newFlag = 0;

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit") << _name;
  setWindowTitle(l.join(" "));

  createActions();
  createGUI();
  loadSettings();
  loadPlugins();
  listSelectionChanged();
}

PluginEditDialog::~PluginEditDialog ()
{
  qDeleteAll(_settings);
}

void PluginEditDialog::createGUI ()
{
  _splitter = new QSplitter;
  _vbox->insertWidget(0, _splitter, 1, 0);
  
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Commands"));
  _splitter->addWidget(gbox);

  QHBoxLayout *thbox = new QHBoxLayout;
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
  tb->addAction(_addAction);
  tb->addAction(_insertAction);
  tb->addAction(_renameAction);
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
  _menu->addAction(_addAction);
  _menu->addAction(_insertAction);
  _menu->addAction(_renameAction);
  _menu->addSeparator();
  _menu->addAction(_upAction);
  _menu->addAction(_downAction);
  _menu->addSeparator();
  _menu->addAction(_deleteAction);
}

void PluginEditDialog::createActions ()
{
  // add
  _addAction = new QAction(QIcon(add_xpm), tr("&Add Command") + "...", this);
  _addAction->setToolTip(tr("Add Command") + "...");
  _addAction->setStatusTip(tr("Add Command") + "...");
  connect(_addAction, SIGNAL(triggered(bool)), this, SLOT(addItem()));

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
  _renameAction = new QAction(QIcon(rename_xpm), tr("&Rename Command") + "...", this);
  _renameAction->setToolTip(tr("Rename Command") + "...");
  _renameAction->setStatusTip(tr("Rename Command") + "...");
  connect(_renameAction, SIGNAL(triggered(bool)), this, SLOT(renameItem()));

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

void PluginEditDialog::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void PluginEditDialog::loadSettings ()
{
  Dialog::loadSettings();

  QList<int> sizes;
  
  QSettings settings(g_localSettings);
  sizes.append(settings.value("plugin_edit_dialog_commands_width", 150).toInt());
  sizes.append(settings.value("plugin_edit_dialog_settings_width", 300).toInt());

  _splitter->setSizes(sizes);
}

void PluginEditDialog::saveSettings ()
{
  Dialog::saveSettings();

  QList<int> sizes = _splitter->sizes();
  
  QSettings settings(g_localSettings);
  settings.setValue("plugin_edit_dialog_commands_width", sizes.at(0));
  settings.setValue("plugin_edit_dialog_settings_width", sizes.at(1));
  settings.sync();
}

void PluginEditDialog::done ()
{
  _status = _DONE;
  if (applySave())
    return;
  saveSettings();
  accept();
}

void PluginEditDialog::addItem ()
{
  QStringList l;
  QSettings set(g_globalSettings);
  l = set.value("plugins").toStringList();
  l.sort();

  QSettings set2(g_localSettings);
  QString s = set2.value("plugin_edit_dialog_last_indicator", 0).toString();
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setTextValue(s);
  dialog->setLabelText(tr("Select plugin"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(addItem2(QString)));
  dialog->show();
}

void PluginEditDialog::addItem2 (QString d)
{
  PluginFactory fac;
  Plugin *plug = fac.plugin(d);
  if (! plug)
  {
    qDebug() << "PluginEditDialog::addItem2: no plugin" << d;
    return;
  }

  QSettings settings(g_localSettings);
  settings.setValue("plugin_edit_dialog_last_indicator", d);
  settings.sync();

  Setting *set = new Setting;
  _settings.append(set);
  plug->settings(set);

  PluginWidget *w = new PluginWidget(this);
  w->setWidgets(set);
  _stack->addWidget(w);

  _list->addItem(d);
  
  delete plug;
}

void PluginEditDialog::insertItem ()
{
  QStringList l;
  QSettings set(g_globalSettings);
  l = set.value("plugins").toStringList();
  l.sort();

  QSettings set2(g_localSettings);
  QString s = set2.value("plugin_edit_dialog_last_indicator", 0).toString();

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setTextValue(s);
  dialog->setLabelText(tr("Select plugin"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(insertItem2(QString)));
  dialog->show();
}

void PluginEditDialog::insertItem2 (QString d)
{
  PluginFactory fac;
  Plugin *plug = fac.plugin(d);
  if (! plug)
  {
    qDebug() << "PluginEditDialog::insertItem2: no plugin" << d;
    return;
  }

  QSettings settings(g_localSettings);
  settings.setValue("plugin_edit_dialog_last_indicator", d);
  settings.sync();

  int index = _stack->currentIndex();
  Setting *set = new Setting;
  _settings.insert(index, set);
  plug->settings(set);

  PluginWidget *w = new PluginWidget(this);
  w->setWidgets(set);
  _stack->insertWidget(index, w);

  _list->insertItem(index, d);

  delete plug;
}

void PluginEditDialog::deleteItem ()
{
  int index = _stack->currentIndex();
  if (index == -1)
    return;

  QWidget *w = _stack->currentWidget();
  if (! w)
    return;
  
  _stack->removeWidget(w);

  Setting *set = _settings.at(index);
  delete set;
  _settings.removeAt(index);

  QListWidgetItem *item = _list->item(index);
  delete item;

  delete w;
}

void PluginEditDialog::newDialog ()
{
  QStringList l;
  DataDataBase db(_table);
  db.names(l);

  NewDialog *dialog = new NewDialog(this);
  dialog->setItems(l);
  dialog->setTitle(tr("Enter a unique name"));
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(newDialog2(QString)));
  dialog->show();
}

void PluginEditDialog::newDialog2 (QString d)
{
  _name = d;

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit") << _name;
  setWindowTitle(l.join(" "));

  _newFlag = 1;

  if (_status == _DONE)
    done();
  else if (_status == _APPLY)
    applySave();
}

void PluginEditDialog::apply ()
{
  _status = _APPLY;
  applySave();
}

int PluginEditDialog::applySave ()
{
  if (_name.isEmpty())
  {
    newDialog();
    return 1;
  }

  QStringList l;
  int loop = 0;
  for (; loop < _stack->count(); loop++)
  {
    PluginWidget *w = (PluginWidget *) _stack->widget(loop);
    w->save();

    QListWidgetItem *item = _list->item(loop);
    if (! item)
      continue;
    
    Setting *set = _settings.at(loop);
    set->setData("STEP_NAME", item->text());
    
    QString s;
    set->string(s);
    l << s;
  }

  if (_table == "indicators")
  {
    Indicator i;
    i.setName(_name);
    i.load();
    i.setCommands(l);
    i.save();

    if (_newFlag)
      i.add(_name);
  }
  else
  {
    DataDataBase db(_table);
    db.transaction();
    db.save(_name, l);
    db.commit();
  }

  saveSettings();
  
  if (_newFlag)
  {
    if (_table == "indicators")
      g_middleMan->indicatorNew(_name);
  }

  emit signalDone();

  return 0;
}

void PluginEditDialog::loadPlugins ()
{
  if (_name.isEmpty())
    return;

  DataDataBase db(_table);
  QStringList l;
  db.load(_name, l);
  
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    Setting *set = new Setting;
    set->parse(l.at(loop));
    if (set->data("PLUGIN").isEmpty())
    {
      delete set;
      continue;
    }
    _settings.append(set);

    PluginWidget *w = new PluginWidget(this);
    w->setWidgets(set);
    _stack->addWidget(w);

    QString s = set->data("STEP_NAME");
    if (s.isEmpty())
      s = set->data("PLUGIN");
    _list->addItem(s);
  }
}

void PluginEditDialog::renameItem ()
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

void PluginEditDialog::renameItem2 (QString d)
{
  int index = _stack->currentIndex();
  if (index == -1)
    return;

  QListWidgetItem *item = _list->item(index);
  item->setText(d);
}

void PluginEditDialog::listSelectionChanged ()
{
  bool status = TRUE;
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    status = FALSE;

  _deleteAction->setEnabled(status);
  _renameAction->setEnabled(status);
  _upAction->setEnabled(status);
  _downAction->setEnabled(status);
}

void PluginEditDialog::upItem ()
{
  int currentRow = _list->currentRow();
  if (currentRow == 0)
    return;
  int newRow = currentRow;
  newRow--;
  moveItem(currentRow, newRow);
}

void PluginEditDialog::downItem ()
{
  int currentRow = _list->currentRow();
  if (currentRow == 0)
    return;
  int newRow = currentRow;
  newRow++;
  moveItem(currentRow, newRow);
}

void PluginEditDialog::moveItem (int oldRow, int newRow)
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;

  QListWidgetItem *item = _list->takeItem(oldRow);
  _list->insertItem(newRow, item);

  _settings.move(oldRow, newRow);

  QWidget *w = _stack->widget(oldRow);
  _stack->removeWidget(w);
  _stack->insertWidget(newRow, w);

  _list->setCurrentRow(newRow);
}
