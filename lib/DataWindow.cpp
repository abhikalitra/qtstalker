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
#include "Global.h"
#include "GlobalSymbol.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "CurveType.h"

#include <QtDebug>

DataWindow::DataWindow (QWidget *p) : QDialog (p, 0)
{
  setAttribute(Qt::WA_DeleteOnClose);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  _table = new QTableWidget;
  _table->setSortingEnabled(FALSE);
  vbox->addWidget(_table);

  resize(750, 550);

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  QStringList l;
  l << "QtStalker" + g_session << ":" << g_currentSymbol->exchange();
  l << ":" << g_currentSymbol->symbol();
  l << "(" + g_currentSymbol->name() + ")";
  l << "-" << tr("Indicators");
  setWindowTitle(l.join(" "));
}

void DataWindow::setPlot (Plot *p)
{
  // get dates first
  QList<QDateTime> dates;
  p->dates(dates);
  int loop = 0;
  for (; loop < dates.count(); loop++)
  {
    Message *bar = _bars.value(loop);
    if (! bar)
    {
      bar = new Message;
      _bars.insert(loop, bar);
    }

    bar->insert("Date", dates.at(loop).toString(Qt::ISODate));
  }

  QHash<QString, Curve *> curves = p->curves();

  CurveType ct;
  Strip strip;
  QHashIterator<QString, Curve *> it(curves);
  while (it.hasNext())
  {
    it.next();
    Curve *line = it.value();
    Data *d = line->settings();

    QList<int> keys = d->keys();

    int loop = 0;
    for (; loop < keys.count(); loop++)
    {
      Data *cb = d->toData(keys.at(loop));

      Message *bar = _bars.value(keys.at(loop));
      if (! bar)
      {
        bar = new Message;
        _bars.insert(keys.at(loop), bar);
      }

      switch ((CurveType::Type) ct.stringToType(d->toData(CurveData::_TYPE)->toString()))
      {
        case CurveType::_OHLC:
        {
          _headers.insert(tr("Open"), tr("Open"));
          _headers.insert(tr("High"), tr("High"));
          _headers.insert(tr("Low"), tr("Low"));
          _headers.insert(tr("Close"), tr("Close"));

          QString s;
          strip.strip(cb->toData(CurveBar::_OPEN)->toDouble(), 4, s);
          bar->insert(tr("Open"), s);

          strip.strip(cb->toData(CurveBar::_HIGH)->toDouble(), 4, s);
          bar->insert(tr("High"), s);

          strip.strip(cb->toData(CurveBar::_LOW)->toDouble(), 4, s);
          bar->insert(tr("Low"), s);

          strip.strip(cb->toData(CurveBar::_CLOSE)->toDouble(), 4, s);
          bar->insert(tr("Close"), s);
          break;
        }
        case CurveType::_HISTOGRAM:
        {
          _headers.insert(d->toData(CurveData::_LABEL)->toString(), d->toData(CurveData::_LABEL)->toString());

          QString s;
          strip.strip(cb->toData(CurveBar::_HIGH)->toDouble(), 4, s);
          bar->insert(d->toData(CurveData::_LABEL)->toString(), s);
          break;
        }
        default:
        {
          _headers.insert(d->toData(CurveData::_LABEL)->toString(), d->toData(CurveData::_LABEL)->toString());

          QString s;
          strip.strip(cb->toData(CurveBar::_VALUE)->toDouble(), 4, s);
          bar->insert(d->toData(CurveData::_LABEL)->toString(), s);
          break;
        }
      }
    }
  }
}

void DataWindow::scrollToBottom ()
{
  QList<QString> hkeys = _headers.keys();
  qSort(hkeys);
  hkeys.removeAll("Date");
  hkeys.prepend("Date");

  int loop = 0;
  for (; loop < hkeys.count(); loop++)
  {
    _table->setColumnCount(_table->columnCount() + 1);
    QTableWidgetItem *item = new QTableWidgetItem(hkeys.at(loop));
    _table->setHorizontalHeaderItem(_table->columnCount() - 1, item);
  }

  QList<int> keys = _bars.keys();
  qSort(keys);

  _table->setRowCount(keys.count());

  for (loop = 0; loop < keys.count(); loop++)
  {
     Message *bar = _bars.value(keys.at(loop));

     int loop2 = 0;
     for (; loop2 < hkeys.count(); loop2++)
     {
        QTableWidgetItem *item = new QTableWidgetItem(bar->value(hkeys.at(loop2)));
        _table->setItem(keys.at(loop), loop2, item);
     }
  }

  qDeleteAll(_bars);
  _bars.clear();

  _table->scrollToBottom();
  _table->resizeColumnsToContents();
}
