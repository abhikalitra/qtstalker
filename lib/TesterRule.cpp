/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "TesterRule.h"

#include <QObject>



TesterRule::TesterRule ()
{
  setDouble(Account, 10000);
  setDouble(EntryCom, 10);
  setDouble(ExitCom, 10);
  setInt(Bars, 275);
  QString s = QObject::tr("Daily");
  setData(BarLength, s);
  s = QObject::tr("Mid Point");
  setData(PriceField, s);
}

void TesterRule::setName (QString &d)
{
  name = d;
}

void TesterRule::getName (QString &d)
{
  d = name;
}

void TesterRule::setParms (QString &d)
{
  QStringList l;
  l = d.split("|");

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    QStringList l2;
    l2 = l[loop].split("=");
    if (l2.count() != 2)
      continue;
    setData((TesterParm) l2[0].toInt(), l2[1]);
  }
}

void TesterRule::getParms (QString &d)
{
  QStringList l;
  QHashIterator<TesterParm, QString> it(data);
  while (it.hasNext())
  {
    it.next();
    l.append(QString::number(it.key()) + "=" + it.value());
  }
  d = l.join("|");
}

void TesterRule::setTrades (QString &d)
{
  trades = d;
}

void TesterRule::getTrades (QString &d)
{
  d = trades;
}

void TesterRule::setSummary (QString &d)
{
  summary = d;
}

void TesterRule::getSummary (QString &d)
{
  d = summary;
}

void TesterRule::setEnterLong (Indicator &d)
{
  enterLong = d;
}

void TesterRule::getEnterLong (Indicator &d)
{
  d = enterLong;
}

void TesterRule::setExitLong (Indicator &d)
{
  exitLong = d;
}

void TesterRule::getExitLong (Indicator &d)
{
  d = exitLong;
}

void TesterRule::setEnterShort (Indicator &d)
{
  enterShort = d;
}

void TesterRule::getEnterShort (Indicator &d)
{
  d = enterShort;
}

void TesterRule::setExitShort (Indicator &d)
{
  exitShort = d;
}

void TesterRule::getExitShort (Indicator &d)
{
  d = exitShort;
}

void TesterRule::setCustomLongStop (Indicator &d)
{
  customLongStop = d;
}

void TesterRule::getCustomLongStop (Indicator &d)
{
  d = customLongStop;
}

void TesterRule::setCustomShortStop (Indicator &d)
{
  customShortStop = d;
}

void TesterRule::getCustomShortStop (Indicator &d)
{
  d = customShortStop;
}

void TesterRule::setData (TesterParm p, QString &d)
{
  data.insert(p, d);
}

void TesterRule::getData (TesterParm p, QString &d)
{
  d.clear();
  d = data.value(p);
}

void TesterRule::setSymbols (QStringList &l)
{
  QString s = l.join(",");
  setData(Symbols, s);
}

void TesterRule::getSymbols (QStringList &l)
{
  QString s;
  getData(Symbols, s);
  l = s.split(",");
}

double TesterRule::getDouble (TesterParm p)
{
  QString d;
  d = data.value(p);
  return d.toDouble();
}

void TesterRule::setDouble (TesterParm p, double d)
{
  QString s = QString::number(d);
  data.insert(p, s);
}

int TesterRule::getInt (TesterParm p)
{
  QString d;
  d = data.value(p);
  return d.toInt();
}

void TesterRule::setInt (TesterParm p, int d)
{
  QString s = QString::number(d);
  data.insert(p, s);
}

