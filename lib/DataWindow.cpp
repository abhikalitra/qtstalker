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
#include "CurveType.h"
#include "SymbolKey.h"
#include "CurveDataKey.h"
#include "CurveBarKey.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

DataWindow::DataWindow (QWidget *p) : QDialog (p, 0)
{
  _dateFlag = 0;
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

  SymbolKey symkeys;
  Data symbol;
  g_currentSymbol.toData(symkeys.indexToString(SymbolKey::_SYMBOL), symbol);
  
  Data name;
  g_currentSymbol.toData(symkeys.indexToString(SymbolKey::_NAME), name);

  QStringList l;
  l << "QtStalker" + g_session << ":" << symbol.toString();
  l << "(" + name.toString() + ")";
  l << "-" << tr("Indicators");
  setWindowTitle(l.join(" "));
}

void DataWindow::setPlot (Plot *p)
{
  setDates(p);
  
  QHash<QString, Curve *> curves = p->curves();

  CurveDataKey cdkeys;
  CurveBarKey cbkeys;
  CurveType ct;
  Strip strip;
  QHashIterator<QString, Curve *> it(curves);
  while (it.hasNext())
  {
    it.next();
    Curve *line = it.value();
    Entity settings = line->settings();

    Data curveType;
    settings.toData(cdkeys.indexToString(CurveDataKey::_TYPE), curveType);
      
    QList<QString> keys = settings.ekeys();

    int loop = 0;
    for (; loop < keys.size(); loop++)
    {
      Entity cbar;
      settings.toEntity(keys.at(loop), cbar);

      Entity dwBar = _bars.value(keys.at(loop));

      switch ((CurveType::Type) ct.stringToIndex(curveType.toString()))
      {
        case CurveType::_OHLC:
        {
          _headers.set(tr("Open"), Data(tr("Open")));
          _headers.set(tr("High"), Data(tr("High")));
          _headers.set(tr("Low"), Data(tr("Low")));
          _headers.set(tr("Close"), Data(tr("Close")));

          QString s;
	  Data td;
	  cbar.toData(cbkeys.indexToString(CurveBarKey::_OPEN), td);
          strip.strip(td.toDouble(), 4, s);
          dwBar.set(tr("Open"), Data(s));

	  cbar.toData(cbkeys.indexToString(CurveBarKey::_HIGH), td);
          strip.strip(td.toDouble(), 4, s);
          dwBar.set(tr("High"), Data(s));

	  cbar.toData(cbkeys.indexToString(CurveBarKey::_LOW), td);
          strip.strip(td.toDouble(), 4, s);
          dwBar.set(tr("Low"), Data(s));

	  cbar.toData(cbkeys.indexToString(CurveBarKey::_CLOSE), td);
          strip.strip(td.toDouble(), 4, s);
          dwBar.set(tr("Close"), Data(s));
          break;
        }
        case CurveType::_HISTOGRAM:
        {
	  Data label;
	  settings.toData(cdkeys.indexToString(CurveDataKey::_LABEL), label);
          _headers.set(label.toString(), label);

          QString s;
	  Data td;
	  cbar.toData(cbkeys.indexToString(CurveBarKey::_HIGH), td);
          strip.strip(td.toDouble(), 4, s);
          dwBar.set(label.toString(), Data(s));
          break;
        }
        default:
        {
	  Data label;
	  settings.toData(cdkeys.indexToString(CurveDataKey::_LABEL), label);
          _headers.set(label.toString(), label);

          QString s;
	  Data td;
	  cbar.toData(cbkeys.indexToString(CurveBarKey::_VALUE), td);
          strip.strip(td.toDouble(), 4, s);
          dwBar.set(label.toString(), Data(s));
          break;
        }
      }
      
      _bars.insert(keys.at(loop), dwBar);
    }
  }
}

void DataWindow::scrollToBottom ()
{
  QList<QString> hkeys = _headers.dkeys();
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

  ScriptVerifyCurveKeys svck;
  QList<QString> tkeys = _bars.keys();
  QList<QString> keys;
  svck.sortKeys(tkeys, keys);

  _table->setRowCount(keys.size());

  for (loop = 0; loop < keys.size(); loop++)
  {
    Entity bar = _bars.value(keys.at(loop));

    int loop2 = 0;
    for (; loop2 < hkeys.size(); loop2++)
    {
      Data td;
      bar.toData(hkeys.at(loop2), td);
       
      QTableWidgetItem *item = new QTableWidgetItem(td.toString());
      _table->setItem(keys.at(loop).toInt(), loop2, item);
    }
  }

  _bars.clear();

  _table->scrollToBottom();
  _table->resizeColumnsToContents();
}

void DataWindow::setDates (Plot *p)
{
  if (_dateFlag)
    return;
  
  QList<QDateTime> dates;
  p->dates(dates);
  
  int loop = 0;
  for (; loop < dates.size(); loop++)
  {
    Entity bar;
    bar.set(QString("Date"), dates.at(loop));
    _bars.insert(QString::number(loop), bar);
  }
    
  _dateFlag++;
}
