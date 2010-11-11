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

#include "PluginPage.h"
#include "Config.h"
#include "MiscPlugin.h"

#include "../pics/configure.xpm"

#include <QCursor>
#include <QToolTip>
#include <QPixmap>
#include <QVBoxLayout>
#include <QIcon>
#include <QtDebug>

PluginPage::PluginPage ()
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
  
  _list = new QListWidget;
  _list->setContextMenuPolicy(Qt::CustomContextMenu);
  _list->setSortingEnabled(TRUE);
  connect(_list, SIGNAL(itemSelectionChanged()), this, SLOT(listStatus()));
  connect(_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(doubleClick(QListWidgetItem *)));
  connect(_list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(_list);

  initPlugins();

  listStatus();
}

void PluginPage::createActions ()
{
  QAction *action  = new QAction(QIcon(configure_xpm), tr("&Configure Plugin") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_C));
  action->setToolTip(tr("Configure plugin") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(configure()));
  _actions.insert(Configure, action);
}

void PluginPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(_actions.value(Configure));

  _menu = new QMenu(this);
  _menu->addAction(_actions.value(Configure));
}

void PluginPage::configure ()
{
  QListWidgetItem *item = _list->currentItem();
  if (! item)
    return;

  configure(item->text());
}

void PluginPage::configure (QString name)
{
  MiscPlugin *plug = _fac.plugin(name);
  if (! plug)
    return;

  plug->configureDialog();
}

void PluginPage::doubleClick (QListWidgetItem *item)
{
  if (! item)
    return;

  configure(item->text());
}

void PluginPage::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void PluginPage::initPlugins ()
{
  _list->clear();

  Config config;
  QStringList l;
  config.getData(Config::MiscPluginList, l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    MiscPlugin *plug = _fac.plugin(l[loop]);
    if (! plug)
      return;

    connect(plug, SIGNAL(signalChartRefresh()), this, SIGNAL(signalChartRefresh()));
    connect(plug, SIGNAL(signalGroupRefresh()), this, SIGNAL(signalGroupRefresh()));

    QString s = plug->name();
    QString s2 = plug->description();
    
    QListWidgetItem *item = new QListWidgetItem(s, _list, 0);
    item->setToolTip(s2);

    plug->initDialog();
  }
}

void PluginPage::listStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (l.count())
    status = TRUE;
  
  _actions.value(Configure)->setEnabled(status);
}

