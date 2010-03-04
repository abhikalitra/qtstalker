/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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

#include "DataWindow.h"
#include "IndicatorPlot.h"
#include "Utils.h"

#include <QLayout>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QtDebug>

DataWindow::DataWindow (QWidget *w) : QDialog (w, 0)
{
  dateFlag = 0;
  ohlcFlag = 0;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  table = new QTableWidget;
  table->setSortingEnabled(FALSE);
  vbox->addWidget (table);

  resize(750, 550);
}

void DataWindow::setPlot (Plot *d)
{
  IndicatorPlot *ip = d->getIndicatorPlot();
  Indicator indicator;
  ip->getIndicator(indicator);
  QList<PlotLine *> lines;
  indicator.getLines(lines);
  
  if (! dateFlag)
  {
    // get the dates
    DateBar dates;
    ip->getDateBar(dates);
    setDates(dates);
  }

  // get the plot data
  QString s;
  int loop;
  for (loop = 0; loop < lines.count(); loop++)
  {
    PlotLine *line = lines.at(loop);
    
    line->getPlugin(s);
    if (s == "Horizontal")
      continue;

    if (s == "OHLC" || s == "Candle")
    {
      if (ohlcFlag)
	continue;
      
      setOHLC(line);
    }
    else
      setLine(line);
  }
}

void DataWindow::setDates (DateBar &dates)
{
  table->setColumnCount(table->columnCount() + 1);
  QTableWidgetItem *item = new QTableWidgetItem(tr("Date"));
  table->setHorizontalHeaderItem(table->columnCount() - 1, item);

  table->setRowCount(dates.count());
  
  int loop;
  for (loop = 0; loop < (int) dates.count(); loop++)
  {
    QDateTime dt;
    dates.getDate(loop, dt);
    QString s = dt.toString(Qt::ISODate);
    
    QTableWidgetItem *item = new QTableWidgetItem(s);
    table->setItem(loop, table->columnCount() - 1, item);
  }
  
  dateFlag = TRUE;
}

void DataWindow::setLine (PlotLine *line)
{
  Utils util;
  QString s;
  line->getLabel(s);

  table->setColumnCount(table->columnCount() + 1);
  QTableWidgetItem *item = new QTableWidgetItem(s);
  table->setHorizontalHeaderItem(table->columnCount() - 1, item);

  int dataLoop = table->rowCount() - line->count();
  int loop;
  for (loop = 0; loop < line->count(); loop++, dataLoop++)
  {
    util.strip(line->getData(loop), 4, s);
    QTableWidgetItem *item = new QTableWidgetItem(s);
    table->setItem(dataLoop, table->columnCount() - 1, item);
  }
}

void DataWindow::setOHLC (PlotLine *line)
{
  Utils util;
  QStringList l;
  l << tr("Open") << tr("High") << tr("Low") << tr("Close");
  
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    table->setColumnCount(table->columnCount() + 1);
    QTableWidgetItem *item = new QTableWidgetItem(l[loop]);
    table->setHorizontalHeaderItem(table->columnCount() - 1, item);
  }

  for (loop = 0; loop < line->count(); loop++)
  {
    PlotLineBar bar;
    line->getData(loop, bar);
    
    QString s;
    util.strip(bar.getData(0), 4, s);
    QTableWidgetItem *item = new QTableWidgetItem(s);
    table->setItem(loop, table->columnCount() - 4, item);

    util.strip(bar.getData(1), 4, s);
    item = new QTableWidgetItem(s);
    table->setItem(loop, table->columnCount() - 3, item);

    util.strip(bar.getData(2), 4, s);
    item = new QTableWidgetItem(s);
    table->setItem(loop, table->columnCount() - 2, item);

    util.strip(bar.getData(3), 4, s);
    item = new QTableWidgetItem(s);
    table->setItem(loop, table->columnCount() - 1, item);
  }
  
  ohlcFlag = TRUE;
}

void DataWindow::scrollToBottom ()
{
  table->scrollToBottom();
  table->resizeColumnsToContents();
}


