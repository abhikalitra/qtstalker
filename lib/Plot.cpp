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

#include "Plot.h"
#include "PlotGrid.h"
#include "Setting.h"
//#include "PlotButtonBox.h"

#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QList>
#include <QSpacerItem>

Plot::Plot (QWidget *w) : QWidget(w)
{
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(0);
  setLayout(hbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  hbox->addLayout(vbox);
  
  indicatorPlot = new IndicatorPlot(this);
  vbox->addWidget(indicatorPlot);

  datePlot = new DatePlot(this);
  vbox->addWidget(datePlot);
  
  vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  hbox->addLayout(vbox);
  
  scalePlot = new ScalePlot(this);
  vbox->addWidget(scalePlot);

  // add a spacer here to keep everything lined up
  vbox->addSpacerItem(new QSpacerItem(scalePlot->width(), datePlot->height()));

/*
  // add spacer to line up the date and scale plots
  PlotButtonBox *pbb = new PlotButtonBox(this);
  pbb->setMinimumHeight(datePlot->height());
  pbb->setMaximumHeight(datePlot->height());
  pbb->setMinimumWidth(scalePlot->width());
  pbb->setMaximumWidth(scalePlot->width());
  connect(pbb, SIGNAL(signalDateToggled()), this, SLOT(toggleDate()));
  connect(pbb, SIGNAL(signalLogToggled()), this, SLOT(toggleLog()));
  vbox->addWidget(pbb);
*/

  connect(indicatorPlot, SIGNAL(signalDraw()), this, SLOT(slotUpdateScalePlot()));
  connect(indicatorPlot, SIGNAL(signalDateFlag(bool)), this, SLOT(slotDateFlagChanged(bool)));
  connect(indicatorPlot, SIGNAL(signalLogFlag(bool)), this, SLOT(slotLogScaleChanged(bool)));
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

//  indicatorPlot->setXGrid(datePlot->getXGrid());

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

void Plot::setInfoFlag (bool d)
{
  indicatorPlot->setInfoFlag(d);
}

void Plot::draw ()
{
  datePlot->draw();
  indicatorPlot->draw();
  scalePlot->setScaler(indicatorPlot->getScaler());
  
  QList<Setting> l;
  indicatorPlot->getScalePoints(l);
  scalePlot->setScalePoints(l);
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

void Plot::setInterval (Bar::BarLength d)
{
  datePlot->setInterval(d);
  indicatorPlot->setInterval(d);
}

void Plot::setDateFlag (bool d)
{
  indicatorPlot->setDateFlag(d);

  if (d)
  {
    datePlot->show();
    datePlot->draw();
  }
  else
    datePlot->hide();
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

void Plot::slotDateFlagChanged (bool d)
{
  setDateFlag(d);
}

void Plot::slotLogScaleChanged (bool d)
{
  setLogScale(d);
  indicatorPlot->draw();
}

int Plot::getWidth ()
{
  return indicatorPlot->getWidth();
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

void Plot::setMenuFlag (bool d)
{
  indicatorPlot->setMenuFlag(d);
}

void Plot::loadChartObjects ()
{
  indicatorPlot->loadChartObjects();
}

void Plot::toggleDate ()
{
  if (datePlot->isHidden())
    slotDateFlagChanged(TRUE);
  else
    slotDateFlagChanged(FALSE);
}

void Plot::toggleLog ()
{
  if (indicatorPlot->getLogScale())
    slotLogScaleChanged(FALSE);
  else
    slotLogScaleChanged(TRUE);
}



