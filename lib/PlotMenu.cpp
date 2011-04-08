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
#include "Globals.h"
#include "ConfirmDialog.h"
#include "IndicatorEditDialog.h"
#include "DataDataBase.h"
#include "Indicator.h"

#include "../pics/loggrid.xpm"
#include "../pics/date.xpm"
#include "../pics/delete.xpm"
#include "../pics/buyarrow.xpm"
#include "../pics/sellarrow.xpm"
#include "../pics/fib.xpm"
#include "../pics/horizontal.xpm"
#include "../pics/text.xpm"
#include "../pics/trend.xpm"
#include "../pics/vertical.xpm"
#include "../pics/edit.xpm"
#include "../pics/lock.xpm"
#include "../pics/indicator.xpm"
#include "../pics/add.xpm"

#include <QtDebug>
#include <QSettings>
#include <QInputDialog>

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
  a->setData(QVariant("BUY"));
  _actions.insert(_BUY_CHART_OBJECT, a);

  // hline
  a = new QAction(QIcon(horizontal_xpm), tr("&HLine"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));
  a->setToolTip(tr("Create Horizontal Line Chart Object"));
  a->setStatusTip(QString(tr("Create Horizontal Line Chart Object")));
  a->setData(QVariant("HLINE"));
  _actions.insert(_HLINE_CHART_OBJECT, a);

  // retracement
  a = new QAction(QIcon(fib_xpm), tr("&Retracement"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  a->setToolTip(tr("Create Retracement Levels Chart Object"));
  a->setStatusTip(QString(tr("Create Retracement Levels Chart Object")));
  a->setData(QVariant("RETRACEMENT"));
  _actions.insert(_RETRACEMENT_CHART_OBJECT, a);

  // sell
  a = new QAction(QIcon(sellarrow_xpm), tr("&Sell"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  a->setToolTip(tr("Create Sell Arrow Chart Object"));
  a->setStatusTip(QString(tr("Create Sell Arrow Chart Object")));
  a->setData(QVariant("SELL"));
  _actions.insert(_SELL_CHART_OBJECT, a);

  // text
  a = new QAction(QIcon(text_xpm), tr("&Text"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_T));
  a->setToolTip(tr("Create Text Chart Object"));
  a->setStatusTip(QString(tr("Create Text Chart Object")));
  a->setData(QVariant("TEXT"));
  _actions.insert(_TEXT_CHART_OBJECT, a);

  // tline
  a = new QAction(QIcon(trend_xpm), tr("T&Line"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_L));
  a->setToolTip(tr("Create Trend Line Chart Object"));
  a->setStatusTip(QString(tr("Create Trend Line Chart Object")));
  a->setData(QVariant("TLINE"));
  _actions.insert(_TLINE_CHART_OBJECT, a);

  // vline
  a = new QAction(QIcon(vertical_xpm), tr("&VLine"), this);
//  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_V));
  a->setToolTip(tr("Create Vertical Line Chart Object"));
  a->setStatusTip(QString(tr("Create Vertical Line Chart Object")));
  a->setData(QVariant("VLINE"));
  _actions.insert(_VLINE_CHART_OBJECT, a);

  // new indicator
  a = new QAction(QIcon(indicator_xpm), tr("&New Indicator") + "...", this);
  a->setShortcut(QKeySequence(QKeySequence::New));
  a->setToolTip(tr("Add New Indicator") + "...");
  a->setStatusTip(tr("Add New Indicator") + "...");
  _actions.insert(_NEW_INDICATOR, a);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(newIndicator()));

  // edit indicator
  a = new QAction(QIcon(edit_xpm), tr("&Edit Indicator") + "...", this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
  a->setToolTip(tr("Edit Indicator") + "...");
  a->setStatusTip(tr("Edit Indicator") + "...");
  _actions.insert(_EDIT_INDICATOR, a);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(editIndicator()));

  // delete indicator
  a = new QAction(QIcon(delete_xpm), tr("&Delete Indicator(s)") + "...", this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
  a->setToolTip(tr("Permanantly Delete Indicator") + "...");
  a->setStatusTip(tr("Permanantly Delete Indicator") + "...");
  _actions.insert(_DELETE_INDICATOR, a);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(deleteIndicator()));

  // delete all chart objects
  a = new QAction(QIcon(delete_xpm), tr("Delete All &Chart Objects"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
  a->setToolTip(tr("Delete All Chart Objects"));
  a->setStatusTip(tr("Delete All Chart Objects"));
  _actions.insert(_DELETE_ALL_CHART_OBJECTS, a);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(deleteAllChartObjects()));

  // date
  a = new QAction(QIcon(date_xpm), tr("Date A&xis"), this);
  a->setToolTip(tr("Toggle Date Axis"));
  a->setStatusTip(tr("Toggle Date Axis"));
  a->setCheckable(TRUE);
  _actions.insert(_DATE_AXIS, a);
  connect(a, SIGNAL(triggered(bool)), this, SIGNAL(signalDateStatus(bool)));

  // log
  a = new QAction(QIcon(loggrid_xpm), tr("Log &Scaling"), this);
  a->setToolTip(tr("Toggle Log Scaling"));
  a->setStatusTip(tr("Toggle Log Scaling"));
  a->setCheckable(TRUE);
  _actions.insert(_LOG_SCALING, a);
  connect(a, SIGNAL(triggered(bool)), this, SIGNAL(signalLogStatus(bool)));

  // lock
  a = new QAction(QIcon(lock_xpm), tr("Loc&ked"), this);
  a->setToolTip(tr("Toggle Indicator Drag and Drop Lock"));
  a->setStatusTip(tr("Toggle Indicator Drag and Drop Lock"));
  a->setCheckable(TRUE);
  a->setChecked(FALSE);
  _actions.insert(_LOCK, a);
  connect(a, SIGNAL(triggered(bool)), this, SIGNAL(signalLockStatus(bool)));

  // add indicator
  a = new QAction(QIcon(add_xpm), tr("&Add Indicator") + "...", this);
  a->setToolTip(tr("Add Indicator") + "...");
  a->setStatusTip(tr("Add Indicator") + "...");
  _actions.insert(_ADD_INDICATOR, a);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(addIndicator()));

  // remove indicator
  a = new QAction(QIcon(delete_xpm), tr("&Remove Indicator") + "...", this);
  a->setToolTip(tr("Remove Indicator") + "...");
  a->setStatusTip(tr("Remove Indicator") + "...");
  _actions.insert(_REMOVE_INDICATOR, a);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(removeIndicator()));
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
  addAction(_actions.value(_NEW_INDICATOR));
  addAction(_actions.value(_ADD_INDICATOR));
  addAction(_actions.value(_EDIT_INDICATOR));
  addAction(_actions.value(_REMOVE_INDICATOR));
  addAction(_actions.value(_DELETE_INDICATOR));
  addSeparator ();
  addMenu(_coListMenu);
  addAction(_actions.value(_DELETE_ALL_CHART_OBJECTS));
  addSeparator ();
  addAction(_actions.value(_DATE_AXIS));
  addAction(_actions.value(_LOG_SCALING));
  addSeparator ();
  addAction(_actions.value(_LOCK));
}

void PlotMenu::editIndicator ()
{
  emit signalEditIndicator();
}

void PlotMenu::deleteIndicator ()
{
  QStringList l;
  l << _indicator;
  emit signalDeleteIndicator(1, l);
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

void PlotMenu::setLog (bool status)
{
  _actions.value(_LOG_SCALING)->setChecked(status);
}

bool PlotMenu::log ()
{
  return _actions.value(_LOG_SCALING)->isChecked();
}

void PlotMenu::setDate (bool status)
{
  _actions.value(_DATE_AXIS)->setChecked(status);
}

bool PlotMenu::date ()
{
  return _actions.value(_DATE_AXIS)->isChecked();
}

void PlotMenu::setLock (bool status)
{
  _actions.value(_LOCK)->setChecked(status);
}

bool PlotMenu::lock ()
{
  return _actions.value(_LOCK)->isChecked();
}

void PlotMenu::setIndicator (QString d)
{
  _indicator = d;
}

void PlotMenu::newIndicator ()
{
  IndicatorEditDialog *dialog = new IndicatorEditDialog(this, 0);
  dialog->show();
}

void PlotMenu::addIndicator ()
{
  QSettings set(g_localSettings);
  QStringList l = set.value("indicators").toStringList();

  QStringList l2;
  DataDataBase db("indicators");
  db.names(l2);

  int loop = 0;
  for (; loop < l.count(); loop++)
    l2.removeAll(l.at(loop));

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l2);
  dialog->setLabelText(tr("Select indicator"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(addIndicator2(QString)));
  dialog->show();
}

void PlotMenu::addIndicator2 (QString d)
{
  Indicator i;
  i.add(d);
}

void PlotMenu::removeIndicator ()
{
  QStringList l;
  l << _indicator;
  emit signalDeleteIndicator(0, l);
}
