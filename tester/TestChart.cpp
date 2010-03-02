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

TestChart::TestChart ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  setLayout(vbox);

  plot = new Plot(this);
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

  plot->setGridFlag(1);
  plot->setScaleToScreen(1);
  plot->setPixelspace(6);
  plot->setIndex(0);
  plot->setInterval((BarData::DailyBar));
  plot->setCrosshairsStatus(0);

//  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
//  connect(this, SIGNAL(signalInterval(BarData::BarLength)), plot, SLOT(setInterval(BarData::BarLength)));
  connect (slider, SIGNAL(valueChanged(int)), plot, SLOT(slotSliderChanged(int)));
}

void TestChart::update (BarData *data, QList<TestTrade *> &trades)
{
  plot->clear();
  
  PlotLine *bars = data->getInput(BarData::Close);
  if (! bars)
  {
    qDebug() << "TestChart::update: no bars";
    return;
  }
  
  QString s = "OHLC";
  bars->setPlugin(s);
  s = "green";
  bars->setColor(s);
  
  PluginFactory fac;
  QString path = "/usr/local/lib/qtstalker/plugins/object";
  QString plugin = "VLine";
  COPlugin *plug = fac.getCO(path, plugin);
  if (! plug)
  {
    qDebug() << "TestChart::update: no VLine plugin";
    delete bars;
    return;
  }
  
  int loop;
  int id = 0;
  IndicatorPlot *ip = plot->getIndicatorPlot();
  ip->addLine(bars);
  
  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);
    
    ChartObject *co = new ChartObject;
    ChartObject *co2 = new ChartObject;

    s = QString::number(id++);
    co->setData(ChartObject::ParmID, s);
    s = QString::number(id++);
    co2->setData(ChartObject::ParmID, s);

    s = "VLine";
    co->setData(ChartObject::ParmPlugin, s);
    co2->setData(ChartObject::ParmPlugin, s);
    
    if (trade->getType() == 0)
    {
      s = "green";
      co->setData(ChartObject::ParmColor, s);
      s = "red";
      co2->setData(ChartObject::ParmColor, s);
    }
    else
    {
      s = "red";
      co->setData(ChartObject::ParmColor, s);
      s = "green";
      co2->setData(ChartObject::ParmColor, s);
    }
    
    QDateTime dt;
    trade->getEnterDate(dt);
    co->setData(ChartObject::ParmDate, dt);
    trade->getExitDate(dt);
    co2->setData(ChartObject::ParmDate, dt);
    
    ip->addChartObject(co);  
    ip->addChartObject(co2);  
  }
  
  plot->draw();
}

