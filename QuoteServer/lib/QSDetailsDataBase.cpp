/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#include "QSDetailsDataBase.h"
#include "QSLog.h"

#include <QDebug>
#include <QtSql>

QSDetailsDataBase::QSDetailsDataBase ()
{
}

int QSDetailsDataBase::remove (QSDetail &detail)
{
  // remove records
  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));
  
  QString s = "DELETE FROM detailsIndex";
  s.append(" WHERE tableName='" + detail.table() + "'");
  q.exec(s);
  if (q.lastError().isValid())
    log.message(QSLog::Error, QString(" QSDetailsDataBase::remove: " + q.lastError().text()));

  return 0;
}

int QSDetailsDataBase::details (QSDetail &detail)
{
  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));

  QString ts;
  detail.keyString(ts);
  QString s = "SELECT " + ts + " FROM detailsIndex";
  s.append(" WHERE tableName='" + detail.table() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSDetailsDataBase::details: " + q.lastError().text()));
    return 1;
  }

  if (q.next())
  {
    int loop = 0;
    for (; loop < detail.keyCount(); loop++)
    {
      QString s = q.value(loop).toString();
      detail.addData(s);
    }
  }

  return 0;
}

int QSDetailsDataBase::setDetails (QSDetail &detail)
{
  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));

  // first check if record exists so we know to do an update or insert
  QString ks;
  detail.keyString(ks);
  QString s = "SELECT " + ks + " FROM detailsIndex WHERE tableName='" + detail.table() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << s;
    log.message(QSLog::Error, QString(" QSDetailsDataBase:setDetails " + q.lastError().text()));
    return 1;
  }

  if (q.next()) // record exists, use update
  {
    s = "UPDATE detailsIndex SET ";

    QStringList kl = detail.keyList();
    QStringList dl = detail.dataList();
    QStringList tl;
    int loop = 0;
    for (; loop < kl.count(); loop++)
    {
      if (detail.isNumber(kl[loop]))
        tl.append(kl.at(loop) + "=" + dl.at(loop));
      else
        tl.append(kl.at(loop) + "='" + dl.at(loop) + "'");
    }
    s.append(tl.join(","));
    
    s.append(" WHERE tableName='" + detail.table() + "'");
  }
  else // new record, use insert
  {
    s = "INSERT INTO detailsIndex (tableName," + ks + ") VALUES(";
    s.append("'" + detail.table() + "',");

    QStringList kl = detail.keyList();
    QStringList dl = detail.dataList();
    QStringList tl;
    int loop = 0;
    for (; loop < dl.count(); loop++)
    {
      if (detail.isNumber(kl[loop]))
        tl.append(dl.at(loop));
      else
        tl.append("'" + dl.at(loop) + "'");
    }
    s.append(tl.join(","));
    
    s.append(")");
  }
    
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << s;
    log.message(QSLog::Error, QString(" QSDetailsDataBase:setDetails " + q.lastError().text()));
    return 1;
  }

  return 0;
}

