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

#include "PlotMenu.h"
#include "ConfirmDialog.h"
#include "IndicatorDialog.h"

#include "../pics/buyarrow.xpm"
#include "../pics/sellarrow.xpm"
#include "../pics/fib.xpm"
#include "../pics/horizontal.xpm"
#include "../pics/text.xpm"
#include "../pics/trend.xpm"
#include "../pics/vertical.xpm"
#include "../pics/delete.xpm"
#include "../pics/indicator.xpm"

#include <QtDebug>

PlotMenu::PlotMenu (QWidget *p) : QMenu (p)
{
  createActions();
  createMenus();
}

void PlotMenu::createActions ()
{
  // buy
  QAction *a = new QAction(QIcon(buyarrow_xpm), tr("&Buy"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_B));
  a->setToolTip(tr("Create Buy Arrow Chart Object"));
  a->setStatusTip(QString(tr("Create Buy Arrow Chart Object")));
  a->setData(QVariant("Buy"));
  _actions.insert(_BUY_CHART_OBJECT, a);

  // hline
  a = new QAction(QIcon(horizontal_xpm), tr("&HLine"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));
  a->setToolTip(tr("Create Horizontal Line Chart Object"));
  a->setStatusTip(QString(tr("Create Horizontal Line Chart Object")));
  a->setData(QVariant("HLine"));
  _actions.insert(_HLINE_CHART_OBJECT, a);

  // retracement
  a = new QAction(QIcon(fib_xpm), tr("&Retracement"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  a->setToolTip(tr("Create Retracement Levels Chart Object"));
  a->setStatusTip(QString(tr("Create Retracement Levels Chart Object")));
  a->setData(QVariant("Retracement"));
  _actions.insert(_RETRACEMENT_CHART_OBJECT, a);

  // sell
  a = new QAction(QIcon(sellarrow_xpm), tr("&Sell"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  a->setToolTip(tr("Create Sell Arrow Chart Object"));
  a->setStatusTip(QString(tr("Create Sell Arrow Chart Object")));
  a->setData(QVariant("Sell"));
  _actions.insert(_SELL_CHART_OBJECT, a);

  // text
  a = new QAction(QIcon(text_xpm), tr("&Text"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_T));
  a->setToolTip(tr("Create Text Chart Object"));
  a->setStatusTip(QString(tr("Create Text Chart Object")));
  a->setData(QVariant("Text"));
  _actions.insert(_TEXT_CHART_OBJECT, a);

  // tline
  a = new QAction(QIcon(trend_xpm), tr("T&Line"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_L));
  a->setToolTip(tr("Create Trend Line Chart Object"));
  a->setStatusTip(QString(tr("Create Trend Line Chart Object")));
  a->setData(QVariant("TLine"));
  _actions.insert(_TLINE_CHART_OBJECT, a);

  // vline
  a = new QAction(QIcon(vertical_xpm), tr("&VLine"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_V));
  a->setToolTip(tr("Create Vertical Line Chart Object"));
  a->setStatusTip(QString(tr("Create Vertical Line Chart Object")));
  a->setData(QVariant("VLine"));
  _actions.insert(_VLINE_CHART_OBJECT, a);

  // delete all chart objects
  a = new QAction(QIcon(delete_xpm), tr("Delete All &Chart Objects"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
  a->setToolTip(tr("Delete All Chart Objects"));
  a->setStatusTip(tr("Delete All Chart Objects"));
  _actions.insert(_DELETE_ALL_CHART_OBJECTS, a);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(deleteAllChartObjects()));

  // edit indicator
  a = new QAction(QIcon(indicator_xpm), tr("&Edit Indicators") + "...", this);
  a->setToolTip(tr("Edit Indicators") + "...");
  a->setStatusTip(tr("Edit Indicators") + "...");
  _actions.insert(_EDIT_INDICATOR, a);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(editIndicator()));
}

void PlotMenu::createMenus ()
{
  // create the chart object menu
  _coListMenu = new QMenu(this);
  _coListMenu->setTitle(tr("New Chart Object..."));
  _coListMenu->addAction(_actions.value(_BUY_CHART_OBJECT));
  _coListMenu->addAction(_actions.value(_HLINE_CHART_OBJECT));
  _coListMenu->addAction(_actions.value(_RETRACEMENT_CHART_OBJECT));
  _coListMenu->addAction(_actions.value(_SELL_CHART_OBJECT));
  _coListMenu->addAction(_actions.value(_TEXT_CHART_OBJECT));
  _coListMenu->addAction(_actions.value(_TLINE_CHART_OBJECT));
  _coListMenu->addAction(_actions.value(_VLINE_CHART_OBJECT));
  connect(_coListMenu, SIGNAL(triggered(QAction *)), this, SLOT(chartObjectMenuSelected(QAction *)));

  // create the main menu
  addAction(_actions.value(_EDIT_INDICATOR));
  addSeparator ();
  addMenu(_coListMenu);
  addAction(_actions.value(_DELETE_ALL_CHART_OBJECTS));
}

void PlotMenu::deleteAllChartObjects ()
{
  ConfirmDialog *dialog = new ConfirmDialog(this);
  dialog->setMessage(tr("Confirm all chart objects delete"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteAllChartObjects2()));
  dialog->show();
}

void PlotMenu::deleteAllChartObjects2 ()
{
  emit signalDeleteAllChartObjects();
}

void PlotMenu::chartObjectMenuSelected (QAction *a)
{
  emit signalNewChartObject(a->data().toString());
}

void PlotMenu::setCOMenuStatus (bool status)
{
  _coListMenu->setEnabled(status);
}

void PlotMenu::editIndicator ()
{
  IndicatorDialog *dialog = new IndicatorDialog(this);
  dialog->show();
}
