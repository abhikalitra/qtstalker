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

void Futures::setBar (Bar *bar)
{
  QStringList l;
  l.append(QString::number(bar->getOpen()));
  l.append(QString::number(bar->getHigh()));
  l.append(QString::number(bar->getLow()));
  l.append(QString::number(bar->getClose()));
  l.append(QString::number(bar->getVolume(), 'f', 0));
  l.append(QString::number(bar->getOI(), 'f', 0));
  setData(bar->getDate().getDateTimeString(FALSE), l.join(","));
}

void Futures::saveDbDefaults (Setting *set)
{
  setData("Symbol", set->getData("Symbol"));
  setData("Type", "Futures");
  setData("Title", set->getData("Title"));
  setData("BarType", set->getData("BarType"));
  setData("FuturesType", set->getData("FuturesType"));
  setData("FuturesMonth", set->getData("FuturesMonth"));
  setData("Plugin", "Futures");
}

DbPlugin * createDbPlugin ()
{
  Futures *o = new Futures;
  return ((DbPlugin *) o);
}

