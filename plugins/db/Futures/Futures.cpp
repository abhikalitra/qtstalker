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

#include "Futures.h"
#include "FuturesDialog.h"

Futures::Futures ()
{
}

Futures::~Futures ()
{
}

Bar * Futures::getBar (QString k, QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  Bar *bar = new Bar;
  bar->setDate(k);
  bar->setData("Open", l[0].toDouble());
  bar->setData("High", l[1].toDouble());
  bar->setData("Low", l[2].toDouble());
  bar->setData("Close", l[3].toDouble());
  bar->setData("Volume", l[4].toDouble());
  bar->setData("OI", l[5].toDouble());
  return bar;
}

void Futures::dbPrefDialog ()
{
  FuturesDialog *dialog = new FuturesDialog(path);
  dialog->exec();
  delete dialog;
}

void Futures::setBar (BarDate date, double open, double high, double low,
                     double close, double volume, double oi)
{
  QStringList l;
  l.append(QString::number(open));
  l.append(QString::number(high));
  l.append(QString::number(low));
  l.append(QString::number(close));
  l.append(QString::number(volume, 'f', 0));
  l.append(QString::number(oi, 'f', 0));
  setData(date.getDateTimeString(FALSE), l.join(","));
}

void Futures::saveDbDefaults (BarData::BarType barType, QString symbol, QString name, QString futuresType,
                              QString futuresMonth, QString, QString)
{
  setData("Symbol", symbol);
  setData("Type", "Futures");
  setData("Title", name);
  setData("BarType", QString::number(barType));
  setData("FuturesType", futuresType);
  setData("FuturesMonth", futuresMonth);
  setData("Plugin", "Futures");
}

Plugin * create ()
{
  Futures *o = new Futures;
  return ((Plugin *) o);
}

