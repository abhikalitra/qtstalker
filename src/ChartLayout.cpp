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
#include "Globals.h"
#include "Command.h"
#include "Script.h"
#include "qtstalker_defines.h"
#include "IndicatorDataBase.h"

#include <QDebug>
#include <QCursor>
#include <QLabel>
#include <QTabBar>
#include <qwt_plot.h>
#include <QSettings>
#include <QMutexLocker>
#include <QTimer>

ChartLayout::ChartLayout ()
{
  _startIndex = -1;
  _barSpacing = 8;
  
  setOpaqueResize(FALSE); // only draw widget after resize is complete to speed things up

  int loop = 1;
  for (; loop < 4; loop++)
  {
    QWidget *w = new QWidget;
    _holders.insert(loop, w);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    w->setLayout(hbox);
    _grids.insert(loop, hbox);

    addWidget(w);
    w->hide();
    
    QString key = QString::number(loop);
    TabWidget *tab = new TabWidget(key);
    _tabs.insert(key, tab);
    hbox->addWidget(tab);
    tab->hide();
  }
}

void ChartLayout::save ()
{
  QSettings settings(g_settingsFile);

  QStringList l;
  QList<int> sizeList = sizes();
  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    l.append(QString::number(sizeList[loop]));
  settings.setValue("chart_layout_sizes", l);

  QHashIterator<QString, TabWidget *> it(_tabs);
  while (it.hasNext())
  {
    it.next();
    Plot *p = (Plot *) it.value()->currentWidget();
    if (p)
      settings.setValue("chart_layout_" + it.key(), it.value()->currentIndex());
  }

  settings.sync();

  // save any chart object changes
  emit signalSaveSettings();
}

void ChartLayout::load ()
{
  // we need to load our splitter sizes
  QSettings settings(g_settingsFile);

  QStringList l = settings.value("chart_layout_sizes").toStringList();
  if (! l.count())
  {
    QList<int> l2;
    l2 << 437 << 20 << 200;
    setSizes(l2);
  }
  else
  {
    QList<int> sizeList = sizes();

    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      if (loop >= sizeList.count())
        break;

      if (l[loop].toInt() < 25)
        sizeList[loop] = 25;
      else
        sizeList[loop] = l[loop].toInt();
    }

    setSizes(sizeList);
  }

  // setup the initial indicators
  IndicatorDataBase db;
  l.clear();
  if (db.indicators(l))
  {
    qDebug() << "ChartLayout::load: IndicatorDataBase error";
    return;
  }
  
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    Indicator *i = new Indicator;
    i->setName(l.at(loop));

    if (db.load(i))
    {
      qDebug() << "ChartLayout::load: IndicatorDataBase error";
      return;
    }

    _indicators.insert(i->name(), i);
    addTab(i);
  }

  QHashIterator<QString, TabWidget *> it(_tabs);
  while (it.hasNext())
  {
    it.next();
    Plot *p = (Plot *) it.value()->currentWidget();
    if (p)
    {
      QString k = "chart_layout_" + it.key();
      it.value()->setCurrentIndex(settings.value(k).toInt());
    }
  }

  emit signalIndex(0);
}

void ChartLayout::addTab (QString name)
{
  IndicatorDataBase db;
  Indicator *i = new Indicator;
  i->setName(name);
  if (db.load(i))
  {
    qDebug() << "ChartLayout::addTab: IndicatorDataBase error";
    return;
  }

  _indicators.insert(name, i);
  
  addTab(i);
}

void ChartLayout::addTab (Indicator *i)
{
  QWidget *w = _holders.value(i->tabRow());
  if (! w)
    return;
  
  w->show();

  QString key = QString::number(i->tabRow());
  TabWidget *tab = _tabs.value(key);
  tab->show();

  Plot *plot = new Plot;
  plot->setIndicator(i->name());
  plot->showDate(i->date());
  plot->setLogScaling(i->getLog());
  plot->setBarSpacing(_barSpacing);
  
  int pos = tab->addTab(plot, QString());
  tab->setTabButton(pos, i->name());

  // set plot settings from config
  QSettings set(g_settingsFile);
  QString s = set.value("plot_background_color").toString();
  QColor color(s);
  plot->setBackgroundColor(color);

  QStringList l = set.value("plot_font").toString().split(";", QString::SkipEmptyParts);
  if (l.count())
  {
    QFont font;
    font.fromString(l.join(","));
    plot->setFont(font);
  }

  // set crosshairs status
  plot->setCrossHairs(set.value("crosshairs", 0).toInt());

  // set crosshairs color
  color.setNamedColor(set.value("crosshairs_color", "white").toString());
  plot->setCrossHairsColor(color);

  connect(plot, SIGNAL(signalInfoMessage(Setting)), this, SIGNAL(signalInfo(Setting)));
  connect(plot, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(this, SIGNAL(signalBarSpacing(int)), plot, SLOT(setBarSpacing(int)));
  connect(this, SIGNAL(signalClear()), plot, SLOT(clear()));
  connect(this, SIGNAL(signalDraw()), plot, SLOT(replot()));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setStartIndex(int)));
  connect(this, SIGNAL(signalSaveSettings()), plot, SLOT(clear()));

  connect(g_middleMan, SIGNAL(signalPlotBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  connect(g_middleMan, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setFont(QFont)));
  connect(g_middleMan, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  connect(g_middleMan, SIGNAL(signalGrid(bool)), plot, SLOT(setGrid(bool)));
  connect(g_middleMan, SIGNAL(signalCrosshairsColor(QColor)), plot, SLOT(setCrossHairsColor(QColor)));
  connect(g_middleMan, SIGNAL(signalCrosshairs(bool)), plot, SLOT(setCrossHairs(bool)));

  _plots.insert(i->name(), plot);
}

void ChartLayout::addNewTab (QString name)
{
  addTab(name);
  loadPlot(name);
}

//******************************************************************
//************* DRAW FUNCTIONS ************************************
//*****************************************************************

void ChartLayout::loadPlots (int index)
{
  if (! g_barData->count())
    return;

  _startIndex = index;

  QStringList l;
  QHashIterator<QString, Indicator *> it(_indicators);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }

  int loop = 0;
  for (; loop < l.count(); loop++)
    loadPlot(l.at(loop));
}

void ChartLayout::loadPlot (QString name)
{
  if (! _plots.contains(name))
    return;

  Plot *plot = _plots.value(name);

  plot->setDates();

  Indicator *i = _indicators.value(name);
  if (! i)
    return;

  Script *script = new Script;
  script->setName(i->name());
  script->setFile(i->script());
  script->setCommand(i->command());
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(indicatorScriptFinished(QString)));
  script->setBarData(g_barData);
  script->setIndicator(i);
  script->startScript();
}

void ChartLayout::indicatorScriptFinished (QString name)
{
  Indicator *i = _indicators.value(name);
  if (! i)
  {
    qDebug() << "ChartLayout::indicatorScriptFinished: no indicator for" << name;
    return;
  }

  // add plots to the plot
  if (! _plots.contains(name))
  {
    qDebug() << "ChartLayout::indicatorScriptFinished: no plot for" << name;
    return;
  }
  
  Plot *plot = _plots.value(name);

  plot->clear();

  i->weedPlots();
  plot->addCurves(i->curves());
  i->clearLines();
  
  plot->loadChartObjects();
  plot->setStartIndex(_startIndex);
}

void ChartLayout::setBarSpacing (int d)
{
  _barSpacing = d;
  emit signalBarSpacing(d);
  emit signalDraw();
}

void ChartLayout::setIndex (int d)
{
  emit signalIndex(d);
}

void ChartLayout::clearIndicator ()
{
  emit signalClear();
//  emit signalDraw();
}

void ChartLayout::saveSettings ()
{
  emit signalSaveSettings();
}

void ChartLayout::setZoom (int pixelSpace, int index)
{
  emit signalBarSpacing(pixelSpace);
  emit signalIndex(index);
  emit signalDraw();
}

int ChartLayout::plotWidth ()
{
  // find the best plot width for either < 1 column or > 1 column
  int maxWidth = 0;
  QHashIterator<QString, Plot *> it(_plots);
  while (it.hasNext())
  {
     it.next();
     int width = it.value()->width();
     if (width > maxWidth)
       maxWidth = width;
  }

  return maxWidth;
}

void ChartLayout::removeTab (QStringList l)
{
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    if (! _plots.contains(l.at(loop)))
      continue;

    Plot *plot = _plots.value(l.at(loop));

    if (! _indicators.contains(l.at(loop)))
      continue;

    Indicator *i = _indicators.value(l.at(loop));
    
    int row = i->tabRow();

    QString key = QString::number(row);
    TabWidget *tab = _tabs.value(key);
    if (! tab)
      continue;

    int ti = tab->indexOf(plot);
    if (ti == -1)
    {
      qDebug() << "ChartLayout::removeTab: tab not found";
      continue;
    }

    tab->removeTab(ti);

    delete plot;
    _plots.remove(l.at(loop));

    delete i;
    _indicators.remove(l.at(loop));

    if (! tab->count())
      tab->hide();

    int loop = 1;
    int flag = 0;
    for (; loop < 3; loop++)
    {
      key = QString::number(row);
      tab = _tabs.value(key);
      if (tab->count())
      {
        flag = 1;
        break;
      }
    }

    if (! flag)
    {
      QWidget *baseRow = _holders.value(row);
      baseRow->hide();
    }
  }
}
