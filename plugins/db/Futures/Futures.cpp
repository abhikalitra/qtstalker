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
#include "BarData.h"

Futures::Futures ()
{
  helpFile = "futuresplugin.html";
}

Futures::~Futures ()
{
}

void Futures::dbPrefDialog ()
{
  FuturesDialog *dialog = new FuturesDialog(helpFile, this);
  dialog->exec();
  delete dialog;
}

Bar * Futures::getBar (QString &k, QString &d)
{
  Bar *bar = new Bar;
  QStringList l = QStringList::split(",", d, FALSE);
  bar->setDate(k);
  bar->setOpen(l[0].toDouble());
  bar->setHigh(l[1].toDouble());
  bar->setLow(l[2].toDouble());
  bar->setClose(l[3].toDouble());
  bar->setVolume(l[4].toDouble());
  bar->setOI(l[5].toInt());
  return bar;
}

void Futures::setBar (Bar &bar)
{
  QString k;
  getHeaderField(BarType, k);
  if (k.toInt() != bar.getTickFlag())
    return;

  k = bar.getDate().getDateTimeString(FALSE);
  
  QString d = QString::number(bar.getOpen()) + "," + QString::number(bar.getHigh()) + "," +
              QString::number(bar.getLow()) + "," + QString::number(bar.getClose()) + "," +
	      QString::number(bar.getVolume(), 'f', 0) + "," + QString::number(bar.getOI());
  
  setData(k, d);
}

void Futures::createNew ()
{
  QString s = QString::number(BarData::Daily);
  setHeaderField(BarType, s);
  s = "Futures";
  setHeaderField(Type, s);
  setHeaderField(Plugin, s);
}

//********************************************************************
//********************************************************************
//********************************************************************

DbPlugin * createDbPlugin ()
{
  Futures *o = new Futures;
  return ((DbPlugin *) o);
}

