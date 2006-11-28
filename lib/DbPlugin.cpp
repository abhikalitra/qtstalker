/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#include "DbPlugin.h"
#include "StocksDialog.h"
#include "FuturesDialog.h"
#include "IndicatorPlugin.h"
#include "Spread.h"
#include "IndexDialog.h"
#include "Index.h"
#include "CC.h"
#include "FuturesData.h"
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qsqlquery.h>


DbPlugin::DbPlugin ()
{
  barLength = BarData::DailyBar;
  barRange = 275;
  db = 0;
  type = Stock;
}

DbPlugin::~DbPlugin ()
{
  close();
}

int DbPlugin::openChart (QString &d)
{
  close();

  bool flag = FALSE;
  QDir dir;
  if (! dir.exists(d, TRUE))
    flag = TRUE;

  if (open(d))
    return TRUE;

  if (flag)
    createTables();

  loadType();
  symbol = d;
  return FALSE;
}

void DbPlugin::close ()
{
  if (db)
  {
    db->close();
    QSqlDatabase::removeDatabase(db);
    db = 0;
  }
}

void DbPlugin::setBarLength (BarData::BarLength d)
{
  barLength = d;
}

void DbPlugin::setBarRange (int d)
{
  barRange = d;
}

void DbPlugin::getHelpFile (QString &d)
{
  d = helpFile;
}

void DbPlugin::getChartObjects (QStringList &d)
{
  d.clear();
  QString s = "SELECT data FROM co";
  QSqlQuery q(db);
  q.exec(s);
  if (! q.isActive())
    return;
    
  while (q.next())
    d.append(q.value(0).toString());
}

void DbPlugin::setChartObject (QString &d, Setting &set)
{
  QString s;
  set.getString(s);
  QString k = "REPLACE INTO co (id, data) VALUES (" + d + ",'" + s + "')";
  QSqlQuery q(db);
  if (! q.exec(k))
    qDebug("DbPlugin::setChartObject: %s", db->lastError().text().latin1());
}

void DbPlugin::deleteChartObject (QString &d)
{
  QString s = "DELETE FROM co WHERE id=" + d;
  QSqlQuery q(db);
  if (! q.exec(s))
    qDebug("DbPlugin::deleteChartObject: %s", db->lastError().text().latin1());
}

void DbPlugin::deleteAllChartObjects ()
{
  QString s = "DELETE FROM co";
  QSqlQuery q(db);
  if (! q.exec(s))
    qDebug("DbPlugin::deleteAllChartObjects: %s", db->lastError().text().latin1());
}

void DbPlugin::dump (QString &d, bool f)
{
  QFile outFile(d);
  if (! outFile.open(IO_WriteOnly))
    return;
  QTextStream outStream(&outFile);
  
  QString sym;
  getHeaderField(Symbol, sym);

  // dump quotes
  QSqlQuery q(db);
  q.exec("SELECT date, open, high, low, close, volume, oi FROM data ORDER BY date ASC");
  if (q.isActive())
  {
    while (q.next())
    {
      outStream << sym << "," << q.value(0).toString() << "," << q.value(1).toString() << ",";
      outStream << q.value(1).toString() << "," << q.value(2).toString() << "," << q.value(3).toString() << ",";
      outStream << q.value(4).toString() << "," << q.value(5).toString() << "\n";
    }
  }

  if (f)
  {
    // we just want quotes
    outFile.close();
    return;
  }

  // dump everything

  // dump header
  QSqlQuery q2(db);
  q2.exec("SELECT type,symbol,title,path,localindicators,quoteplugin,spreadfirstsymbol,"
          "spreadsecondsymbol,indexlist,ccadjustment,futurestype,futuresmonth, bartype, fundamentals FROM header");
  if (q2.isActive())
  {
    q2.first();
    outStream << "HEADER=" << q2.value(0).toString() << "," << q2.value(1).toString() << "," << q2.value(2).toString() << ",";
    outStream << q2.value(3).toString() << "," << q2.value(4).toString() << "," << q2.value(5).toString() << ",";
    outStream << q2.value(6).toString() << "," << q2.value(7).toString() << "," << q2.value(8).toString() << ",";
    outStream << q2.value(9).toString() << "," << q2.value(10).toString() << "," << q2.value(11).toString() << "\n";
  }
  
  // dump co
  QSqlQuery q3(db);
  q3.exec("SELECT id, data FROM co ORDER BY id ASC");
  if (q3.isActive())
  {
    while (q3.next())
      outStream << "CO=ID=" << q3.value(0).toString() << "," << q3.value(1).toString() << "\n";
  }
  
  outFile.close();
}

void DbPlugin::getFirstBar (Bar &bar)
{
  QSqlQuery q(db);
  q.exec("SELECT date, open, high, low, close, volume, oi FROM data ORDER BY date ASC LIMIT 1");
  if (! q.isActive())
    return;

  q.next();
  if (! q.isValid())
    return;
  getBar(q, bar);
}

void DbPlugin::getLastBar (Bar &bar)
{
  QSqlQuery q(db);
  q.exec("SELECT date, open, high, low, close, volume, oi FROM data ORDER BY date DESC LIMIT 1");
  if (! q.isActive())
    return;

  q.next();
  if (! q.isValid())
    return;
  getBar(q, bar);
}

void DbPlugin::getPrevBar (QDateTime &startDate, Bar &bar)
{
  QString s = startDate.toString("yyyyMMddhhmmss");
  QSqlQuery q(db);
  q.exec("SELECT date, open, high, low, close, volume, oi FROM data WHERE date < " + s + " ORDER BY date DESC LIMIT 1");
  if (! q.isActive())
    return;

  q.next();
  if (! q.isValid())
    return;
  getBar(q, bar);
}

void DbPlugin::getNextBar (QDateTime &startDate, Bar &bar)
{
  QString s = startDate.toString("yyyyMMddhhmmss");
  QSqlQuery q(db);
  q.exec("SELECT date, open, high, low, close, volume, oi FROM data WHERE date > " + s + " ORDER BY date ASC LIMIT 1");
  if (! q.isActive())
    return;

  q.next();
  if (! q.isValid())
    return;
  getBar(q, bar);
}

void DbPlugin::getSearchBar (QDateTime &startDate, Bar &bar)
{
  QString s = startDate.toString("yyyyMMddhhmmss");
  QSqlQuery q(db);
  q.exec("SELECT date, open, high, low, close, volume, oi FROM data WHERE date = " + s + " LIMIT 1");
  if (! q.isActive())
    return;

  q.next();
  if (! q.isValid())
    return;
  getBar(q, bar);
}

void DbPlugin::getBar (QString &k, Bar &bar)
{
  QSqlQuery q(db);
  q.exec("SELECT date, open, high, low, close, volume, oi FROM data WHERE date = " + k + " LIMIT 1");
  if (! q.isActive())
    return;

  q.next();
  if (! q.isValid())
    return;
  getBar(q, bar);
}

void DbPlugin::getHistory (BarData *barData, QDateTime &startDate)
{
  QString s;
  getHeaderField(BarType, s);
  int barType = s.toInt();
  barData->setBarType((BarData::BarType) barType);  
  barData->setBarLength(barLength);

  if (type == Spread1)
  {
    QString fs, ss;
    getHeaderField(SpreadFirstSymbol, fs);
    getHeaderField(SpreadSecondSymbol, ss);
    close(); // we have to close the current chart because sqlite can only open one at a time or we'll segfault
    Spread spread;
    spread.getHistory(barData, startDate, fs, ss, barRange, barLength);
    open(symbol); // open her back up
    barData->createDateList();
    return;
  }
  else
  {
    if (type == Index1)
    {
      QString s;
      getHeaderField(IndexList, s);
      close();
      Index index;
      index.getHistory(barData, startDate, s, barRange, barLength);
      open(symbol);
      barData->createDateList();
      return;
    }
    else
    {
      if (type == CC1)
      {
        QString fs, af;
        getHeaderField(Symbol, fs);
        getHeaderField(CCAdjustment, af);
        close();
        CC cc;
        cc.getHistory(barData, startDate, fs, (bool) af.toInt(), barRange, barLength);
        open(symbol);
        barData->createDateList();
        return;
      }
    }
  }
  
  QDateTime dt = startDate;
  while (1)
  {
    s = dt.toString("yyyyMMddhhmmss");
    QSqlQuery q(db);
    QString qs = "SELECT date,open,high,low,close,volume,oi FROM data WHERE date < " + s + " ORDER BY date DESC LIMIT " + 
                 QString::number(barRange);
    q.exec(qs);
    if (! q.isActive())
      break;

    while (q.next())
    {
      Bar bar;
      getBar(q, bar);
      bar.setTickFlag(barType);
      barData->prepend(bar);
      if (barData->count() >= barRange)
        break;
    }

    if (barData->count() >= barRange)
      break;

    q.last();
    if (! q.isValid())
      break;
    Bar tbar;
    s = q.value(0).toString();
    tbar.setDate(s);
    tbar.getDate(dt);
  }

  barData->createDateList();
}

void DbPlugin::setHeaderField (int i, QString &d)
{
  QString k;
  if (! d.length())
    d = "";
  getHeaderKey(i, k);
  if (! k.length())
    return;

  QString s = "UPDATE header SET " + k + "='" + d + "' WHERE id=1";
  QSqlQuery q(db);
  if (! q.exec(s))
  {
    s = db->lastError().text();
    if (s.length())
      qDebug("DbPlugin::setHeaderField: %s", s.latin1());
  } 
}

void DbPlugin::getHeaderField (int i, QString &d)
{
  d.truncate(0);
  QString k;
  getHeaderKey(i, k);
  if (! k.length())
    return;

  QString s = "SELECT " + k + " FROM header";
  QSqlQuery q(db);
  if (! q.exec(s))
  {
    s = db->lastError().text();
    if (s.length())
    {
      qDebug("DbPlugin::getHeaderField: %s", s.latin1());
      return;
    }
  }

  q.next();
  if (! q.isValid())
    return;
  d = q.value(0).toString();
}

void DbPlugin::getHeaderKey (int i, QString &k)
{
  switch (i)
  {
    case BarType:
      k = "bartype";
      break;
    case Fundamentals:
      k = "fundamentals";
      break;
    case Symbol:
      k = "symbol";
      break;
    case Type:
      k = "type";
      break;
    case Title:
      k = "title";
      break;
    case Path:
      k = "path";
      break;
    case LocalIndicators:
      k = "localindicators";
      break;
    case QuotePlugin:
      k = "quoteplugin";
      break;
    case SpreadFirstSymbol:
      k = "spreadfirstsymbol";
      break;
    case SpreadSecondSymbol:
      k = "spreadsecondsymbol";
      break;
    case IndexList:
      k = "indexlist";
      break;
    case CCAdjustment:
      k = "ccadjustment";
      break;
    case FuturesType:
      k = "futurestype";
      break;
    case FuturesMonth:
      k = "futuresmonth";
      break;
    default:
      k.truncate(0);
      break;
  }
}

void DbPlugin::deleteIndicator (QString &d)
{
  // d = the path to the indicator

  QString s;
  getHeaderField(LocalIndicators, s);
  QStringList l = QStringList::split(",", s, FALSE);
  l.remove(d);
  if (l.count())
  {
    s = l.join(",");
    setHeaderField(LocalIndicators, s);
  }
  else
  {
    s = "";
    setHeaderField(LocalIndicators, s);
  }
}

void DbPlugin::addIndicator (QString &d)
{
  // d = the path to the indicator

  QString s;
  getHeaderField(LocalIndicators, s);
  QStringList l = QStringList::split(",", s, FALSE);
  int i = l.findIndex(d);
  if (i != -1)
    return;
  
  l.append(d);
  s = l.join(",");
  setHeaderField(LocalIndicators, s);
}

void DbPlugin::getAllBars (BarData *bars)
{
  QSqlQuery q(db);
  q.exec("SELECT date, open, high, low, close, volume, oi FROM data ORDER BY date ASC");
  if (! q.isActive())
    return;
    
  while (q.next())
  {
    Bar bar;
    getBar(q, bar);
    bars->appendRaw(bar);
  }
}

void DbPlugin::loadType ()
{
  QString s;
  getHeaderField(Type, s);
  type = getType(s);
}

DbPlugin::DbType DbPlugin::getType (QString &d)
{
  DbPlugin::DbType t = Stock;

  while (1)
  {
    if (! d.compare("Stock"))
    {
      t = Stock;
      break;
    }

    if (! d.compare("Futures"))
    {
      t = Futures;
      break;
    }

    if (! d.compare("Spread"))
    {
      t = Spread1;
      break;
    }

    if (! d.compare("Index"))
    {
      t = Index1;
      break;
    }

    if (! d.compare("CC"))
    {
      t = CC1;
      break;
    }

    break;
  }

  return t;
}

void DbPlugin::getBar (QSqlQuery &q, Bar &bar)
{
  QString s = q.value(0).toString();
  bar.setDate(s);
  bar.setOpen(q.value(1).toDouble());
  bar.setHigh(q.value(2).toDouble());
  bar.setLow(q.value(3).toDouble());
  bar.setClose(q.value(4).toDouble());
  bar.setVolume(q.value(5).toDouble());
  bar.setOI(q.value(6).toInt());
}

void DbPlugin::setBar (Bar &bar)
{
  QString k, d;

  bar.getDateTimeString(FALSE, k);

  d = "REPLACE INTO data (date, open, high, low, close, volume, oi) VALUES (";
  d.append(k + "," + QString::number(bar.getOpen()) + "," + QString::number(bar.getHigh()) + ",");
  d.append(QString::number(bar.getLow()) + "," + QString::number(bar.getClose()) + ",");
  d.append(QString::number(bar.getVolume(), 'f', 0) + "," + QString::number(bar.getOI()) + ")");

  QSqlQuery q(db);
  if (! q.exec(d))
    qDebug("DbPlugin::setBar: %s", db->lastError().text().latin1());
}

void DbPlugin::createNew (QString &d)
{
  DbPlugin::DbType t = getType(d);
  createNew(t);
}

void DbPlugin::createNew (DbPlugin::DbType d)
{
  type = d;

  if (type == Stock)
  {
    QString s = "Stock";
    setHeaderField(DbPlugin::Type, s);

    QFileInfo fi(symbol);
    s = fi.fileName();
    setHeaderField(DbPlugin::Title, s);
    setHeaderField(DbPlugin::Symbol, s);

    return;
  }

  if (type == Futures)
  {
    FuturesData fd;
    if (fd.setSymbolPath(symbol))
      return;

    QString s = "Futures";
    setHeaderField(DbPlugin::Type, s);  

    fd.getName(s);
    setHeaderField(DbPlugin::Title, s);

    QFileInfo fi(symbol);
    s = fi.fileName();
    setHeaderField(DbPlugin::Symbol, s);
      
    fd.getSymbol(s);
    setHeaderField(DbPlugin::FuturesType, s);
      
    s = symbol.right(1);
    setHeaderField(DbPlugin::FuturesMonth, s);

    return;
  }

  if (type == Spread1)
  {
    QString pat, sym;
    Spread spread;
    bool rc = spread.createNew(pat, sym);
    if (rc)
    {
      if (openChart(pat))
      {
        QMessageBox::information(0,
                                 QObject::tr("Qtstalker: Disk Error"),
                                 QObject::tr("Cannot create chart."));
        close();
        return;
      }

      setHeaderField(DbPlugin::Symbol, sym);
      QString s = "Spread";
      setHeaderField(DbPlugin::Type, s);  
      setHeaderField(DbPlugin::Title, sym);

      type = Spread1;
      dbPrefDialog();
      return;
    }

    return;
  }

  if (type == Index1)
  {
    QString pat, sym;
    IndexDialog dialog;
    bool rc = dialog.createNew(pat, sym);
    if (rc)
    {
      if (openChart(pat))
      {
        close();
        QMessageBox::information(0,
                                 QObject::tr("Qtstalker: Disk Error"),
                                 QObject::tr("Cannot create Index chart."));
        return;
      }

      setHeaderField(DbPlugin::Symbol, sym);
      QString s = "Index";
      setHeaderField(DbPlugin::Type, s);  
      setHeaderField(DbPlugin::Title, sym);

      type = Index1;
      dbPrefDialog();
      return;
    }

    return;
  }

  if (type == CC1)
  {
    QString pat, sym, adj;
    CC cc;
    bool rc = cc.createNew(pat, sym, adj);
    if (rc)
    {
      if (openChart(pat))
      {
        close();
        QMessageBox::information(0,
                                 QObject::tr("Qtstalker: Disk Error"),
                                 QObject::tr("Cannot create CC chart."));
        return;
      }

      setHeaderField(DbPlugin::Symbol, sym);
      QString s = "CC";
      setHeaderField(DbPlugin::Type, s);  
      s = sym + " - Continuous Adjusted";
      setHeaderField(DbPlugin::Title, s);
      setHeaderField(CCAdjustment, adj);
    }
  }
}

void DbPlugin::dbPrefDialog ()
{
  if (type == Stock)
  {
    StocksDialog *dialog = new StocksDialog(helpFile, this);
    dialog->exec();
    delete dialog;
    return;
  }

  if (type == Futures)
  {
    FuturesDialog *dialog = new FuturesDialog(helpFile, this);
    dialog->exec();
    delete dialog;
    return;
  }

  if (type == Spread1)
  {
    QString fs, ss;
    getHeaderField(SpreadFirstSymbol, fs);
    getHeaderField(SpreadSecondSymbol, ss);
    Spread dialog;
    bool rc = dialog.prefDialog(fs, ss);
    if (rc)
    {
      setHeaderField(SpreadFirstSymbol, fs);
      setHeaderField(SpreadSecondSymbol, ss);
    }
    return;
  }

  if (type == Index1)
  {
    QString nam, il;
    getHeaderField(Title, nam);
    getHeaderField(IndexList, il);
    IndexDialog *dialog = new IndexDialog(nam, il);
    int rc = dialog->exec();
    if (rc == QDialog::Accepted)
    {
      dialog->getName(nam);
      setHeaderField(Title, nam);
      dialog->getList(il);
      setHeaderField(IndexList, il);
    }
    delete dialog;
    return;
  }

  if (type == CC1)
  {
    QString s;
    getHeaderField(CCAdjustment, s);
    CC cc;
    bool rc = cc.prefDialog(s);
    if (rc)
      setHeaderField(CCAdjustment, s);  
  }
}

void DbPlugin::getSymbol (QString &d)
{
  d = symbol;
}

void DbPlugin::deleteBar (QString &d)
{
  QString s = "DELETE FROM data WHERE date=" + d;
  QSqlQuery q(db);
  if (! q.exec(s))
    qDebug("DbPlugin::deleteBar: %s", db->lastError().text().latin1());
}

void DbPlugin::setData (QString &d)
{
  QSqlQuery q(db);
  if (! q.exec(d))
    qDebug("DbPlugin::setData: %s", db->lastError().text().latin1());
}

void DbPlugin::createTables ()
{
  QSqlQuery q(db);

  // ceate the data table for quote storage
  if (! q.exec("create table data (date numeric(14,0) primary key, open numeric(15,4), high numeric(15,4), "
               "low numeric(15,4), close numeric(15,4), volume numeric(15,0), oi numeric(15,0))"))
  {
    qDebug("DbPlugin::openChart:create data table: %s", db->lastError().text().latin1());
    return;
  }

  // create header info table
  if (! q.exec("create table header (id integer primary key, type varchar, symbol varchar, title varchar, "
               "path varchar, localindicators varchar, quoteplugin varchar, spreadfirstsymbol varchar, "
               "spreadsecondsymbol varchar, indexlist varchar, ccadjustment varchar, futurestype varchar, "
               "futuresmonth varchar, bartype varchar, fundamentals varchar)"))
  {
    qDebug("DbPlugin::openChart:create header table: %s", db->lastError().text().latin1());
    return;
  }

  // create the header record
  if (! q.exec("INSERT INTO header (title) VALUES (' ')"))
  {
    qDebug("DbPlugin::openChart:insert blank record %s", db->lastError().text().latin1());
    return;
  }

  // create co table
  if (! q.exec("create table co (id integer, data varchar)"))
  {
    qDebug("DbPlugin::openChart:create co table: %s", db->lastError().text().latin1());
    return;
  }
}

bool DbPlugin::open (QString &d)
{
  db = QSqlDatabase::addDatabase("QSQLITE");
  db->setDatabaseName(d);
  db->setUserName("qtstalker");
  db->setPassword("qtstalker");
  db->setHostName("localhost");
  if (! db->open())
  {
    qDebug("DbPlugin::open: %s", db->lastError().text().latin1());
    return TRUE;
  }
  else
    return FALSE;
}
