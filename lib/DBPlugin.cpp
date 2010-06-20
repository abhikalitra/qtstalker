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

#include "DBPlugin.h"
#include "Bar.h"

#include <QDebug>
#include <QtSql>

DBPlugin::DBPlugin ()
{
  _dbName = "quotes";
}

// virtual
DBPlugin::~DBPlugin ()
{
  if (quotes.count())
    qDeleteAll(quotes);
}

// virtual
void DBPlugin::getBars (BarData &)
{
}

// virtual
void DBPlugin::dialog ()
{
}

// virtual
int DBPlugin::scriptCommand (QStringList &, Indicator &, QByteArray &)
{
  return 0;
}

// virtual
int DBPlugin::deleteSymbol (BarData *)
{
  return 0;
}

// virtual
int DBPlugin::setDetail (QString &, BarData *, QString &)
{
  return 0;
}

// virtual
int DBPlugin::setDetail (int, BarData *, QString &)
{
  return 0;
}

// virtual
int DBPlugin::detail (QString &, BarData *, QString &)
{
  return 0;
}

// virtual
int DBPlugin::detail (int, BarData *, QString &)
{
  return 0;
}

/*********************************************************/
/*********************************************************/
/*********************************************************/

void DBPlugin::getFirstDate (QString &table, QDateTime &date)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT min(date) FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::getFirstDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void DBPlugin::getLastDate (QString &table, QDateTime &date)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT max(date) FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::getLastDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void DBPlugin::barErrorMessage (int rc)
{
  QString function = "::scriptCommand:";
  
  switch ((Bar::RC) rc)
  {
    case Bar::RC_InvalidOpen:
      qDebug() << plugin << function << "invalid open";
      break;
    case Bar::RC_InvalidHigh:
      qDebug() << plugin << function << "invalid high";
      break;
    case Bar::RC_InvalidLow:
      qDebug() << plugin << function << "invalid low";
      break;
    case Bar::RC_InvalidClose:
      qDebug() << plugin << function << "invalid close";
      break;
    case Bar::RC_InvalidVolume:
      qDebug() << plugin << function << "invalid volume";
      break;
    case Bar::RC_InvalidOI:
      qDebug() << plugin << function << "invalid oi";
      break;
    case Bar::RC_OGTH: // open > high
      qDebug() << plugin << function << "open > high";
      break;
    case Bar::RC_OLTL: // open < low
      qDebug() << plugin << function << "open < low";
      break;
    case Bar::RC_CGTH: // close > high
      qDebug() << plugin << function << "close > high";
      break;
    case Bar::RC_CLTL: // close < low
      qDebug() << plugin << function << "close < low";
      break;
    case Bar::RC_VLT0: // volume < 0
      qDebug() << plugin << function << "volume < 0";
      break;
    case Bar::RC_LGTH: // low > high
      qDebug() << plugin << function << "low > high";
      break;
    case Bar::RC_HLTL: // high < low
      qDebug() << plugin << function << "high < low";
      break;
    case Bar::RC_OILT0: // oi < 0
      qDebug() << plugin << function << "oi < 0";
      break;
    default:
      break;
  }
}

