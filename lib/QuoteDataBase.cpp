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

#include "QuoteDataBase.h"
#include "Bar.h"
#include "Setting.h"

#include <QtDebug>
#include <QtSql>
#include <QVariant>


QuoteDataBase::QuoteDataBase ()
{
}

void QuoteDataBase::init (QString &dbFile)
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "quotes");
  db.setHostName("localhost");
  db.setDatabaseName(dbFile);
  db.setUserName("QtStalker");
  db.setPassword("QtStalker");
  if (! db.open())
    qDebug() << "QuoteDataBase::init: quotes db open failed";

  QSqlQuery q(db);
  QString s = "CREATE TABLE IF NOT EXISTS symbolIndex (";
  s.append(" symbol TEXT PRIMARY KEY UNIQUE");
  s.append(", tableName TEXT");
  s.append(", name TEXT");
  s.append(", exchange TEXT");
  s.append(", type TEXT");
  s.append(", sector TEXT");
  s.append(", futuresSymbol TEXT");
  s.append(", futuresMonth TEXT");
  s.append(", futuresTick REAL");
  s.append(", data TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "QuoteDataBase::init:createSymbolIndexTable: " << q.lastError().text();
}

void QuoteDataBase::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database("quotes");
  db.transaction();
}

void QuoteDataBase::commit ()
{
  QSqlDatabase db = QSqlDatabase::database("quotes");
  db.commit();
}

void QuoteDataBase::getAllChartsList (QStringList &l)
{
  l.clear();

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT symbol FROM symbolIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getAllChartsList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void QuoteDataBase::getSearchList (QString &pat, QStringList &l)
{
  l.clear();

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT symbol FROM symbolIndex WHERE symbol LIKE";
  s.append(" '" + pat + "'"); // add the pattern to the end of the sql command
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getSearchList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void QuoteDataBase::getChart (BarData *data)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));

  QString symbol, ts, ts2, s, table;
  data->getSymbol(symbol);
  if (getIndexData(symbol, table, data))
    return;

  QDateTime firstDate;
  getFirstDate(table, firstDate);

  QDateTime lastDate;
  getLastDate(table, lastDate);

  QHash<QString, Bar *> bars;
  QStringList dateList;

  while (1)
  {
    QString sd = firstDate.toString(Qt::ISODate);
    QString ed = lastDate.toString(Qt::ISODate);

    s = "SELECT date,open,high,low,close,volume,oi FROM " + table;
    s.append(" WHERE date >='" + sd + "' AND date <='" + ed + "'");
    s.append(" ORDER BY date DESC");
    s.append(" LIMIT " + QString::number(data->getBarsRequested()));
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::getChart:" << q.lastError().text();
      break;
    }

    while (q.next())
    {
      QDateTime dt = q.value(0).toDateTime();
      lastDate = dt;
      QDateTime sd, ed;
      setStartEndDates(dt, sd, ed, data->getBarLength());
      s = sd.toString(Qt::ISODate) + ed.toString(Qt::ISODate);
      Bar *bar = bars[s];
      if (! bar)
      {
	if (bars.count() == data->getBarsRequested())
	  break;

	dateList.append(s); // save new dateList entry
	bar = new Bar;
        bar->setDate(dt); // save actual date

	bars.insert(s, bar);

        QVariant v = q.value(1);
        if (v.isValid())
	  bar->setOpen(v.toDouble());

        v = q.value(2);
        if (v.isValid())
	  bar->setHigh(v.toDouble());

        v = q.value(3);
        if (v.isValid())
	  bar->setLow(v.toDouble());

        v = q.value(4);
        if (v.isValid())
	  bar->setClose(v.toDouble());

        v = q.value(5);
        if (v.isValid())
	  bar->setVolume(v.toDouble());

        v = q.value(6);
        if (v.isValid())
	  bar->setOI(v.toDouble());

	continue;
      }

      QVariant v = q.value(1);
      if (v.isValid())
        bar->setOpen(v.toDouble());

      v = q.value(2);
      if (v.isValid())
      {
        double h = v.toDouble();
        if (h > bar->getHigh())
          bar->setHigh(h);
      }

      v = q.value(3);
      if (v.isValid())
      {
        double l = v.toDouble();
        if (l < bar->getLow())
          bar->setLow(l);
      }

      v = q.value(5);
      if (v.isValid())
      {
        double v1 = v.toDouble();
        double v2 = bar->getVolume();
        bar->setVolume(v1 + v2);
      }
    }

    if (bars.count() == data->getBarsRequested())
      break;

    if (lastDate == firstDate)
      break;
  }

  // order the bars from most recent to first
  int loop;
  for (loop = 0; loop < dateList.count(); loop++)
    data->prepend(bars[dateList[loop]]);

  data->createDateList();
}

void QuoteDataBase::getFirstDate (QString &table, QDateTime &date)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT min(date) FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getFirstDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void QuoteDataBase::getLastDate (QString &table, QDateTime &date)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT max(date) FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getLastDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void QuoteDataBase::setStartEndDates (QDateTime &date, QDateTime &startDate, QDateTime &endDate,
				      BarData::BarLength barLength)
{
  QString s, s2;
  int tint = 0;
  startDate = date;

  switch (barLength)
  {
    case BarData::Minute1:
      startDate.setTime(QTime(startDate.time().hour(), startDate.time().minute(), 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(60);
      break;
    case BarData::Minute5:
      tint = startDate.time().minute() / 5;
      startDate.setTime(QTime(startDate.time().hour(), tint * 5, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(300);
      break;
    case BarData::Minute10:
      tint = startDate.time().minute() / 10;
      startDate.setTime(QTime(startDate.time().hour(), tint * 10, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(600);
      break;
    case BarData::Minute15:
      tint = startDate.time().minute() / 15;
      startDate.setTime(QTime(startDate.time().hour(), tint * 15, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(900);
      break;
    case BarData::Minute30:
      tint = startDate.time().minute() / 30;
      startDate.setTime(QTime(startDate.time().hour(), tint * 30, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(1800);
      break;
    case BarData::Minute60:
      startDate.setTime(QTime(startDate.time().hour(), 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(3600);
      break;
    case BarData::DailyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addDays(1);
      break;
    case BarData::WeeklyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- startDate.date().dayOfWeek());
      endDate = startDate;
      endDate = endDate.addDays(7);
      break;
    case BarData::MonthlyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- (startDate.date().day() - 1));
      endDate = startDate;
      endDate = endDate.addDays(endDate.date().daysInMonth());
      break;
    default:
      break;
  }
}

int QuoteDataBase::getIndexData (QString &symbol, QString &table, BarData *data)
{
  table.clear();

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT name,tableName FROM symbolIndex WHERE symbol='" + symbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getTableName: " << q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    s = q.value(0).toString();
    data->setName(s);

    table = q.value(1).toString();
  }

  return 0;
}

void QuoteDataBase::setChart (QList<Bar> *bars)
{
  if (! bars->count())
    return;

  Bar bar = bars->at(0);
  QString tsymbol, name, exchange, type;
  bar.getSymbol(tsymbol);
  bar.getName(name);
  bar.getExchange(exchange);
  bar.getType(type);

  QSqlQuery q(QSqlDatabase::database("quotes"));

  transaction();

  QString table;
  BarData data;
  if (getIndexData(tsymbol, table, &data))
    return;

  if (table.isEmpty())
  {
    // create a unique table name from hash algo
    uint hash = qHash(tsymbol);
    table = "T" + QString::number(hash);

    // add new symbol entry into the symbolIndex table
    QString ts = "REPLACE INTO symbolIndex (symbol,name,tableName,exchange,type) VALUES(";
    ts.append("'" + tsymbol + "'");
    ts.append(",'" + name + "'");
    ts.append(",'" + table + "'");
    ts.append(",'" + exchange + "'");
    ts.append(",'" + type + "'");
    ts.append(")");
    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::setChart:create new symbol index record: " << q.lastError().text();
      qDebug() << ts;
      return;
    }

    // new symbol, create new table for it
    ts = "CREATE TABLE IF NOT EXISTS " + table + " (";
    ts.append("date DATETIME PRIMARY KEY UNIQUE");
    ts.append(", open REAL");
    ts.append(", high REAL");
    ts.append(", low REAL");
    ts.append(", close REAL");
    ts.append(", volume INT");
    ts.append(", oi INT");
    ts.append(")");
    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::setChart:create new symbol table: " << q.lastError().text();
      qDebug() << ts;
      return;
    }
  }

  int loop;
  QString tdate;
  for (loop = 0; loop < bars->count(); loop++)
  {
    Bar bar = bars->at(loop);
    bar.getDateTimeString(tdate);

    QString ts = "REPLACE INTO " + table + " VALUES('" + tdate + "'";

    QString k = "Open";
    QString d;
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "High";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "Low";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "Close";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "Volume";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "OI";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0)");
    else
      ts.append("," + d + ")");

    q.exec(ts);
    if (q.lastError().isValid())
      qDebug() << "QuoteDataBase::setChart:save quote in symbol table: " << q.lastError().text();
  }

  commit();
}

