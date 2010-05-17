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

#include "DataWindow.h"
#include "Plot.h"
#include "Utils.h"
#include "IndicatorDataBase.h"

#include <QLayout>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QtDebug>
#include <QStringList>

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

void DataWindow::setData (QHash<QString, Plot *> &list)
{
  IndicatorDataBase db;
  QStringList l;
  db.getIndicatorList(l);
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    Plot *plot = list.value(l[loop]);
    if (! plot)
      continue;

    setPlot(plot);
  }
}

void DataWindow::setPlot (Plot *d)
{
  Indicator indicator = d->indicator();
  QList<PlotLine *> lines;
  indicator.getLines(lines);
  
  if (! dateFlag)
  {
    // get the dates
    DateBar dates = d->dateBars();
    setDates(dates);
  }

  // get the plot data
  QString s;
  int loop;
  for (loop = 0; loop < lines.count(); loop++)
  {
    PlotLine *line = lines.at(loop);
    
    if (line->type() == "Horizontal")
      continue;

    if (line->type() == "OHLC" || line->type() == "Candle")
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
  QString s = line->label();

  table->setColumnCount(table->columnCount() + 1);
  QTableWidgetItem *item = new QTableWidgetItem(s);
  table->setHorizontalHeaderItem(table->columnCount() - 1, item);

  QList<int> keys;
  line->keys(keys);
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = line->data(keys.at(loop));
    util.strip(bar->data(), 4, s);
    QTableWidgetItem *item = new QTableWidgetItem(s);
    table->setItem(keys.at(loop), table->columnCount() - 1, item);
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
    PlotLineBar *bar = line->data(loop);
    
    QString s;
    util.strip(bar->data(0), 4, s);
    QTableWidgetItem *item = new QTableWidgetItem(s);
    table->setItem(loop, table->columnCount() - 4, item);

    util.strip(bar->data(1), 4, s);
    item = new QTableWidgetItem(s);
    table->setItem(loop, table->columnCount() - 3, item);

    util.strip(bar->data(2), 4, s);
    item = new QTableWidgetItem(s);
    table->setItem(loop, table->columnCount() - 2, item);

    util.strip(bar->data(3), 4, s);
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


