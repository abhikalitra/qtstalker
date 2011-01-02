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

#include "YahooSymbolDialog.h"
#include "Globals.h"
#include "YahooAddSymbolDialog.h"
#include "Doc.h"
#include "ScriptPluginFactory.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QLayout>
#include <QDebug>
#include <QIcon>
#include <QDialogButtonBox>
#include <QSettings>

YahooSymbolDialog::YahooSymbolDialog ()
{
  _helpFile = "Yahoo.html";

  QStringList l;
  l << "QtStalker" << g_session << ":" << "Yahoo Symbols";
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  selectionChanged();
  
  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void YahooSymbolDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(5);
  vbox->addLayout(hbox);

  _list = new QListWidget;
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  hbox->addWidget(_list);

  QVBoxLayout *tbbox = new QVBoxLayout;
  tbbox->setSpacing(2);
  tbbox->setMargin(0);
  hbox->addLayout(tbbox);

  QPushButton *b = new QPushButton;
  connect(b, SIGNAL(clicked()), this, SLOT(addSymbol()));
  b->setText(tr("Add..."));
  b->setIcon(QIcon(add_xpm));
  tbbox->addWidget(b);
  
  _deleteButton = new QPushButton;
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteSymbol()));
  _deleteButton->setText(tr("Delete"));
  _deleteButton->setIcon(QIcon(delete_xpm));
  tbbox->addWidget(_deleteButton);
  
  tbbox->addStretch();

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

  // help button
  b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void YahooSymbolDialog::addSymbol ()
{
  YahooAddSymbolDialog *dialog = new YahooAddSymbolDialog;
  connect(dialog, SIGNAL(signalNew()), this, SLOT(loadSettings()));
  dialog->show();
}
  
void YahooSymbolDialog::deleteSymbol ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin("YAHOO_DATABASE");
  if (! plug)
  {
    qDebug() << "YahooSymbolDialog::deleteSymbol: no plugin";
    return;
  }

  QStringList cl;
  cl << "YAHOO_DATABASE" << "TRANSACTION";
  Command command(cl.join(","));

  if (plug->command(&command))
    qDebug() << "YahooAddSymbolDialog::deleteSymbol: command error";

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    cl.clear();
    cl << "YAHOO_DATABASE" << "DELETE" << l.at(loop)->text();
    command.parse(cl.join(","));

    if (plug->command(&command))
      qDebug() << "YahooSymbolDialog::deleteSymbol: command error";
  }
  
  cl.clear();
  cl << "YAHOO_DATABASE" << "COMMIT";
  command.parse(cl.join(","));

  if (plug->command(&command))
    qDebug() << "YahooAddSymbolDialog::deleteSymbol: command error";

  delete plug;
}

void YahooSymbolDialog::loadSettings ()
{
  _list->clear();
  
  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin("YAHOO_DATABASE");
  if (! plug)
  {
    qDebug() << "YahooSymbolDialog::loadSettings: no plugin";
    return;
  }

  QStringList l;
  l << "YAHOO_DATABASE" << "SYMBOLS";
  Command command(l.join(","));

  if (plug->command(&command))
    qDebug() << "YahooSymbolDialog::loadSettings: command error";

  delete plug;

  l.clear();
  l = command.stringData().split(",", QString::SkipEmptyParts);

  _list->addItems(l);

  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("yahoo_symbol_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("yahoo_symbol_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void YahooSymbolDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("yahoo_symbol_dialog_window_size", size());
  settings.setValue("yahoo_symbol_dialog_window_position", pos());
  settings.sync();
}

void YahooSymbolDialog::done ()
{
  QList<QListWidgetItem *> sel = _list->selectedItems();
  if (! sel.count())
  {
    accept();
    return;
  }

  QStringList l;
  int loop = 0;
  for (; loop < sel.count(); loop++)
    l.append(sel.at(loop)->text());

  emit signalSymbols(l);

  accept();
}

void YahooSymbolDialog::selectionChanged ()
{
  QList<QListWidgetItem *> sel = _list->selectedItems();
  if (! sel.count())
    _deleteButton->setEnabled(FALSE);
  else
    _deleteButton->setEnabled(TRUE);
}

void YahooSymbolDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void YahooSymbolDialog::cancel ()
{
  saveSettings();
  reject();
}
