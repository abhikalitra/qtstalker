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
#include "GroupDataBase.h"
#include "Config.h"
#include "SymbolDialog.h"
#include "UpdateChartPageThread.h"
#include "QuoteServerDialog.h"
#include "GroupAddDialog.h"

#include "../pics/addgroup.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"
//#include "../pics/delete.xpm"
#include "../pics/configure.xpm"

#include <QCursor>
#include <QToolTip>
#include <QLayout>
#include <QMenu>
#include <QKeyEvent>
#include <QtDebug>
#include <QList>

ChartPage::ChartPage ()
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
  
  _nav = new SymbolListWidget;
  connect(_nav, SIGNAL(signalSymbolSelected(BarData)), this, SLOT(chartOpened(BarData)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(_nav);

  // update to last symbol search before displaying
  Config config;
  config.getData(Config::LastChartPanelExchangeSearch, _searchExchange);
  config.getData(Config::LastChartPanelSymbolSearch, _searchString);

  updateList();
}

void ChartPage::createActions ()
{
  QAction *action  = new QAction(QIcon(asterisk_xpm), tr("Show &All Symbols"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Show All Symbols"));
  connect(action, SIGNAL(activated()), this, SLOT(allButtonPressed()));
  _actions.insert(ShowAll, action);

  action  = new QAction(QIcon(search_xpm), tr("Symbol &Search"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  action->setToolTip(tr("Symbol Search"));
  connect(action, SIGNAL(activated()), this, SLOT(symbolSearch()));
  _actions.insert(Search, action);

  action  = new QAction(QIcon(addgroup), tr("Add To &Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_G));
  action->setToolTip(tr("Add symbol to group"));
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  _actions.insert(AddGroup, action);

//  action  = new QAction(QIcon(delete_xpm), tr("&Delete Symbol"), this);
//  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
//  action->setToolTip(tr("Delete symbol from the database permanently"));
//  connect(action, SIGNAL(activated()), this, SLOT(deleteSymbol()));
//  _actions.insert(Delete, action);

  action  = new QAction(QIcon(configure_xpm), tr("&Configure Quote Server"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_C));
  action->setToolTip(tr("Configure Quote Server"));
  connect(action, SIGNAL(activated()), this, SLOT(serverDialog()));
  _actions.insert(Server, action);
}

void ChartPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(_actions.value(ShowAll));
  tb->addAction(_actions.value(Search));
  tb->addAction(_actions.value(AddGroup));
//  tb->addAction(_actions.value(Delete));
  tb->addAction(_actions.value(Server));

  _menu = new QMenu(this);
  _menu->addAction(_actions.value(AddGroup));
  _menu->addSeparator();
  _menu->addAction(_actions.value(ShowAll));
  _menu->addAction(_actions.value(Search));
  _menu->addSeparator();
//  _menu->addAction(_actions.value(Delete));
//  _menu->addSeparator();
  _menu->addAction(_actions.value(Server));
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
  GroupAddDialog *dialog = new GroupAddDialog(_nav->symbols());
  connect(dialog, SIGNAL(signalGroupChanged()), this, SIGNAL(signalAddToGroup()));
  dialog->show();
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

  setEnabled(FALSE);

  _nav->setSortingEnabled(FALSE);
  
  qRegisterMetaType<BarData>("BarData");
  UpdateChartPageThread *r = new UpdateChartPageThread(this, _searchExchange, _searchString);
  connect(r, SIGNAL(signalSymbol(BarData)), _nav, SLOT(addSymbol(BarData)), Qt::QueuedConnection);
  connect(r, SIGNAL(signalDone()), this, SLOT(requestDone()), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}

void ChartPage::requestDone ()
{
  _nav->setSortingEnabled(TRUE);
  setEnabled(TRUE);
  buttonStatus();
}

void ChartPage::symbolSearch ()
{
  SymbolDialog *dialog = new SymbolDialog(1);
  dialog->setSymbols(_searchExchange, _searchString);
  connect(dialog, SIGNAL(signalResults(QString, QString)), this, SLOT(symbolSearch2(QString, QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ChartPage::symbolSearch2 (QString ex, QString ss)
{
  _searchExchange = ex;
  _searchString = ss;
  
  Config config;
  config.setData(Config::LastChartPanelSymbolSearch, _searchString);
  config.setData(Config::LastChartPanelExchangeSearch, _searchExchange);
  
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

void ChartPage::deleteSymbol ()
{
/*  
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (! l.count())
    return;

  int rc = QMessageBox::warning(this,
                                tr("Qtstalker: Warning"),
                                tr("Are you sure you want to permanently delete symbol(s)?"),
                                QMessageBox::Yes,
                                QMessageBox::No,
                                QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QListWidgetItem *item = l.at(loop);
    int row = _nav->row(item);
    BarData bd;
    if (_symbols.getItem(row, bd))
    {
      qDebug() << "ChartPage::deleteSymbol: BarData not found" << row;
      continue;
    }

// FIXME:
    if (plug->deleteSymbol(&bd))
    {
      qDebug() << "ChartPage::deleteSymbol: plugin deleteSymbol error";
      continue;
    }
  }

  updateList();
*/  
}

void ChartPage::serverDialog ()
{
  QuoteServerDialog *dialog = new QuoteServerDialog(this);
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}
  
