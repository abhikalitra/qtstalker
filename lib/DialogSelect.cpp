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

#include "DialogSelect.h"
#include "WindowTitle.h"

#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"

#include <QtDebug>
#include <QToolBar>
#include <QToolButton>

DialogSelect::DialogSelect (QWidget *p, QString id, Entity settings) : Dialog (p)
{
  _id = id;
  _settings = settings;
  _keySize = "select_dialog_window_size";
  _keyPos = "select_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Select"), QString()));

  createGUI();
  
  if (! _id.isEmpty())
  {
    Data list, title, mode;
    _settings.toData(QString("LIST"), list);
    _settings.toData(QString("TITLE"), title);
    _settings.toData(QString("MODE"), mode);
    
    setItems(list.toList());
    setTitle(title.toString());
    setMode(mode.toInteger());
  }
  
  loadSettings();

  selectionChanged();
}

void DialogSelect::createGUI ()
{
  int pos = 0;
  _title = new QLabel;
  _vbox->insertWidget(pos++, _title);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  _vbox->insertLayout(pos++, hbox);

  // list
  _list = new QListWidget;
  _list->setSortingEnabled(TRUE);
  _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(done()));
  hbox->addWidget(_list);

  QToolBar *tb = new QToolBar;
  tb->setOrientation(Qt::Vertical);
  hbox->addWidget(tb);

  // select all button
  QToolButton *b = new QToolButton;
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

  // clear some uneeded space
  _message->hide();
}

void DialogSelect::selectionChanged ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();
  _okButton->setEnabled(sl.count());
}

void DialogSelect::done ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  QStringList l;
  int loop = 0;
  for (; loop < sl.count(); loop++)
    l << sl.at(loop)->text();

  _saveFlag++;
  _settings.set(QString("LIST"), Data(l));
  
  saveSettings();

  emit signalDone(l);
  _selected = l;

  accept();
}

void DialogSelect::setItems (QStringList l)
{
  _list->clear();
  _list->addItems(l);
}

void DialogSelect::setMode (int mode)
{
  if (mode)
    _list->setSelectionMode(QAbstractItemView::SingleSelection);
}

void DialogSelect::setTitle (QString d)
{
  _title->setText(d);
}

QStringList DialogSelect::selected ()
{
  return _selected;
}
