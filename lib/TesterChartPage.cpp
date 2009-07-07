/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include <QLayout>
#include <QGroupBox>
#include <QFont>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>

#include "TesterChartPage.h"
#include "IndicatorPlugin.h"




TesterChartPage::TesterChartPage (QWidget *p) : QWidget (p)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  setLayout(vbox);

  split = new QSplitter;
  split->setOrientation(Qt::Vertical);
  vbox->addWidget(split);

  equityPlot = new Plot (split);
  equityPlot->setGridFlag(TRUE);
  equityPlot->setPixelspace(5);
  equityPlot->setIndex(0);
  equityPlot->setDateFlag(TRUE);
  equityPlot->setInfoFlag(FALSE);
  equityPlot->setCrosshairsFlag (FALSE); // turn off crosshairs
  equityPlot->setMenuFlag(FALSE);
  QObject::connect(this, SIGNAL(signalIndex(int)), equityPlot, SLOT(setIndex(int)));
  
  plot = new Plot (split);
  plot->setGridFlag(TRUE);
  plot->setPixelspace(5);
  plot->setIndex(0);
  plot->setDateFlag(TRUE);
  plot->setInfoFlag(FALSE);
  plot->setCrosshairsFlag (FALSE); // turn off crosshairs
  plot->setMenuFlag(FALSE);
  QObject::connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));

  QList<int> sizeList = split->sizes();
  sizeList[1] = 100;
  split->setSizes(sizeList);

  QHBoxLayout *hbox = new QHBoxLayout;
  vbox->addLayout(hbox);

  slider = new QSlider;
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderChanged(int)));
  hbox->addWidget(slider);
}

void TesterChartPage::slotSliderChanged (int v)
{
  emit signalIndex(v);
  plot->draw();
  equityPlot->draw();
}

void TesterChartPage::updateChart (BarData *recordList, QList<TradeItem*> &trades, double eq)
{
  plot->setData(recordList);
  equityPlot->setData(recordList);
  
  //set up indicator
  PlotLine *bars = new PlotLine;
  QColor green("green");
  QColor red("red");
  QColor blue("blue");
  QColor color;
  bars->setType(PlotLine::Bar);

  IndicatorPlugin *i = new IndicatorPlugin;
  QString s = "bars";
  i->setName(s);
//  i->addLine(bars);
  i->getName(s);
//  plot->addIndicator(i);

  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    bars->append(blue, recordList->getOpen(loop), recordList->getHigh(loop),
                 recordList->getLow(loop), recordList->getClose(loop), FALSE);
  }

  for (loop = 0; loop < (int) trades.count(); loop++)
  {
    TradeItem *trade = trades.at(loop);
    QDateTime edate, xdate;
    trade->getEnterDate(edate);
    trade->getExitDate(xdate);

    if (trade->getTradePosition() == TradeItem::Long)
      color = green;
    else
      color = red;

    int i = recordList->getX(edate);
    int j = recordList->getX(xdate);

    int loop2;
    for (loop2 = i; loop2 <= j; loop2++)
      bars->setColorBar(loop2, color);
  }

  slider->setMaximum(recordList->count() - 1);
        
  plot->draw();

  //setup the equity line
  PlotLine *el = new PlotLine;
  el->setColor(green);
  
  int tloop = 0;
  double equity = eq;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    QDateTime rdate, edate, xdate;
    recordList->getDate(loop, rdate);
    TradeItem *trade = trades.at(tloop);
    if (! trade)
      continue;
    trade->getEnterDate(edate);
    trade->getExitDate(xdate);

    // check if we are in a trade
    if (rdate >= edate && rdate < xdate)
    {
      double t = equity + trade->getCurrentProfit(recordList->getClose(loop));
      el->append(t);
    }
    else
    {
      // check if we are ready to close trade out 
      if (rdate == xdate)
      {
        equity = trade->getBalance();
        tloop++;
        if (tloop >= (int) trades.count())
          tloop = trades.count() - 1;
        el->append(equity);
      }
      else
        el->append(equity);
    }
  }

  IndicatorPlugin *i2 = new IndicatorPlugin;
  QString str = "equity";
  i2->setName(str);
//  i2->addLine(el);
  i2->getName(str);
//  equityPlot->addIndicator(i2);
  equityPlot->draw();
}

void TesterChartPage::clear ()
{
  plot->clear();
  equityPlot->clear();
}

