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
#include "BarData.h"
#include <qfileinfo.h>


Stocks::Stocks ()
{
  helpFile = "stocksplugin.html";
}

Stocks::~Stocks ()
{
}

void Stocks::dbPrefDialog ()
{
  StocksDialog *dialog = new StocksDialog(helpFile, this);
  dialog->exec();
  delete dialog;
}

Bar * Stocks::getBar (QString k, QString d)
{
  Bar *bar = new Bar;
  QStringList l = QStringList::split(",", d, FALSE);
  bar->setDate(k);
  bar->setOpen(l[0].toDouble());
  bar->setHigh(l[1].toDouble());
  bar->setLow(l[2].toDouble());
  bar->setClose(l[3].toDouble());
  bar->setVolume(l[4].toDouble());
  return bar;
}

void Stocks::setBar (Bar *bar)
{
  if (getHeaderField(BarType).toInt())
  {
    if (! bar->getTickFlag())
      return;
  }
  else
  {
    if (bar->getTickFlag())
      return;
  }

  QStringList l;
  l.append(QString::number(bar->getOpen()));
  l.append(QString::number(bar->getHigh()));
  l.append(QString::number(bar->getLow()));
  l.append(QString::number(bar->getClose()));
  l.append(QString::number(bar->getVolume(), 'f', 0));
  setData(bar->getDate().getDateTimeString(FALSE), l.join(","));
}

void Stocks::createNew ()
{
  QFileInfo fi(getHeaderField(Path));
  setHeaderField(BarType, QString::number(BarData::Daily));
  setHeaderField(Symbol, fi.fileName());
  setHeaderField(Title, fi.fileName());
  setHeaderField(Type, "Stock");
  setHeaderField(Plugin, "Stocks");
}

//********************************************************************
//********************************************************************
//********************************************************************

DbPlugin * createDbPlugin ()
{
  Stocks *o = new Stocks;
  return ((DbPlugin *) o);
}
