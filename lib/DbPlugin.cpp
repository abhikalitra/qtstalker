/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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


DbPlugin::DbPlugin ()
{
  barLength = BarData::DailyBar;
  barRange = 275;
  db = 0;
  type = Stock;
}

DbPlugin::~DbPlugin ()
{
  if (db)
    db->close(db, 0);
}

int DbPlugin::openChart (QString &d)
{
  if (db)
    close();

  bool flag = FALSE;  
  QDir dir(d);
  if (! dir.exists())
    flag = TRUE;
  
  int rc = db_create(&db, NULL, 0);
  if (rc)
  {
    qDebug("DbPlugin::openChart: %s", db_strerror(rc));
    return TRUE;
  }
  
  rc = db->open(db, NULL, (char *) d.latin1(), NULL, DB_BTREE, DB_CREATE, 0664);
  if (rc)
  {
    qDebug("DbPlugin::openChart: %s", db_strerror(rc));
    return TRUE;
  }

  if (flag)
    setHeaderField(Path, d);

  loadType();

  symbol = d;
  
  return FALSE;
}

void DbPlugin::close ()
{
  if (db)
  {
    db->close(db, 0);
    db = 0;
  }
}

void DbPlugin::getData (QString &k, QString &d)
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  if (db->get(db, NULL, &key, &data, 0) == 0)
    d = (char *) data.data;
  else
    d.truncate(0);
}

void DbPlugin::setData (QString &k, QString &d)
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  data.data = (char *) d.latin1();
  data.size = d.length() + 1;

  db->put(db, NULL, &key, &data, 0);
}

void DbPlugin::deleteData (QString &k)
{
  DBT key;
  memset(&key, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  db->del(db, NULL, &key, 0);
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

void DbPlugin::getChartObjectsList (QStringList &d)
{
  d.clear();
  QString s;
  getHeaderField(CO, s);
  d = QStringList::split(",", s, FALSE);
}

void DbPlugin::getChartObjects (QStringList &d)
{
  d.clear();
  QStringList l;
  getChartObjectsList(l);
  int loop;
  QString s;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    getData(l[loop], s);
    d.append(s);
  }
}

void DbPlugin::setChartObject (QString &d, Setting &set)
{
  QString s;
  QStringList l;
  getChartObjectsList(l);
  if (l.findIndex(d) == -1)
  {
    l.append(d);
    s = l.join(",");
    setHeaderField(CO, s);
  }
  
  set.getString(s);
  setData(d, s);
}

void DbPlugin::deleteChartObject (QString &d)
{
  QString s;
  QStringList l;
  getChartObjectsList(l);
  l.remove(d);
  if (! l.count())
    s = "";
  else
    s = l.join(",");
  setHeaderField(CO, s);

  deleteData(d);
}

void DbPlugin::dump (QString &d, bool f)
{
  QFile outFile(d);
  if (! outFile.open(IO_WriteOnly))
    return;
  QTextStream outStream(&outFile);
  
  DBT key;
  DBT data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cur, 0);

  QString sym;
  getHeaderField(Symbol, sym);

  while (! cur->c_get(cur, &key, &data, DB_NEXT))
  {
    if (f)
    {
      if (key.size != 15)
        continue;
    
      QString s = (char *) key.data;
      Bar bar;
      if (bar.setDate(s))
        continue;
  
      outStream << sym << "," << (char *) key.data << "," << (char *) data.data << "\n";
    }
    else
      outStream << (char *) key.data << "=" << (char *) data.data << "\n";
  }

  cur->c_close(cur);

  outFile.close();
}

void DbPlugin::getFirstBar (Bar &bar)
{
  DBT key;
  DBT data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cur, 0);
  while (! cur->c_get(cur, &key, &data, DB_NEXT))
  {
    if (key.size != 15)
      continue;
    
    QString k = (char *) key.data;
    Bar tbar;
    if (tbar.setDate(k))
      continue;

    QString d = (char *) data.data;
    getBar(k, d, bar);
   
    break;
  }
  cur->c_close(cur);
}

void DbPlugin::getLastBar (Bar &bar)
{
  DBT key;
  DBT data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cur, 0);
  while (! cur->c_get(cur, &key, &data, DB_PREV))
  {
    if (key.size != 15)
      continue;
    
    QString k = (char *) key.data;
    Bar tbar;
    if (tbar.setDate(k))
      continue;

    QString d = (char *) data.data;
    getBar(k, d, bar);
    
    break;
  }
  cur->c_close(cur);
}

void DbPlugin::getBar (QString &k, Bar &bar)
{
  QString d;
  getData(k, d);
  if (d.length())
    getBar(k, d, bar);
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
    Spread spread;
    spread.getHistory(barData, startDate, fs, ss, barRange, barLength);
    barData->createDateList();
    return;
  }
  else
  {
    if (type == Index1)
    {
      QString s;
      getHeaderField(IndexList, s);
      Index index;
      index.getHistory(barData, startDate, s, barRange, barLength);
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
        CC cc;
        cc.getHistory(barData, startDate, fs, (bool) af.toInt(), barRange, barLength);
        barData->createDateList();
        return;
      }
    }
  }
  
  DBT key;
  DBT data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cur, 0);

  s = startDate.toString("yyyyMMddhhmmss");
  key.data = (char *) s.latin1();
  key.size = s.length() + 1;
  int ret = cur->c_get(cur, &key, &data, DB_SET_RANGE);
  if (ret)
  {
    char *err = db_strerror(ret);
    qDebug("%s %s", s.latin1(), err);
  }

  while (! cur->c_get(cur, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
      
    if (key.size != 15)
      continue;
    
    QString k = (char *) key.data;
    Bar bar;
    if (bar.setDate(k))
      continue;

    QString d = (char *) data.data;
    getBar(k, d, bar);
    bar.setTickFlag(barType);
    barData->prepend(bar);
  }
  
  cur->c_close(cur);

  barData->createDateList();
}

void DbPlugin::setHeaderField (int i, QString &d)
{
  QString k;
  if (! d.length())
    d = "";
  getHeaderKey(i, k);
  if (k.length())  
    setData(k, d);
}

void DbPlugin::getHeaderField (int i, QString &d)
{
  d.truncate(0);
  QString k;
  getHeaderKey(i, k);
  if (k.length())
    getData(k, d);
}

void DbPlugin::getHeaderKey (int i, QString &k)
{
  switch (i)
  {
    case BarType:
      k = "BarType";
      break;
    case Plugin:
      k = "Plugin";
      break;
    case Symbol:
      k = "Symbol";
      break;
    case Type:
      k = "Type";
      break;
    case Title:
      k = "Title";
      break;
    case Path:
      k = "Path";
      break;
    case CO:
      k = "COList";
      break;
    case LocalIndicators:
      k = "LocalIndicators";
      break;
    case QuotePlugin:
      k = "QuotePlugin";
      break;
    case SpreadFirstSymbol:
      k = "SpreadFirstSymbol";
      break;
    case SpreadSecondSymbol:
      k = "SpreadSecondSymbol";
      break;
    case IndexList:
      k = "IndexList";
      break;
    case CCAdjustment:
      k = "CCAdjustment";
      break;
    case FuturesType:
      k = "FuturesType";
      break;
    case FuturesMonth:
      k = "FuturesMonth";
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
  DBT key;
  DBT data;
  DBC *cur;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  QString s;
  getHeaderField(BarType, s);
  int barType = s.toInt();

  db->cursor(db, NULL, &cur, 0);

  while (! cur->c_get(cur, &key, &data, DB_PREV))
  {
    if (key.size != 15)
      continue;
    
    QString k = (char *) key.data;
    Bar bar;
    if (bar.setDate(k))
      continue;

    QString d = (char *) data.data;
    getBar(k, d, bar);
    bar.setTickFlag(barType);
    bars->prependRaw(bar);
  }

  cur->c_close(cur);
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

void DbPlugin::getBar (QString &k, QString &d, Bar &bar)
{
  QStringList l = QStringList::split(",", d, FALSE);

  bar.setDate(k);
  bar.setOpen(l[0].toDouble());
  bar.setHigh(l[1].toDouble());
  bar.setLow(l[2].toDouble());
  bar.setClose(l[3].toDouble());

  switch (type)
  {
    case Stock:
      bar.setVolume(l[4].toDouble());
      break;
    case Futures:
    case CC1:
      bar.setVolume(l[4].toDouble());
      bar.setOI(l[5].toInt());
      break;
    default:
      break;
  }
}

void DbPlugin::setBar (Bar &bar)
{
  QString k, d;

  bar.getDateTimeString(FALSE, k);

  switch (type)
  {
    case Stock:
      d = QString::number(bar.getOpen()) + "," + QString::number(bar.getHigh()) + "," +
          QString::number(bar.getLow()) + "," + QString::number(bar.getClose()) + "," +
          QString::number(bar.getVolume(), 'f', 0);
      break;
    case Futures:
      d = QString::number(bar.getOpen()) + "," + QString::number(bar.getHigh()) + "," +
          QString::number(bar.getLow()) + "," + QString::number(bar.getClose()) + "," +
          QString::number(bar.getVolume(), 'f', 0) + "," + QString::number(bar.getOI());
      break;
    default:
      break;
  }

  setData(k, d);
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
      setHeaderField(DbPlugin::Plugin, s);  
      setHeaderField(DbPlugin::Title, sym);
      return;
    }
  }

  if (type == Index1)
  {
    QString pat, sym;
    Spread dialog;
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

      setHeaderField(DbPlugin::Symbol, symbol);
      QString s = "Index";
      setHeaderField(DbPlugin::Type, s);  
      setHeaderField(DbPlugin::Plugin, s);  
      setHeaderField(DbPlugin::Title, symbol);
      return;
    }
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
      setHeaderField(DbPlugin::Plugin, s);  
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

