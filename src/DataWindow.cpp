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
#include "Strip.h"

#include <QLayout>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QtDebug>
#include <QStringList>

DataWindow::DataWindow (QWidget *w) : QDialog (w, 0)
{
  setAttribute(Qt::WA_DeleteOnClose);
  _dateFlag = 0;
  _ohlcFlag = 0;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  _table = new QTableWidget;
  _table->setSortingEnabled(FALSE);
  vbox->addWidget(_table);

  resize(750, 550);
}

void DataWindow::setData (QHash<QString, PlotSettings> &list)
{
  QHashIterator<QString, PlotSettings> it(list);
  while (it.hasNext())
  {
    it.next();
    PlotSettings set = it.value();

    if (! _dateFlag)
      setDates(set);
    
    setPlot(set);
  }
}

void DataWindow::setPlot (PlotSettings &set)
{
  QHash<QString, Curve *> curves;
  set.plot->curves(curves);

  // get the plot data
  QHashIterator<QString, Curve *> it(curves);
  while (it.hasNext())
  {
    it.next();
    Curve *line = it.value();
    
    switch ((Curve::Type) line->type())
    {
      case Curve::Horizontal:
        break;
      case Curve::Candle:
      case Curve::OHLC:
        if (! _ohlcFlag)
          setOHLC(line);
        break;
      default:
        setLine(line);
        break;
    }
  }
}

void DataWindow::setDates (PlotSettings &set)
{
  QList<QDateTime> dates;
  set.plot->dates(dates);
  
  _table->setColumnCount(_table->columnCount() + 1);
  QTableWidgetItem *item = new QTableWidgetItem(tr("Date"));
  _table->setHorizontalHeaderItem(_table->columnCount() - 1, item);

  _table->setRowCount(dates.count());
  
  int loop;
  for (loop = 0; loop < (int) dates.count(); loop++)
  {
    QString s = dates.at(loop).toString("yyyy-MM-dd HH:mm:ss");
    
    QTableWidgetItem *item = new QTableWidgetItem(s);
    _table->setItem(loop, _table->columnCount() - 1, item);
  }
  
  _dateFlag = TRUE;
}

void DataWindow::setLine (Curve *line)
{
  Strip strip;
  QString s = line->label();

  _table->setColumnCount(_table->columnCount() + 1);
  QTableWidgetItem *item = new QTableWidgetItem(s);
  _table->setHorizontalHeaderItem(_table->columnCount() - 1, item);

  QList<int> keys;
  line->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = line->bar(keys.at(loop));
    strip.strip(bar->data(), 4, s);
    QTableWidgetItem *item = new QTableWidgetItem(s);
    _table->setItem(keys.at(loop), _table->columnCount() - 1, item);
  }
}

void DataWindow::setOHLC (Curve *line)
{
  Strip strip;
  QStringList l;
  l << tr("Open") << tr("High") << tr("Low") << tr("Close");
  
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    _table->setColumnCount(_table->columnCount() + 1);
    QTableWidgetItem *item = new QTableWidgetItem(l[loop]);
    _table->setHorizontalHeaderItem(_table->columnCount() - 1, item);
  }

  for (loop = 0; loop < line->count(); loop++)
  {
    CurveBar *bar = line->bar(loop);
    
    QString s;
    strip.strip(bar->data(0), 4, s);
    QTableWidgetItem *item = new QTableWidgetItem(s);
    _table->setItem(loop, _table->columnCount() - 4, item);

    strip.strip(bar->data(1), 4, s);
    item = new QTableWidgetItem(s);
    _table->setItem(loop, _table->columnCount() - 3, item);

    strip.strip(bar->data(2), 4, s);
    item = new QTableWidgetItem(s);
    _table->setItem(loop, _table->columnCount() - 2, item);

    strip.strip(bar->data(3), 4, s);
    item = new QTableWidgetItem(s);
    _table->setItem(loop, _table->columnCount() - 1, item);
  }
  
  _ohlcFlag = TRUE;
}

void DataWindow::scrollToBottom ()
{
  _table->scrollToBottom();
  _table->resizeColumnsToContents();
}
