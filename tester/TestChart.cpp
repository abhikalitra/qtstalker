/*
 *  TestChart
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


#include <QVBoxLayout>
#include <QtDebug>
#include <QStringList>

#include "TestChart.h"
#include "Plot.h"
#include "PluginFactory.h"
#include "COPlugin.h"
#include "TestConfig.h"
#include "IndicatorPlot.h"
#include "ChartObject.h"
#include "MATH1.h"

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
  plot->setScaleToScreen(0);
  plot->setPixelspace(6);
  plot->setIndex(0);
  plot->setCrosshairsStatus(0);
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

  TestConfig config;
  QString s;
  config.getData(TestConfig::PlotPluginPath, s);
  IndicatorPlot *ip = plot->getIndicatorPlot();
  ip->setPlotPluginPath(s);
  config.getData(TestConfig::COPluginPath, s);
  ip->setCOPluginPath(s);

//  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
//  connect(this, SIGNAL(signalInterval(BarData::BarLength)), plot, SLOT(setInterval(BarData::BarLength)));
  connect (slider, SIGNAL(valueChanged(int)), plot, SLOT(slotSliderChanged(int)));
}

void TestChart::update (BarData *data, QList<TestTrade *> &trades, QString &coPluginPath)
{
  plot->clear();
  plot->setInterval(data->getBarLength());

  MATH1 m;
  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  PlotLine *bars = m.getBARS(data, up, down, neutral);
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
  
  PluginFactory fac;
  QString plugin = "VLine";
  COPlugin *plug = fac.getCO(coPluginPath, plugin);
  if (! plug)
  {
    qDebug() << "TestChart::update: no VLine plugin";
    i.clear();
    return;
  }
  
  int loop;
  int id = 0;
  IndicatorPlot *ip = plot->getIndicatorPlot();
  ip->setData(data);
  
  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);
    
    ChartObject *buy = new ChartObject;
    ChartObject *sell = new ChartObject;

    s = QString::number(id++);
    buy->setData(ChartObject::ParmID, s);
    s = QString::number(id++);
    sell->setData(ChartObject::ParmID, s);
    
    data->getSymbol(s);
    buy->setData(ChartObject::ParmSymbol, s);
    sell->setData(ChartObject::ParmSymbol, s);
    
    s = "QtStalkerTester";
    buy->setData(ChartObject::ParmIndicator, s);
    sell->setData(ChartObject::ParmIndicator, s);

    s = "VLine";
    buy->setData(ChartObject::ParmPlugin, s);
    sell->setData(ChartObject::ParmPlugin, s);
    
    buy->setData(ChartObject::ParmColor, up);
    sell->setData(ChartObject::ParmColor, down);
    
    QDateTime sd, ed;
    double sp, ep;
    trade->getEnterDate(sd);
    trade->getExitDate(ed);
    sp = trade->getEnterPrice();
    ep = trade->getExitPrice();
    if (trade->getType() == 0)
    {
      buy->setData(ChartObject::ParmDate, sd);
      buy->setData(ChartObject::ParmPrice, sp);
      sell->setData(ChartObject::ParmDate, ed);
      sell->setData(ChartObject::ParmPrice, ep);
    }
    else
    {
      sell->setData(ChartObject::ParmDate, sd);
      sell->setData(ChartObject::ParmPrice, sp);
      buy->setData(ChartObject::ParmDate, ed);
      buy->setData(ChartObject::ParmPrice, ep);
    }
    
    i.addChartObject(buy);  
    i.addChartObject(sell);  
  }

  ip->setIndicator(i);
  
  slider->setRange(0, data->count() - 1);
  slider->setValue(0);
  
  plot->draw();
}

