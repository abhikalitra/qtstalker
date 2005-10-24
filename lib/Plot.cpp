/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "Plot.h"
#include <qlayout.h>

Plot::Plot (QWidget *w) : QWidget(w)
{
  dateFlag = FALSE;
  tabFlag = TRUE;
  
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(0);
  vbox->setSpacing(0);
  
  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setMargin(0);
  hbox->setSpacing(0);

  indicatorPlot = new IndicatorPlot(this);
  hbox->addWidget(indicatorPlot, 1, 0);
  
  scalePlot = new ScalePlot(this);
  hbox->addWidget(scalePlot);
  
  datePlot = new DatePlot(this);
  vbox->addWidget(datePlot);
  
  connect(indicatorPlot, SIGNAL(signalDraw()), this, SLOT(slotUpdateScalePlot()));
}

Plot::~Plot ()
{
}

void Plot::clear ()
{
  datePlot->clear();
  scalePlot->clear();
  indicatorPlot->clear();
}

void Plot::setData (BarData *l)
{
  if (! l->count())
    return;

  datePlot->setData(l);
  indicatorPlot->setXGrid(datePlot->getXGrid());
  
  scalePlot->setData(l->getClose(l->count() - 1));
  
  indicatorPlot->setData(l);
}

void Plot::setScaleToScreen (bool d)
{
  indicatorPlot->setScaleToScreen(d);
  scalePlot->setScaleToScreen(d);
}

void Plot::setLogScale (bool d)
{
  indicatorPlot->setLogScale(d);
  scalePlot->setLogScale(d);
}

void Plot::setChartPath (QString d)
{
  indicatorPlot->setChartPath(d);
}

void Plot::setDrawMode (bool d)
{
  indicatorPlot->setDrawMode(d);
}

bool Plot::getCrosshairsStatus ()
{
  return indicatorPlot->getCrosshairsStatus();
}

void Plot::setInfoFlag (bool d)
{
  indicatorPlot->setInfoFlag(d);
}

void Plot::draw ()
{
  datePlot->draw();
  indicatorPlot->draw();
  scalePlot->setScaler(indicatorPlot->getScaler());
  scalePlot->draw();
}

void Plot::drawRefresh ()
{
  datePlot->drawRefresh();
  indicatorPlot->drawRefresh();
  scalePlot->drawRefresh();
}

void Plot::setBackgroundColor (QColor d)
{
  datePlot->setBackgroundColor(d);
  scalePlot->setBackgroundColor(d);
  indicatorPlot->setBackgroundColor(d);
}

void Plot::setBorderColor (QColor d)
{
  datePlot->setBorderColor(d);
  scalePlot->setBorderColor(d);
  indicatorPlot->setBorderColor(d);
}

void Plot::setGridColor (QColor d)
{
  indicatorPlot->setGridColor(d);
}

void Plot::setPlotFont (QFont d)
{
  datePlot->setPlotFont(d);
  scalePlot->setPlotFont(d);
  indicatorPlot->setPlotFont(d);
}

void Plot::setGridFlag (bool d)
{
  indicatorPlot->setGridFlag(d);
}

void Plot::setPixelspace (int d)
{
  datePlot->setPixelspace(d);
  indicatorPlot->setPixelspace(d);
}

void Plot::setIndex (int d)
{
  datePlot->setIndex(d);
  indicatorPlot->setIndex(d);
}

void Plot::setTabFlag (bool d)
{
  tabFlag = d;
}

bool Plot::getTabFlag ()
{
  return tabFlag;
}

void Plot::setInterval (BarData::BarCompression d)
{
  datePlot->setInterval(d);    
  indicatorPlot->setInterval(d);    
}

void Plot::setDateFlag (bool d)
{
  dateFlag = d;
  
  if (dateFlag)
    datePlot->show();
  else
    datePlot->hide();
}

void Plot::addIndicator (QString &d, Indicator *i)
{
  indicatorPlot->addIndicator(d, i);
}

Indicator * Plot::getIndicator (QString &d)
{
  return indicatorPlot->getIndicator(d);
}

bool Plot::deleteIndicator (QString &d)
{
  return indicatorPlot->deleteIndicator(d);
}

void Plot::getIndicators (QStringList &l)
{
  indicatorPlot->getIndicators(l);
}

void Plot::setCrosshairsStatus (bool status)
{
  indicatorPlot->setCrosshairsStatus(status);
}

void Plot::slotSliderChanged (int v)
{
  setIndex(v);
  draw();
}

void Plot::slotGridChanged (bool d)
{
  setGridFlag(d);
}

void Plot::slotScaleToScreenChanged (bool d)
{
  setScaleToScreen(d);
  indicatorPlot->draw();
}

void Plot::slotDrawModeChanged (bool d)
{
  setDrawMode(d);
}

void Plot::slotDateFlagChanged (bool d)
{
  setDateFlag(d);
}

void Plot::slotLogScaleChanged (bool d)
{
  setLogScale(d);
  indicatorPlot->draw();
}

void Plot::addChartObject (Setting &set)
{
  indicatorPlot->addChartObject(set);
}

void Plot::setCrosshairsFlag (bool d)
{
  indicatorPlot->setCrosshairsFlag(d);
}

void Plot::crossHair (int d, int d2, bool d3)
{
  indicatorPlot->crossHair(d, d2, d3);
}

int Plot::getWidth ()
{
  return indicatorPlot->width();
}

IndicatorPlot * Plot::getIndicatorPlot ()
{
  return indicatorPlot;
}

DatePlot * Plot::getDatePlot ()
{
  return datePlot;
}

void Plot::slotUpdateScalePlot ()
{
  scalePlot->setScaler(indicatorPlot->getScaler());
  scalePlot->draw();
}



