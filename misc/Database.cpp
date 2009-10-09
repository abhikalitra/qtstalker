/*
 *  CSV 
 *
 *  Copyright (C) 2009 Stefan S. Stratigakos
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

#include "Database.h"

#include <QtDebug>
#include <QDir>


Database::Database ()
{
  QDir dir(QDir::homePath());
  QString home = dir.absolutePath() + "/.CSV";
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home))
      qDebug() << "CSV::Unable to create" << home;
  }

  QString s = home + "/data.sqlite";
  QSqlDatabase db = QSqlDatabase::database("data");
  if (! db.isValid())
  {
    db = QSqlDatabase::addDatabase("QSQLITE", "data");
    db.setHostName("localhost");
    db.setDatabaseName(s);
    db.setUserName("CSV");
    db.setPassword("CSV");
    if (! db.open())
      qDebug() << "CSV::Database: data db open failed";
  }
  
  // create the config table
  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS config (key INT PRIMARY KEY, setting VARCHAR(50))";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSV::Database:createConfigTable: " << q.lastError().text();

  // create the import rules table
  s = "CREATE TABLE IF NOT EXISTS importRules (";
  s.append(" name TEXT PRIMARY KEY");
  s.append(", format TEXT");
  s.append(", delimeter TEXT");
  s.append(", fileNameSymbol BOOL");
  s.append(", dateFormat TEXT");
  s.append(", interval INT");
  s.append(", fileList TEXT");
  s.append(", timeFormat TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSV::Database:createImportRulesTable: " << q.lastError().text();
}

void Database::setConfig (QString &k, QString &d)
{
  QSqlDatabase db = QSqlDatabase::database("data");
  if (! db.isValid())
  {
    qDebug() << "Database::setConfig:" << db.lastError().text();
    return;
  }

  QSqlQuery q(db);
  QString s = "INSERT OR REPLACE INTO config (key, setting) VALUES (" + k + ",'" + d + "')";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Database::setConfig: " << q.lastError().text();
    return;
  }
}

void Database::getConfig (QString &k, QString &d)
{
  d.clear();

  QSqlDatabase db = QSqlDatabase::database("data");
  if (! db.isValid())
  {
    qDebug() << "Database::getConfig:" << db.lastError().text();
    return;
  }

  QSqlQuery q(db);
  QString s = "SELECT setting FROM config WHERE key=" + k;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Database::getConfig: " << q.lastError().text();
    return;
  }

  if (q.next())
    d = q.value(0).toString();
}

//************************************************************
//****************** RULE FUNCTIONS **************************
//************************************************************

void Database::getRules (QStringList &l)
{
  l.clear();

  QSqlDatabase db = QSqlDatabase::database("data");
  if (! db.isValid())
  {
    qDebug() << "Database::getRules:" << db.lastError().text();
    return;
  }

  QSqlQuery q(db);
  QString s = "SELECT name FROM importRules";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Database::getRules: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void Database::deleteRule (QString &name)
{
  QSqlDatabase db = QSqlDatabase::database("data");
  if (! db.isValid())
  {
    qDebug() << "Database::deleteRule:" << db.lastError().text();
    return;
  }

  QSqlQuery q(db);
  QString s = "DELETE FROM importRules WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "Database::deleteRule: " << q.lastError().text();
}

void Database::loadRule (CSVRule &rule)
{
  QSqlDatabase db = QSqlDatabase::database("data");
  if (! db.isValid())
  {
    qDebug() << "Database::loadRule:" << db.lastError().text();
    return;
  }

  QString ts;
  rule.getName(ts);

  QSqlQuery q(db);
  QString s = "SELECT * FROM importRules WHERE name='" + ts + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Database::loadRule: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    s = q.value(1).toString();
    rule.setFormat(s);
    
    s = q.value(2).toString();
    rule.setDelimiter(s);

    rule.setFileNameSymbol(q.value(3).toBool());

    s = q.value(4).toString();
    rule.setDateFormat(s);

    s = q.value(5).toString();
    rule.setInterval(s);

    s = q.value(6).toString();
    QStringList l = s.split("|");
    rule.setFileList(l);

    s = q.value(7).toString();
    rule.setTimeFormat(s);
  }
}

void Database::saveRule (CSVRule &rule)
{
  QSqlDatabase db = QSqlDatabase::database("data");
  if (! db.isValid())
  {
    qDebug() << "Database::saveRule:" << db.lastError().text();
    return;
  }

  QSqlQuery q(db);
  QString s = "INSERT OR REPLACE INTO importRules VALUES (";

  QString s2;
  rule.getName(s2);
  s.append("'" + s2 + "'");

  rule.getFormat(s2);
  s.append(",'" + s2 + "'");

  rule.getDelimiter(s2);
  s.append(",'" + s2 + "'");
  
  s2 = QString::number(rule.getFileNameSymbol());
  s.append("," + s2);

  rule.getDateFormat(s2);
  s.append(",'" + s2 + "'");

  rule.getInterval(s2);
  s.append("," + s2);

  QStringList l;
  rule.getFileList(l);
  s.append(",'" + l.join("|") + "'");

  rule.getTimeFormat(s2);
  s.append(",'" + s2 + "'");

  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Database::saveRule: " << q.lastError().text();
    return;
  }
}

