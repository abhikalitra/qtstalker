/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "Stocks.h"
#include "StocksDialog.h"

Stocks::Stocks ()
{
}

Stocks::~Stocks ()
{
}

Bar * Stocks::getBar (QString k, QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  Bar *bar = new Bar;
  bar->setDate(k);
  bar->setData("Open", l[0].toDouble());
  bar->setData("High", l[1].toDouble());
  bar->setData("Low", l[2].toDouble());
  bar->setData("Close", l[3].toDouble());
  bar->setData("Volume", l[4].toDouble());
  return bar;
}

void Stocks::dbPrefDialog ()
{
  StocksDialog *dialog = new StocksDialog(path);
  dialog->exec();
  delete dialog;
}

void Stocks::setBar (BarDate date, double open, double high, double low,
                     double close, double volume, double)
{
  QStringList l;
  l.append(QString::number(open));
  l.append(QString::number(high));
  l.append(QString::number(low));
  l.append(QString::number(close));
  l.append(QString::number(volume, 'f', 0));
  setData(date.getDateTimeString(FALSE), l.join(","));
}

void Stocks::saveDbDefaults (BarData::BarType barType, QString symbol, QString name, QString,
                             QString, QString, QString)
{
  setData("Symbol", symbol);
  setData("Type", "Stock");
  setData("Title", name);
  setData("BarType", QString::number(barType));
  setData("Plugin", "Stocks");
}

Plugin * create ()
{
  Stocks *o = new Stocks;
  return ((Plugin *) o);
}

