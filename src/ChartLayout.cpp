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

#include "ChartLayout.h"
#include "Config.h"
#include "IndicatorDataBase.h"
#include "IndicatorDialog.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPluginDialog.h"
#include "IndicatorThread.h"
#include "ChartObjectDataBase.h"
#include "Plot.h"
#include "IndicatorDeleteDialog.h"

#include <QDebug>
#include <QCursor>
#include <QLabel>
#include <QTabBar>
#include <qwt_plot.h>

ChartLayout::ChartLayout ()
{
  _startIndex = -1;
  
  setOpaqueResize(FALSE); // only draw widget after resize is complete to speed things up

  int loop = 1;
  for (; loop < 4; loop++)
  {
    QWidget *w = new QWidget;
    _holders.insert(loop, w);

    QGridLayout *grid = new QGridLayout;
    grid->setMargin(0);
    grid->setSpacing(0);
    _grids.insert(loop, grid);

    w->setLayout(grid);
    addWidget(w);
    w->hide();
  }
}

void ChartLayout::save ()
{
  Config config;
  config.setData(Config::ChartLayoutSizes, (QSplitter *) this);

  QHashIterator<QString, TabWidget *> it(_tabs);
  while (it.hasNext())
  {
    it.next();
    Plot *p = (Plot *) it.value()->currentWidget();
    if (p)
    {
      QString k = "ChartLayout" + it.key();
      QString d = QString::number(it.value()->currentIndex());
      config.setData(k, d);
    }
  }
}

void ChartLayout::load ()
{
  // we need to load our splitter sizes
  Config config;
  config.getData(Config::ChartLayoutSizes, (QSplitter *) this);

  // setup the initial indicators
  IndicatorDataBase idb;
  QStringList l;
  idb.getActiveIndicatorList(l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    Indicator i;
    i.setName(l[loop]);
    idb.getIndicator(i);
    addTab(i);
  }

  QHashIterator<QString, TabWidget *> it(_tabs);
  while (it.hasNext())
  {
    it.next();
    Plot *p = (Plot *) it.value()->currentWidget();
    if (p)
    {
      QString k = "ChartLayout" + it.key();
      QString d;
      config.getData(k, d);
      it.value()->setCurrentIndex(d.toInt());
    }
  }

  // set plot settings from config
  QColor color;
  config.getData(Config::BackgroundColor, color);
  emit signalBackgroundColor(color);
  
  QFont font;
  config.getData(Config::PlotFont, font);
  emit signalFont(font);

  int tint = config.getInt(Config::CrossHairs);
  emit signalCrossHairs(tint);

  config.getData(Config::CrossHairsColor, color);
  emit signalCrossHairsColor(color);

  emit signalDraw();
}

void ChartLayout::addTab (QString &name)
{
  IndicatorDataBase idb;
  Indicator i;
  i.setName(name);
  idb.getIndicator(i);
  addTab(i);
}

void ChartLayout::addTab (Indicator &i)
{
  if (! i.enable())
    return;

  QGridLayout *grid = _grids.value(i.tabRow());
  if (! grid)
    return;
  
  QWidget *w = _holders.value(i.tabRow());
  if (! w)
    return;

  QString key = QString::number(i.tabRow()) + ":" + QString::number(i.column());
  TabWidget *tab = _tabs.value(key);
  if (! tab)
  {
    tab = new TabWidget(key);
//    connect(tab, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged()));
    _tabs.insert(key, tab);
    grid->addWidget(tab, 0, i.column() - 1);
    w->show();
  }

  PlotSettings settings;
  settings.plot = new Plot;
  settings.name = i.name();
  settings.plot->setIndicator(i.name());
  int pos = tab->addTab(settings.plot, QString());

  // create a label that paints horizontally
  QTabBar *tb = tab->getTabBar();
  QLabel *label = new QLabel;
  label->setIndent(0);
  label->setMargin(0);
  label->setAlignment(Qt::AlignLeft);
  label->setText(i.name());
  tb->setTabButton(pos, QTabBar::LeftSide, label);

  settings.row = i.tabRow();
  settings.col = i.column();
  _plots.insert(i.name(), settings);

  settings.plot->showDate(i.date());
//  settings.plot->setLogScaling(i.getLog());

  connect(this, SIGNAL(signalBackgroundColor(QColor)), settings.plot, SLOT(setBackgroundColor(QColor)));
  connect(this, SIGNAL(signalFont(QFont)), settings.plot, SLOT(setFont(QFont)));
  connect(this, SIGNAL(signalGridColor(QColor)), settings.plot, SLOT(setGridColor(QColor)));
//  connect(plot, SIGNAL(signalPixelSpaceChanged(int, int)), this, SIGNAL(signalZoom(int, int)));
  connect(settings.plot, SIGNAL(signalInfoMessage(Setting)), this, SIGNAL(signalInfo(Setting)));
  connect(settings.plot, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatus(QString)));
//  connect(this, SIGNAL(signalPixelSpace(int)), settings.plot, SLOT(setSpacing(int)));
  connect(this, SIGNAL(signalClear()), settings.plot, SLOT(clear()));
  connect(this, SIGNAL(signalGrid(bool)), settings.plot, SLOT(setGrid(bool)));
  connect(this, SIGNAL(signalDraw()), settings.plot, SLOT(replot()));
  connect(this, SIGNAL(signalIndex(int)), settings.plot, SLOT(setStartIndex(int)));
  connect(settings.plot, SIGNAL(signalNewIndicator()), this, SLOT(newIndicator()));
  connect(settings.plot, SIGNAL(signalEditIndicator(QString)), this, SLOT(editIndicator(QString)));
  connect(settings.plot, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(deleteIndicator()));
  connect(settings.plot, SIGNAL(signalBackgroundColorChanged(QColor)), this, SLOT(backgroundColorChanged(QColor)));
  connect(settings.plot, SIGNAL(signalFontChanged(QFont)), this, SLOT(fontChanged(QFont)));
  connect(this, SIGNAL(signalCrossHairs(bool)), settings.plot, SLOT(setCrossHairs(bool)));
  connect(this, SIGNAL(signalCrossHairsColor(QColor)), settings.plot, SLOT(setCrossHairsColor(QColor)));
}

//******************************************************************
//************* DRAW FUNCTIONS ************************************
//*****************************************************************

void ChartLayout::loadPlots (BarData &bd, int index)
{
  if (! bd.count())
    return;

  _barData = bd;
  _startIndex = index;

  IndicatorDataBase db;
  QStringList indicatorList;
  db.getActiveIndicatorList(indicatorList);

  int loop = 0;
  for (; loop < indicatorList.count(); loop++)
  {
    if (! _plots.contains(indicatorList[loop]))
      continue;
    
    PlotSettings ps = _plots.value(indicatorList[loop]);

    ps.plot->setDates(_barData);
    
    Indicator i;
    i.setName(indicatorList[loop]);
    db.getIndicator(i);

    qRegisterMetaType<Indicator>("Indicator");
    IndicatorThread *r = new IndicatorThread(this, _barData, i);
    connect(r, SIGNAL(signalDone(Indicator)), this, SLOT(indicatorThreadFinished(Indicator)), Qt::QueuedConnection);
    connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
    r->start();
  }
}

void ChartLayout::indicatorThreadFinished (Indicator i)
{
  // add plots to the plot
  if (! _plots.contains(i.name()))
    return;
  
  PlotSettings settings = _plots.value(i.name());
  
  settings.plot->clear();

  settings.plot->addCurves(i.curves());

  settings.plot->loadChartObjects();

  settings.plot->setStartIndex(_startIndex);
}

void ChartLayout::setGridColor (QColor d)
{
  emit signalGridColor(d);
  emit signalDraw();
}

void ChartLayout::setCrossHairsColor (QColor d)
{
  emit signalCrossHairsColor(d);
  emit signalDraw();
}

void ChartLayout::setPixelSpace (int d)
{
  emit signalPixelSpace(d);
  emit signalDraw();
}

void ChartLayout::setIndex (int d)
{
  emit signalIndex(d);
//  emit signalDraw();
}

void ChartLayout::clearIndicator ()
{
  _barData.clear();
  
  emit signalClear();
  emit signalDraw();
}

void ChartLayout::saveSettings ()
{
  emit signalSaveSettings();
}

void ChartLayout::setGrid (bool d)
{
  emit signalGrid(d);
  emit signalDraw();
}

void ChartLayout::setCrossHairs (bool d)
{
  emit signalCrossHairs(d);
  emit signalDraw();
}

void ChartLayout::setZoom (int pixelSpace, int index)
{
  emit signalPixelSpace(pixelSpace);
  emit signalIndex(index);
  emit signalDraw();
}

void ChartLayout::backgroundColorChanged (QColor c)
{
  Config config;
  config.setData(Config::BackgroundColor, c);
  emit signalBackgroundColor(c);
  emit signalDraw();
}

void ChartLayout::fontChanged (QFont f)
{
  Config config;
  config.setData(Config::PlotFont, f);
  emit signalFont(f);
  emit signalDraw();
}

int ChartLayout::plotWidth ()
{
  // find the best plot width for either < 1 column or > 1 column
  int maxWidth = 0;
  int minWidth = 999999;
  int cols = 1;
  QHashIterator<QString, PlotSettings> it(_plots);
  while (it.hasNext())
  {
     it.next();
     PlotSettings settings = it.value();

     if (settings.col > cols)
       cols = settings.col;

     int width = settings.plot->width();
     
     if (width > maxWidth)
       maxWidth = width;

     if (width < minWidth)
       minWidth = width;
  }

  if (cols > 1)
    return minWidth;
  else
    return maxWidth;
}

void ChartLayout::newIndicator ()
{
  IndicatorDialog *dialog = new IndicatorDialog;
  connect(dialog, SIGNAL(signalDone(Indicator)), this, SLOT(newIndicator2(Indicator)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ChartLayout::newIndicator2 (Indicator i)
{
  IndicatorPluginFactory fac;
  IndicatorPlugin *ip = fac.plugin(i.indicator());
  if (! ip)
  {
    qDebug() << "ChartLayout::newIndicator2: plugin error" << i.indicator();
    return;
  }

  ip->defaults(i);
  
  IndicatorPluginDialog *dialog = ip->dialog(i);
  if (! dialog)
  {
    qDebug() << "ChartLayout::newIndicator2: no dialog";
    return;
  }

  connect(dialog, SIGNAL(signalDone(Indicator)), this, SLOT(newIndicator3(Indicator)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ChartLayout::newIndicator3 (Indicator i)
{
  addTab(i.name());
  emit signalDraw();
}

void ChartLayout::editIndicator (QString name)
{
  Indicator i;
  i.setName(name);
  IndicatorDataBase db;
  db.getIndicator(i);
  
  IndicatorPluginFactory fac;
  IndicatorPlugin *ip = fac.plugin(i.indicator());
  if (! ip)
  {
    qDebug() << "ChartLayout::editIndicator: plugin error" << i.indicator();
    return;
  }

  IndicatorPluginDialog *dialog = ip->dialog(i);
  if (! dialog)
  {
    qDebug() << "ChartLayout::editIndicator: no dialog";
    return;
  }

  connect(dialog, SIGNAL(signalDone(Indicator)), this, SLOT(editIndicator2(Indicator)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ChartLayout::editIndicator2 (Indicator i)
{
  if (! _barData.count())
    return;

  qRegisterMetaType<Indicator>("Indicator");
  IndicatorThread *r = new IndicatorThread(this, _barData, i);
  connect(r, SIGNAL(signalDone(Indicator)), this, SLOT(indicatorThreadFinished(Indicator)), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}

void ChartLayout::deleteIndicator ()
{
  IndicatorDeleteDialog *dialog = new IndicatorDeleteDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalDelete(Indicator)), this, SLOT(removeTab(Indicator)));
  dialog->show();
}

void ChartLayout::removeTab (Indicator i)
{
  if (! _plots.contains(i.name()))
    return;

  PlotSettings settings = _plots.value(i.name());

  int row = settings.row;
  int col = settings.col;

  QString key = QString::number(row) + ":" + QString::number(col);
  TabWidget *tab = _tabs.value(key);
  if (! tab)
    return;

  tab->removeTab(tab->indexOf(settings.plot));

  _plots.remove(i.name());

  if (! tab->count())
  {
    delete tab;
    _tabs.remove(key);

    QGridLayout *grid = _grids.value(row);
    if (grid)
    {
      if (! grid->count())
      {
        QWidget *w = _holders.value(row);
        if (w)
          w->hide();
      }
    }
  }
}

QHash<QString, PlotSettings> & ChartLayout::plots ()
{
  return _plots;
}

