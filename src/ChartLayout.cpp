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

#include <QInputDialog>
#include <QCursor>
#include <QMessageBox>
#include <QLabel>
#include <QTabBar>

ChartLayout::ChartLayout ()
{
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
    connect(tab, SIGNAL(currentChanged(QWidget *)), this, SLOT(drawTab(QWidget *)));
    _tabs.insert(key, tab);
    grid->addWidget(tab, 0, i.column() - 1);
    w->show();
  }
  
  Plot *plot = new Plot;
  _plotList.insert(i.name(), plot);
  int pos = tab->addTab(plot, QString());

  // create a label that paints horizontally
  QTabBar *tb = tab->getTabBar();
  QLabel *label = new QLabel;
  label->setIndent(0);
  label->setMargin(0);
  label->setAlignment(Qt::AlignLeft);
  label->setText(i.name());
  tb->setTabButton(pos, QTabBar::LeftSide, label);

  plot->setIndicator(i);
  plot->setRow(i.tabRow());
  plot->setColumn(i.column());

  connect(plot, SIGNAL(signalNewIndicator()), this, SLOT(newIndicator()));
  connect(plot, SIGNAL(signalEditIndicator(QString)), this, SLOT(editIndicator(QString)));
  connect(plot, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(deleteIndicator(QString)));
  connect(plot, SIGNAL(signalMoveIndicator(QString)), this, SLOT(moveIndicator(QString)));

  connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  connect(plot, SIGNAL(signalBackgroundColorChanged(QColor)), this, SIGNAL(signalBackgroundColor(QColor)));

  connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  connect(plot, SIGNAL(signalBorderColorChanged(QColor)), this, SIGNAL(signalBorderColor(QColor)));

  connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  connect(plot, SIGNAL(signalPlotFontChanged(QFont)), this, SIGNAL(signalPlotFont(QFont)));

  connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  connect(plot, SIGNAL(signalPixelSpaceChanged(int, int)), this, SIGNAL(signalZoom(int, int)));
  connect(plot, SIGNAL(signalInfoMessage(Setting *)), this, SIGNAL(signalInfo(Setting *)));
  connect(plot, SIGNAL(signalStatusMessage(QString)), this, SIGNAL(signalStatus(QString)));
  connect(this, SIGNAL(signalPixelSpace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(BarData::BarLength)), plot, SLOT(setInterval(BarData::BarLength)));
  connect(this, SIGNAL(signalClearIndicator()), plot, SLOT(clear()));
  connect(this, SIGNAL(signalSaveSettings()), plot, SLOT(clear()));
  connect(this, SIGNAL(signalGrid(bool)), plot, SLOT(gridChanged(bool)));
  connect(this, SIGNAL(signalNewExternalChartObject(QString)), plot, SLOT(newExternalChartObject(QString)));
  connect(plot, SIGNAL(signalNewExternalChartObjectDone()), this, SLOT(newExternalChartObjectDone()));
  connect(this, SIGNAL(signalSetExternalChartObject()), plot, SLOT(setExternalChartObjectFlag()));
  connect(this, SIGNAL(signalCursorChanged(int)), plot, SLOT(cursorChanged(int)));
  connect(plot, SIGNAL(signalIndexChanged(int)), this, SIGNAL(signalIndexChanged(int)));
  connect(plot, SIGNAL(signalDraw(QString)), this, SLOT(drawPlot(QString)));
}

//******************************************************************
//************* DRAW FUNCTIONS ************************************
//*****************************************************************

void ChartLayout::drawTab (QWidget *w)
{
  if (! w)
    return;

  Plot *p = (Plot *) w;
  p->draw();
}

void ChartLayout::drawPlot (QString d)
{
  Plot *p = _plotList.value(d);
  if (! p || ! p->isVisible())
    return;
  p->draw();
}

void ChartLayout::drawPlots ()
{
  QHashIterator<QString, TabWidget *> it(_tabs);
  while (it.hasNext())
  {
    it.next();
    Plot *p = (Plot *) it.value()->currentWidget();
    if (p)
      p->draw();
  }
}

void ChartLayout::refresh (QString name)
{
  Plot *p = _plotList.value(name);
  if (p)
    p->draw();
}

void ChartLayout::loadPlots (BarData &bd, int index)
{
  IndicatorDataBase db;
  QStringList indicatorList;
  db.getActiveIndicatorList(indicatorList);

  int loop = 0;
  for (; loop < indicatorList.count(); loop++)
  {
    Plot *plot = _plotList.value(indicatorList[loop]);
    if (! plot)
      continue;

    plot->loadIndicator(bd, index);
  }
}

void ChartLayout::setGridColor (QColor d)
{
  emit signalGridColor(d);
  drawPlots();
}

void ChartLayout::setPixelSpace (int d)
{
  emit signalPixelSpace(d);
  drawPlots();
}

void ChartLayout::setIndex (int d)
{
  emit signalIndex(d);
  drawPlots();
}

void ChartLayout::setInterval (int d)
{
  emit signalInterval((BarData::BarLength) d);
  emit signalRefresh ();
}

void ChartLayout::clearIndicator ()
{
  emit signalClearIndicator();
}

void ChartLayout::saveSettings ()
{
  emit signalSaveSettings();
}

void ChartLayout::setGrid (bool d)
{
  emit signalGrid(d);
  drawPlots();
}

void ChartLayout::newExternalChartObject (QString d)
{
  emit signalNewExternalChartObject(d);
}

void ChartLayout::setExternalChartObject ()
{
  emit signalSetExternalChartObject();
}

void ChartLayout::cursorChanged (int d)
{
  emit signalCursorChanged(d);
}

void ChartLayout::setZoom (int pixelSpace, int index)
{
  emit signalPixelSpace(pixelSpace);
  emit signalIndex(index);
//  emit signalIndexChanged(index)
  drawPlots();
}

void ChartLayout::newExternalChartObjectDone ()
{
  emit signalSetExternalChartObject();
}

int ChartLayout::plotWidth ()
{
  // find the best plot width for either < 1 column or > 1 column
  int maxWidth = 0;
  int minWidth = 999999;
  int cols = 1;
  QHashIterator<QString, Plot *> it(_plotList);
  while (it.hasNext())
  {
     it.next();
     Plot *p = it.value();

     if (p->column() > cols)
       cols = p->column();
     
     if (p->width() > maxWidth)
       maxWidth = p->width();

     if (p->width() < minWidth)
       minWidth = p->width();
  }

  if (cols > 1)
    return minWidth;
  else
    return maxWidth;
}

void ChartLayout::newIndicator ()
{
  IndicatorDialog *dialog = new IndicatorDialog;
  if (dialog->newDialog())
  {
    delete dialog;
    return;
  }

  QString name = dialog->name();

  delete dialog;

  addTab(name);

  emit signalRefresh();
}

void ChartLayout::editIndicator (QString indicator)
{
  IndicatorDialog *dialog = new IndicatorDialog;
  if (dialog->dialog(indicator))
  {
    delete dialog;
    return;
  }

  delete dialog;

  emit signalRefresh();
}

void ChartLayout::deleteIndicator (QString indicator)
{
  int rc = QMessageBox::warning(this,
                                tr("Qtstalker: Warning"),
                                tr("Are you sure you want to permanently delete this indicator?"),
                                QMessageBox::Yes,
                                QMessageBox::No,
                                QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  Indicator i;
  i.setName(indicator);
  IndicatorDataBase db;
  db.getIndicator(i);

  removeTab(i);
  
  db.deleteIndicator(indicator);

  emit signalStatus(QString(tr("Indicator ") + indicator + tr(" deleted")));
}

void ChartLayout::removeTab (Indicator &i)
{
  Plot *plot = _plotList.value(i.name());
  if (! plot)
    return;

  int row = plot->row();
  int col = plot->column();

  QString key = QString::number(row) + ":" + QString::number(col);
  TabWidget *tab = _tabs.value(key);
  if (! tab)
    return;

  tab->removeTab(tab->indexOf(plot));

  delete plot;
  _plotList.remove(i.name());

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

void ChartLayout::moveIndicator (QString indicator)
{
  Indicator oi, ni;
  oi.setName(indicator);
  IndicatorDataBase db;
  db.getIndicator(oi);
  ni = oi;

  IndicatorDialog *dialog = new IndicatorDialog;
  if (dialog->moveDialog(ni))
  {
    delete dialog;
    return;
  }

  delete dialog;

  if (oi.tabRow() == ni.tabRow() && oi.column() == ni.column())
    return;
  
  db.setIndicator(ni);

  removeTab(oi);
  
  addTab(ni);

  emit signalRefresh();
}

