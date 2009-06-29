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
#include <QLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QTableWidgetItem>
#include <QList>
#include <QtDebug>


DataWindow::DataWindow (QWidget *w) : QDialog (w, 0)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  table = new QTableWidget;
//  table->setSelectionMode(QTableWidget::Single);
//  table->setReadOnly(TRUE);
//  table->setColumnWidth(col, 80);
  vbox->addWidget (table);

  resize(750, 550);
}

DataWindow::~DataWindow ()
{
}

void DataWindow::setData (int row, int col, QString &data)
{
  if (row > table->rowCount() -1)
    table->setRowCount(row + 1);

  QTableWidgetItem *ti = new QTableWidgetItem(data, 0);
  table->setItem(row, col, ti);
}

void DataWindow::setHeader (int col, QString &d)
{
  QTableWidgetItem *ti = new QTableWidgetItem(d, 0);
  table->setHorizontalHeaderItem(col, ti);
}

void DataWindow::setBars (BarData *d)
{
  if (! d->count())
    return;
    
  table->setColumnCount(2);
  table->setRowCount(d->count());

  QStringList sl;
  sl.append(tr("Date"));
  sl.append(tr("Time"));
  table->setHorizontalHeaderLabels(sl);
  
  int loop;
  for (loop = 0; loop < (int) d->count(); loop++)
  {
    QDateTime dt;
    d->getDate(loop, dt);
    QString s = dt.toString("yyyy-MM-dd");
    QTableWidgetItem *ti = new QTableWidgetItem(s, 0);
    table->setItem(loop, 0, ti);

    s = dt.toString("HH:mm:ss.zzz");
    ti = new QTableWidgetItem(s, 0);
    table->setItem(loop, 1, ti);
  }
}

void DataWindow::setPlot (Plot *d)
{
  IndicatorPlot *i = d->getIndicatorPlot();
  QList<PlotLine *> lines;
  i->getPlotList(lines);

  int loop2;
  QString s;
  for (loop2 = 0; loop2 < lines.count(); loop2++)
  {
    table->setColumnCount(table->columnCount() + 1);
      
    PlotLine *line = lines.at(loop2);
    line->getLabel(s);
    QTableWidgetItem *ti = new QTableWidgetItem(s, 0);
    table->setHorizontalHeaderItem(table->columnCount() - 1, ti);

    int loop3;
    int offset = table->rowCount() - line->getSize();
    for (loop3 = 0; loop3 < line->getSize(); loop3++)
    {
      ti = new QTableWidgetItem(strip(line->getData(loop3), 4), 0);
      table->setItem(loop3 + offset, table->columnCount() - 1, ti);
    }
//    table->adjustColumn(table->columnCount() - 1);
  }
}

QString DataWindow::strip (double d, int p)
{
  QString s = QString::number(d, 'f', p);

  while (1)
  {
    if (s.indexOf('.', -1, Qt::CaseSensitive) != -1)
    {
      s.truncate(s.length() - 1);
      break;
    }
    else
    {
      if (s.indexOf('0', -1, Qt::CaseSensitive) != -1)
        s.truncate(s.length() - 1);
      else
        break;
    }
  }

  return s;
}


