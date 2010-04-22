/*
 *  TestChart
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


#include <QVBoxLayout>
#include <QtDebug>
#include <QStringList>

#include "TestChart.h"
#include "Plot.h"
#include "COFactory.h"
#include "COPlugin.h"
#include "TestConfig.h"
#include "IndicatorPlot.h"
#include "COPlugin.h"
#include "BARSUtils.h"

TestChart::TestChart ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  setLayout(vbox);

  plot = new Plot(this);
  plot->setDateFlag(1);
  plot->setLogScale(0);
  plot->setGridFlag(0);
  plot->setScaleToScreen(1);
  plot->setPixelspace(6);
  plot->setIndex(0);
  vbox->addWidget(plot);

  slider = new QSlider;
  slider->setOrientation(Qt::Horizontal);
  vbox->addWidget(slider);

  // plot settings
  plot->setDateFlag(1);
  plot->setLogScale(0);
  
  QColor color("black");
  plot->setBackgroundColor(color);

  color.setNamedColor("white");
  plot->setBorderColor(color);
  
  color.setNamedColor("darkgray");
  plot->setGridColor(color);

  QFont font;
  plot->setPlotFont(font);

//  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
//  connect(this, SIGNAL(signalInterval(BarData::BarLength)), plot, SLOT(setInterval(BarData::BarLength)));
  connect (slider, SIGNAL(valueChanged(int)), plot, SLOT(slotSliderChanged(int)));
}

void TestChart::update (BarData &data, QList<TestTrade *> &trades)
{
  plot->clear();
  plot->setInterval(data.getBarLength());

  BARSUtils b;
  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  PlotLine *bars = b.getBARS(&data, up, down, neutral);
  if (! bars)
  {
    qDebug() << "TestChart::update: no bars";
    return;
  }
  
  Indicator i;
  QString s = "QtStalkerTester";
  i.setName(s);
  i.setIndicator(s);
  i.setEnable(1);
  i.setDate(1);
  i.addLine(bars);
  
  int loop;
  int id = 0;
  plot->setData(&data);
  
  COFactory fac;
  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);
    
    s = "Buy";
    COPlugin *buy = fac.getCO(s);
    s = "Sell";
    COPlugin *sell = fac.getCO(s);;

    buy->setID(id++);
    sell->setID(id++);
    
    s = data.getSymbol();
    buy->setSymbol(s);
    sell->setSymbol(s);
    
    s = "QtStalkerTester";
    buy->setIndicator(s);
    sell->setIndicator(s);

    QDateTime sd = trade->getEnterDate();
    QDateTime ed = trade->getExitDate();
    double low = trade->getLow() * 0.99;
    double high = trade->getHigh() * 1.01;
    if (trade->getType() == 0)
    {
//      buy->setData(ChartObject::ParmDate, sd);
//      buy->setData(ChartObject::ParmPrice, low);
//      sell->setData(ChartObject::ParmDate, ed);
//      sell->setData(ChartObject::ParmPrice, high);
    }
    else
    {
//      sell->setData(ChartObject::ParmDate, sd);
//      sell->setData(ChartObject::ParmPrice, high);
//      buy->setData(ChartObject::ParmDate, ed);
//      buy->setData(ChartObject::ParmPrice, low);
    }
    
    i.addChartObject(buy);  
    i.addChartObject(sell);  
  }

  IndicatorPlot *ip = plot->getIndicatorPlot();
  ip->setIndicator(i);
  
  slider->setRange(0, data.count() - 1);
  slider->setValue(0);
  
  plot->draw();
}

