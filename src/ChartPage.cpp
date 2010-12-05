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

#include "ChartPage.h"
#include "Globals.h"

#include "../pics/add.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"

#include <QCursor>
#include <QToolTip>
#include <QLayout>
#include <QMenu>
#include <QKeyEvent>
#include <QtDebug>
#include <QList>
#include <QSettings>

ChartPage::ChartPage ()
{
  setObjectName("ChartPage");
  
  createActions();

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);
  tb->setIconSize(QSize(18, 18));
  
  createButtonMenu(tb);
  
  _nav = new SymbolListWidget;
  connect(_nav, SIGNAL(signalSymbolSelected(BarData)), this, SLOT(chartOpened(BarData)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(_nav);

  // update to last symbol search before displaying
  QSettings settings;
  settings.beginGroup("main" + g_session);

  _searchExchange = settings.value("last_chart_panel_exchange_search", "*").toString();
  _searchString = settings.value("last_chart_panel_symbol_search", "*").toString();

  updateList();
}

void ChartPage::createActions ()
{
  QAction *action  = new QAction(QIcon(asterisk_xpm), tr("Show &All Symbols"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Show All Symbols"));
  connect(action, SIGNAL(activated()), this, SLOT(allButtonPressed()));
  _actions.insert(ShowAll, action);

  action  = new QAction(QIcon(search_xpm), tr("Symbol &Search") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  action->setToolTip(tr("Symbol Search") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(symbolSearch()));
  _actions.insert(Search, action);

  action  = new QAction(QIcon(add_xpm), tr("Add To &Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_G));
  action->setToolTip(tr("Add symbol to group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  _actions.insert(AddGroup, action);
}

void ChartPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(_actions.value(ShowAll));
  tb->addAction(_actions.value(Search));
  tb->addAction(_actions.value(AddGroup));

  _menu = new QMenu(this);
  _menu->addAction(_actions.value(AddGroup));
  _menu->addSeparator();
  _menu->addAction(_actions.value(ShowAll));
  _menu->addAction(_actions.value(Search));
  _menu->addSeparator();
}

void ChartPage::chartOpened (BarData bd)
{
  emit fileSelected(bd);
  emit addRecentChart(bd);
}

void ChartPage::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void ChartPage::addToGroup ()
{
  Command command("GROUP_SELECT_DIALOG");

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartPage::addToGroup: no plugin" << command.plugin();
    return;
  }

  connect(plug, SIGNAL(signalDone(QString)), this, SLOT(addToGroup2(QString)));

  plug->command(command);
}

void ChartPage::addToGroup2 (QString d)
{
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (! l.count())
    return;

  QStringList cl;
  cl << "GROUP_ADD_ITEMS" << d;

  int loop = 0;
  for (; loop < l.count(); loop++)
    cl << l.at(loop)->text();

  Command command(cl.join(","));

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartPage::addToGroup2: no plugin" << command.plugin();
    return;
  }

  plug->command(command);
}

void ChartPage::doKeyPress (QKeyEvent *key)
{
  key->accept();

  if (key->modifiers() == Qt::ControlModifier)
  {
    switch(key->key())
    {
      case Qt::Key_G:
        addToGroup();
	break;
      default:
        break;
    }
  }
  else
  {
    switch(key->key())
    {
      default:
        break;
    }
  }
}

void ChartPage::updateList ()
{
  _nav->clearSymbols();

  _nav->setSortingEnabled(FALSE);

  QStringList cl;
  cl << "QUOTE_DATABASE" << "SEARCH" << _searchExchange << _searchString;

  Command command(cl.join(","));

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartPage::updateList: no plugin" << command.plugin();
    return;
  }

  plug->command(command);

  cl = command.stringData().split(";", QString::SkipEmptyParts);
  
  int loop = 0;
  for (; loop < cl.count(); loop++)
  {
    QStringList l = cl.at(loop).split(",");
    if (l.count() != 3)
      continue;
    
    BarData bd;
    bd.setExchange(l.at(0));
    bd.setSymbol(l.at(1));
    bd.setName(l.at(2));
    
    _nav->addSymbol(bd);
  }

  _nav->setSortingEnabled(TRUE);
  
  buttonStatus();
}

void ChartPage::symbolSearch ()
{
  Command command("SYMBOL_DIALOG");

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartPage::symbolSearch: no plugin" << command.plugin();
    return;
  }

  connect(plug, SIGNAL(signalDone(QString)), this, SLOT(symbolSearch2(QString)));

  plug->command(command);
}

void ChartPage::symbolSearch2 (QString d)
{
  QStringList l = d.split(",", QString::SkipEmptyParts);
  if (l.count() != 2)
    return;
  
  _searchExchange = l.at(0);
  _searchString = l.at(1);
  
  QSettings settings;
  settings.beginGroup("main" + g_session);
  settings.setValue("last_chart_panel_symbol_search", _searchString);
  settings.setValue("last_chart_panel_exchange_search", _searchExchange);
  settings.sync();
  
  updateList();
}

void ChartPage::allButtonPressed ()
{
  _searchExchange = "*";
  _searchString = "*";
  updateList();
}

void ChartPage::buttonStatus ()
{
  int status = _nav->count();
  _actions.value(AddGroup)->setEnabled(status); 
}
