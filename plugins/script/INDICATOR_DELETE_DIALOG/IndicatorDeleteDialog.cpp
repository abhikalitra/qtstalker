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

#include "IndicatorDeleteDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "Command.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>

IndicatorDeleteDialog::IndicatorDeleteDialog ()
{
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("Delete Indicator"));

  createGUI();

  loadIndicators();

  loadSettings();

  selectionChanged();
}

void IndicatorDeleteDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  vbox->addWidget(_list);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setDefault(TRUE);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void IndicatorDeleteDialog::selectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (sl.count())
    status = 1;

  _okButton->setEnabled(status);
}

void IndicatorDeleteDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void IndicatorDeleteDialog::done ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  ScriptPluginFactory fac;

  QStringList deleted;
  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);

    QStringList l;
    l << "INDICATOR_DATABASE" << "DELETE" << item->text();
    
    Command command(l.join(","));

    ScriptPlugin *plug = fac.plugin(command.plugin());
    if (! plug)
    {
      qDebug() << "IndicatorDeleteDialog::done: no plugin" << command.plugin();
      return;
    }
    
    plug->command(command);
    delete plug;

    deleted << item->text();
  }
  
  saveSettings();

  emit signalDone(deleted.join(","));
  
  accept();
}

void IndicatorDeleteDialog::cancel ()
{
  saveSettings();
  reject();
}

void IndicatorDeleteDialog::loadIndicators ()
{
  Command command("INDICATOR_DATABASE,INDICATORS");

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "IndicatorDeleteDialog::loadIndicators: no plugin" << command.plugin();
    return;
  }

  plug->command(command);

  QStringList l = command.stringData().split(",", QString::SkipEmptyParts);
  
  _list->clear();
  _list->addItems(l);
}

void IndicatorDeleteDialog::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("indicator_delete_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("indicator_delete_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void IndicatorDeleteDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("indicator_delete_dialog_window_size", size());
  settings.setValue("indicator_delete_dialog_window_position", pos());
  settings.sync();
}
