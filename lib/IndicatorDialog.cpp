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

#include "IndicatorDialog.h"
#include "Globals.h"
#include "ScriptDataBase.h"
#include "DockWidget.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"

#include <QtDebug>
#include <QSettings>
#include <QToolBar>
#include <QFileDialog>

IndicatorDialog::IndicatorDialog (QWidget *p) : Dialog (p)
{
  _keySize = "indicator_dialog_window_size";
  _keyPos = "indicator_dialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Indicators");
  setWindowTitle(l.join(" "));

  createGUI();

  QSettings settings(g_localSettings);
  l = settings.value("indicators").toStringList();

  _list->clear();
  _list->addItems(l);

  loadSettings();

  selectionChanged();
}

void IndicatorDialog::createGUI ()
{
  int pos = 0;
  QLabel *label = new QLabel(tr("Indicators"));
  _vbox->insertWidget(pos++, label);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  _vbox->insertLayout(pos++, hbox);

  _list = new QListWidget;
  _list->setSortingEnabled(FALSE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  hbox->addWidget(_list);

  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  hbox->addWidget(tb);

  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(add_xpm));
  b->setToolTip(tr("Add Indicators"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addButtonPressed()));
  tb->addWidget(b);

  _deleteButton = new QToolButton;
  _deleteButton->setIcon(QIcon(delete_xpm));
  _deleteButton->setToolTip(tr("Delete Indicators"));
  connect(_deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteButtonPressed()));
  tb->addWidget(_deleteButton);

  // select all button
  b = new QToolButton;
  b->setIcon(QIcon(select_all_xpm));
  b->setToolTip(tr("Select All"));
  connect(b, SIGNAL(clicked(bool)), _list, SLOT(selectAll()));
  tb->addWidget(b);

  // unselect all button
  b = new QToolButton;
  b->setIcon(QIcon(unselect_all_xpm));
  b->setToolTip(tr("Unselect All"));
  connect(b, SIGNAL(clicked(bool)), _list, SLOT(clearSelection()));
  tb->addWidget(b);

  // clear some unused space
  _message->hide();
}

void IndicatorDialog::selectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _list->selectedItems();
  if (sl.count())
    status = 1;

  _deleteButton->setEnabled(status);
}

void IndicatorDialog::done ()
{
  QStringList nl;
  int loop = 0;
  for (; loop < _list->count(); loop++)
    nl << _list->item(loop)->text();

  _removeList.removeDuplicates();

  QList<QString> pl = g_plotGroup->plots();
  for (loop = 0; loop < pl.count(); loop++)
  {
    Plot *p = g_plotGroup->plot(pl.at(loop));
    if (! p)
      continue;

    if (_removeList.indexOf(p->scriptFile()) == -1)
      continue;

    g_plotGroup->removePlot(pl.at(loop));
  }

  QSettings settings(g_localSettings);
  settings.setValue("indicators", nl);
  settings.sync();

  saveSettings();

  g_sidePanel->reloadChart();

  accept();
}

void IndicatorDialog::addButtonPressed ()
{
  QSettings settings(g_localSettings);

  QFileDialog *dialog = new QFileDialog(this);
  dialog->setWindowTitle("QtStalker" + g_session + ": " + tr("Select Indicator Script"));
  dialog->setDirectory(settings.value("indicator_dialog_last_script").toString());
  connect(dialog, SIGNAL(fileSelected(const QString &)), this, SLOT(addButtonPressed2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void IndicatorDialog::addButtonPressed2 (QString d)
{
  _list->addItem(d);

  QSettings settings(g_localSettings);
  settings.setValue("indicator_dialog_last_script", d);
  settings.sync();
}

void IndicatorDialog::deleteButtonPressed ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QListWidgetItem *item = l.at(loop);
    _removeList << item->text();
    delete item;
  }
}
