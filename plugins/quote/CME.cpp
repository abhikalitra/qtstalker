/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "CME.h"
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qstringlist.h>
#include <stdlib.h>

CME::CME ()
{
  connect(&op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(error(QNetworkOperation *)));
  connect(&op2, SIGNAL(finished(QNetworkOperation *)), this, SLOT(error2(QNetworkOperation *)));

  pluginName = "CME";
  version = 0.2;
  
  QStringList l;
  l.append(tr("Today"));
  l.append(tr("History"));
  set(tr("Quote Type"), tr("Today"), Setting::List);
  setList(tr("Quote Type"), l);

  l.clear();
  l.append("FC");
  l.append("LB");
  l.append("LC");
  l.append("LN");
  l.append("PB");
  l.append("AD");
  l.append("NB");
  l.append("CD");
  l.append("EC");
  l.append("JY");
  l.append("SF");
  l.append("ES");
  l.append("GI");
  l.append("ND");
  l.append("NQ");
  l.append("SP");
  l.append("ED");
  set(tr("Symbol"), tr("FC"), Setting::List);
  setList(tr("Symbol"), l);

  about = "Downloads daily settlement quotes from CME\n";
  about.append("and imports it directly into qtstalker.");
}

CME::~CME ()
{
}

void CME::download ()
{
  fileCount = 0;

  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker");

  qInitNetworkProtocols();
  
  QString s = getData(tr("Quote Type"));
  if (! s.compare(tr("History")))
  {
    QString symbol = getData(tr("Symbol"));
    symbol = symbol.lower();

    dir.setPath(file);
    symbolFile = file;
    symbolFile.append("/");
    symbolFile.append(symbol);
    symbolFile.append("ytd.zip");
    dir.remove(symbolFile, TRUE);

    s = symbolFile;
    s.truncate(s.length() - 3);
    s.append("eod");
    dir.remove(s, TRUE);

    s = "ftp://ftp.cme.com//pub/hist_eod/";
    s.append(symbol);
    s.append("ytd.zip");

    op2.copy(s, symbolFile, FALSE, FALSE);
  }
  else
  {
    dir.setPath(file);
    QString s = file;
    s.append("/stlags");
    dir.remove(s, TRUE);

    s = file;
    s.append("/stlcur");
    dir.remove(s, TRUE);

    s = file;
    s.append("/stleqt");
    dir.remove(s, TRUE);

    s = file;
    s.append("/stlint");
    dir.remove(s, TRUE);

    QStringList l;
    l.append("ftp://ftp.cme.com//pub/settle/stlags");
    l.append("ftp://ftp.cme.com//pub/settle/stlcur");
    l.append("ftp://ftp.cme.com//pub/settle/stleqt");
    l.append("ftp://ftp.cme.com//pub/settle/stlint");

    op.copy(l, file, FALSE);
  }
}

void CME::error (QNetworkOperation *o)
{
  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
    fileCount++;

  if (fileCount == 4)
  {
    QString s = file;
    s.append("/stlags");
    parseToday(s);

    s = file;
    s.append("/stlcur");
    parseToday(s);

    s = file;
    s.append("/stleqt");
    parseToday(s);

    s = file;
    s.append("/stlint");
    parseToday(s);

    emit done();
  }
}

void CME::error2 (QNetworkOperation *o)
{
  if (o->state() != QNetworkProtocol::StDone)
    return;

  if (o->errorCode() != QNetworkProtocol::NoError)
  {
    QString s = o->protocolDetail();
    qDebug(s.latin1());
    emit done();
    return;
  }

  QDir dir(file);
  if (dir.exists(symbolFile, TRUE))
  {
    parseHistory();
    emit done();
  }
}

void CME::parseHistory ()
{
  QString s = "unzip ";
  s.append(symbolFile);
  s.append(" -d ");
  s.append(file);
  if (system(s))
    return;

  s = symbolFile;
  s.truncate(s.length() - 3);
  s.append("eod");

  QFile f(s);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("could not open parse history file");
    return;
  }
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;

    Setting *set = new Setting;

    // date
    QString s2 = s.mid(31, 6);
    s2.prepend("20");
    s2.append("000000");
    set->set("Date", s2, Setting::None);

    // csymbol
    s2 = s.mid(37, 2);
    set->set("CSymbol", s2, Setting::None);

    // symbol
    s2 = s.mid(37, 2);
    s2.append("20");
    s2.append(s.mid(41, 2));

    QString month = s.mid(43, 2);
    switch (month.toInt())
    {
      case 1:
        month = "F";
        break;
      case 2:
        month = "G";
        break;
      case 3:
        month = "H";
        break;
      case 4:
        month = "J";
        break;
      case 5:
        month = "K";
        break;
      case 6:
        month = "M";
        break;
      case 7:
        month = "N";
        break;
      case 8:
        month = "Q";
        break;
      case 9:
        month = "U";
        break;
      case 10:
        month = "V";
        break;
      case 11:
        month = "X";
        break;
      default:
        month = "Z";
        break;
    }
    s2.append(month);
    set->set("Symbol", s2, Setting::None);

    set->set("Month", month, Setting::None);

    QString dec = s.mid(30, 1);
    
    // fix decimal for JY
    s2 = set->getData("CSymbol");
    if (! s2.compare("JY"))
      dec = QString::number(dec.toInt() - 2);

    // open
    s2 = s.mid(53, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->set("Open", s2, Setting::None);

    // high
    s2 = s.mid(73, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->set("High", s2, Setting::None);

    // low
    s2 = s.mid(83, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->set("Low", s2, Setting::None);

    // close
    s2 = s.mid(113, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->set("Close", s2, Setting::None);

    // volume
    s2 = s.mid(122, 7);
    set->set("Volume", s2, Setting::None);

    // oi
    s2 = s.mid(136, 7);
    set->set("OI", s2, Setting::None);

    parse(set);

    delete set;
  }

  f.close();
}

void CME::parseToday (QString filePath)
{
  QFile f(filePath);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  QString s = stream.readLine();
  s = s.stripWhiteSpace();
  QString s2 = "PRICES AS OF ";
  int p = s.contains(s2);
  s.remove(0, p + s2.length() - 1);
  QString date = s.left(8);
  QStringList dl = QStringList::split("/", date, FALSE);
  date = "20";
  date.append(dl[2]);
  date.append(dl[0]);
  date.append(dl[1]);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();

    if (s.contains("FC FEEDER CATTLE"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("FC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LB LUMBER FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LC LIVE CATTLE FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LH LEAN HOG FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LN");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("PB FROZEN PORK BELLY FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("PB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("AD AUSTRALIAN DOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("AD");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("NB BRITISH POUND FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("NB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("CD CANADIAN DOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("CD");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("EC EURO FX FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("EC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("JY JAPANESE YEN FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("JY");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("SF SWISS FRANC FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("SF");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ES E-MINI S&P 500 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ES");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("GI GSCI FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("GI");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ND NASDAQ-100 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ND");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("NQ E-MINI NASDAQ-100 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("NQ");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("SP S&P 500 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("SP");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ED EURODOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ED");
	saveTodayData(l);
      }
    }
  }

  f.close();
}

void CME::saveTodayData (QStringList l)
{
  Setting *set = new Setting();

  QString symbol = l[0];
  symbol.append("20");
  QString s = l[2];
  s.remove(0, 3);
  symbol.append(s);

  s = l[2];
  s.truncate(3);
  QString month;
  while (1)
  {
    if (! s.compare("JAN"))
    {
      month = "F";
      break;
    }

    if (! s.compare("FEB"))
    {
      month = "G";
      break;
    }

    if (! s.compare("MAR"))
    {
      month = "H";
      break;
    }

    if (! s.compare("APR"))
    {
      month = "J";
      break;
    }

    if (! s.compare("MAY"))
    {
      month = "K";
      break;
    }

    if (! s.compare("JUN"))
    {
      month = "M";
      break;
    }

    if (! s.compare("JLY"))
    {
      month = "N";
      break;
    }

    if (! s.compare("AUG"))
    {
      month = "Q";
      break;
    }

    if (! s.compare("SEP"))
    {
      month = "U";
      break;
    }

    if (! s.compare("OCT"))
    {
      month = "V";
      break;
    }

    if (! s.compare("NOV"))
    {
      month = "X";
      break;
    }

    month = "Z";

    break;
  }
  symbol.append(month);

  set->set("CSymbol", l[0], Setting::None);
  set->set("Symbol", symbol, Setting::None);

  s = l[1];
  s.append("000000");
  set->set("Date", s, Setting::None);

  set->set("Month", month, Setting::None);
  
  QString open = l[3];
  QString high = l[4];
  QString low = l[5];
  QString close = l[7];
  QString volume = l[9];
  QString oi = l[12];

  s = l[0];
  if (! s.compare("JY"))
  {
    setTFloat(open);
    open = QString::number(tfloat);
    if (open.toFloat() > 9999)
      open.insert(1, ".");
    else
      open.prepend(".");

    setTFloat(high);
    high = QString::number(tfloat);
    if (high.toFloat() > 9999)
      high.insert(1, ".");
    else
      high.prepend(".");

    setTFloat(low);
    low = QString::number(tfloat);
    if (low.toFloat() > 9999)
      low.insert(1, ".");
    else
      low.prepend(".");

    setTFloat(close);
    close = QString::number(tfloat);
    if (close.toFloat() > 9999)
      close.insert(1, ".");
    else
      close.prepend(".");
  }

  set->set("Open", open, Setting::None);
  set->set("High", high, Setting::None);
  set->set("Low", low, Setting::None);
  set->set("Close", close, Setting::None);
  set->set("Volume", volume, Setting::None);
  set->set("OI", oi, Setting::None);

  parse(set);

  delete set;
}

void CME::parse (Setting *data)
{
  if (setFutures(data->getData("CSymbol")))
    return;

  // open
  QString open;
  if (setTFloat(data->getData("Open")))
    return;
  else
    open = QString::number(tfloat);

  // high
  QString high;
  if (setTFloat(data->getData("High")))
    return;
  else
    high = QString::number(tfloat);

  // low
  QString low;
  if (setTFloat(data->getData("Low")))
    return;
  else
    low = QString::number(tfloat);

  // close
  QString close;
  if (setTFloat(data->getData("Close")))
    return;
  else
    close = QString::number(tfloat);

  // volume
  QString volume;
  if (setTFloat(data->getData("Volume")))
    return;
  else
    volume = QString::number(tfloat);

  // oi
  QString oi;
  if (setTFloat(data->getData("OI")))
    return;
  else
    oi = QString::number(tfloat);

  // check for bad values
  if (close.toFloat() == 0)
    return;
  if (open.toFloat() == 0)
    open = close;
  if (high.toFloat() == 0)
    high = close;
  if (low.toFloat() == 0)
    low = close;

  Setting *r = new Setting;
  r->set("Date", data->getData("Date"), Setting::Date);
  r->set("Open", open, Setting::Float);
  r->set("High", high, Setting::Float);
  r->set("Low", low, Setting::Float);
  r->set("Close", close, Setting::Float);
  r->set("Volume", volume, Setting::Float);
  r->set("Open Interest", oi, Setting::Float);

  QString s = dataPath;
  s.append("/");
  s.append(data->getData("Symbol"));
  ChartDb *db = new ChartDb();
  db->setPath(s);
  db->openChart();

  s = db->getChartType();
  if (! s.length())
  {
    db->setChartType(tr("Futures"));
    db->setSymbol(data->getData("Symbol"));
    db->setSource(pluginName);

    Setting *set = new Setting;
    set->set("Futures Month", data->getData("Month"), Setting::None);
    set->set("Futures Type", futureSymbol, Setting::None);
    set->set("Title", futureName, Setting::Text);
    db->setDetails(set);
    delete set;
  }

  db->setRecord(r);
  delete db;

  s = data->getData("Symbol");
  if (! s.compare(cc))
  {
    QString symbol = "CC";
    symbol.append(futureSymbol);

    s = dataPath;
    s.append("/");
    s.append(symbol);
    ChartDb *db = new ChartDb();
    db->setPath(s);
    db->openChart();

    s = db->getChartType();
    if (! s.length())
    {
      db->setChartType(tr("CC Futures"));
      db->setSymbol(symbol);
      db->setSource(pluginName);

      Setting *set = new Setting;
      set->set("Futures Type", futureSymbol, Setting::None);
      s = QObject::tr("Continuous ");
      s.append(futureSymbol);
      set->set("Title", s, Setting::Text);
      db->setDetails(set);
      delete set;
    }

    db->setRecord(r);
    delete db;
  }

  delete r;
}

void CME::cancelDownload ()
{
  op.stop();
  emit done();
}

Plugin * create ()
{
  CME *o = new CME;
  return ((Plugin *) o);
}

