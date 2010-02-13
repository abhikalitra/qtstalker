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

void QuoteDataBase::init (QString &plugin, QString &dbFile, QString &host, QString &user, QString &pass)
{
  QSqlDatabase db = QSqlDatabase::addDatabase(plugin, "quotes");
  db.setHostName(host);
  db.setDatabaseName(dbFile);
  db.setUserName(user);
  db.setPassword(pass);
  if (! db.open())
    qDebug() << "QuoteDataBase::init: quotes db open failed";
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

void QuoteDataBase::getAllChartsList (QString &sql, QStringList &l)
{
  l.clear();
//  config.getData(Config::DbAllSymbols, s);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  q.exec(sql);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getAllChartsList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void QuoteDataBase::getSearchList (QString &sql, QString &pat, QStringList &l)
{
  l.clear();
//  config.getData(Config::DbSearchSymbols, s);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = sql;
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

void QuoteDataBase::getChart (QString &sql, QString &sqlFirstDate, QString &sqlLastDate, BarData *data)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));

  QString symbol, ts, ts2, s;
  data->getSymbol(symbol);

//  Config config;
//  config.getData(Config::DbGetSymbol, sql);

  QDateTime firstDate;
  getFirstDate(sqlFirstDate, firstDate, symbol);

  QDateTime lastDate;
  getLastDate(sqlLastDate, lastDate, symbol);

  QHash<QString, Bar *> bars;
  QStringList dateList;

  while (1)
  {
    QString sd = firstDate.toString("yyyy-MM-dd HH:mm:ss");
    sd.prepend("'"); // we need to surround date strings in ' chars here because of TEXT storage conflicts.
    sd.append("'");

    QString ed = lastDate.toString("yyyy-MM-dd HH:mm:ss");
    ed.prepend("'"); // we need to surround date strings in ' chars here because of TEXT storage conflicts.
    ed.append("'");

    s = sql;
    s.replace("$symbol", symbol, Qt::CaseSensitive);
    s.replace("$sd", sd, Qt::CaseSensitive);
    s.replace("$ed", ed, Qt::CaseSensitive);
    s.replace("$records", QString::number(data->getBarsRequested()), Qt::CaseSensitive);
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
      s = sd.toString("yyyyMMddHHmmss") + ed.toString("yyyyMMddHHmmss");
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

void QuoteDataBase::getFirstDate (QString &sql, QDateTime &date, QString &symbol)
{
//  config.getData(Config::DbFirstDate, s);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = sql;
  s.replace("$symbol", symbol, Qt::CaseSensitive);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getFirstDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void QuoteDataBase::getLastDate (QString &sql, QDateTime &date, QString &symbol)
{
//  config.getData(Config::DbLastDate, s);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = sql;
  s.replace("$symbol", symbol, Qt::CaseSensitive);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getLastDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void QuoteDataBase::setStartEndDates (QDateTime &date, QDateTime &startDate, QDateTime &endDate, BarData::BarLength barLength)
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

