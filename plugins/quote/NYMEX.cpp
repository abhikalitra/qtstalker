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

#include "NYMEX.h"
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qstringlist.h>

NYMEX::NYMEX ()
{
  connect(&op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(error(QNetworkOperation *)));

  pluginName = "NYMEX";
  version = 0.2;

  about = "Downloads daily settlement quotes from NYMEX\n";
  about.append("and COMEX, and imports it directly into qtstalker.\n");
  about.append("NOTE: There is no Open and Open Interest data available.");
}

NYMEX::~NYMEX ()
{
}

void NYMEX::download ()
{
  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker");

  qInitNetworkProtocols();

  QString s = file;
  s.append("/innf.txt");
  dir.remove(s, TRUE);
  
  s = file;
  s.append("/incf.txt");
  dir.remove(s, TRUE);

  QStringList l;
  l.append("http://www.nymex.com/futures/innf.txt");
  l.append("http://www.nymex.com/futures/incf.txt");

  fileCount = 0;

  op.copy(l, file, FALSE);
}

void NYMEX::error (QNetworkOperation *o)
{
  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
    fileCount++;

  if (fileCount == 2)
  {
    QString s = file;
    s.append("/innf.txt");
    parseFile(s);

    s = file;
    s.append("/incf.txt");
    parseFile(s);

    emit done();
  }
}

void NYMEX::parse (QString d)
{
  QString s = stripJunk(d);

  QStringList l = QStringList::split(",", s, FALSE);
  if (l.count() != 10)
    return;

  // futures month
  QString fmonth;
  switch (l[2].toInt())
  {
    case 1:
      fmonth = "F";
      break;
    case 2:
      fmonth = "G";
      break;
    case 3:
      fmonth = "H";
      break;
    case 4:
      fmonth = "J";
      break;
    case 5:
      fmonth = "K";
      break;
    case 6:
      fmonth = "M";
      break;
    case 7:
      fmonth = "N";
      break;
    case 8:
      fmonth = "Q";
      break;
    case 9:
      fmonth = "U";
      break;
    case 10:
      fmonth = "V";
      break;
    case 11:
      fmonth = "X";
      break;
    case 12:
      fmonth = "Z";
      break;
    default:
      break;
  }

  // symbol
  QString symbol = l[1];
  symbol.append("20");
  symbol.append(l[3]);
  symbol.append(fmonth);

  if (setFutures(l[1]))
    return;

  // date
  QString date = l[0];
  date.append("000000");

  // high
  QString high;
  if (setTFloat(l[8]))
    return;
  else
    high = QString::number(tfloat);

  // low
  QString low;
  if (setTFloat(l[9]))
    return;
  else
    low = QString::number(tfloat);

  // close
  QString close;
  if (setTFloat(l[5]))
    return;
  else
    close = QString::number(tfloat);

  // volume
  QString volume;
  if (setTFloat(l[7]))
    return;
  else
    volume = QString::number(tfloat);

  // check for bad values
  if (high.toFloat() == 0)
    high = close;
  if (low.toFloat() == 0)
    low = close;

  Setting *r = new Setting;
  r->set("Date", date, Setting::Date);
  r->set("Open", close, Setting::Float);
  r->set("High", high, Setting::Float);
  r->set("Low", low, Setting::Float);
  r->set("Close", close, Setting::Float);
  r->set("Volume", volume, Setting::Float);
  r->set("Open Interest", "0", Setting::Float);

  s = dataPath;
  s.append("/");
  s.append(symbol);
  ChartDb *db = new ChartDb();
  db->setPath(s);
  db->openChart();

  s = db->getChartType();
  if (! s.length())
  {
    db->setChartType(tr("Futures"));
    db->setSymbol(symbol);
    db->setSource(pluginName);

    Setting *set = new Setting;
    set->set("Futures Month", fmonth, Setting::None);
    set->set("Futures Type", futureSymbol, Setting::None);
    set->set("Title", futureName, Setting::Text);
    db->setDetails(set);
    delete set;
  }

  db->setRecord(r);
  delete db;

  if (! symbol.compare(cc))
  {
    symbol = "CC";
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

void NYMEX::parseFile (QString d)
{
  QFile f(d);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  QString date;
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    int p = s.find("/", 0, TRUE);
    if (p != -1)
    {
      date = s.mid(p - 2, 10);
      break;
    }
  }
  QStringList dl = QStringList::split("/", date, FALSE);
  date = dl[2];
  date.append(dl[0]);
  date.append(dl[1]);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    int p = s.find("LOW", 0, TRUE);
    if (p != -1)
      break;
  }

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();

    while (s.contains(","))
    {
      int pos = s.find(",", 0, TRUE);
      s.remove(pos, 1);
    }

    QStringList l = QStringList::split(" ", s, FALSE);
    if (l.count() != 9)
      continue;

    if (! l[0].compare("NN") || ! l[0].compare("PN") || ! l[0].compare("QJ") || ! l[0].compare("QL") ||
        ! l[0].compare("SC") || ! l[0].compare("AL"))
      continue;

    if (l[7].toFloat() == 0 || l[8].toFloat() == 0)
      continue;

    l.prepend(date);

    parse(l.join(","));
  }

  f.close();
}

void NYMEX::cancelDownload ()
{
  op.stop();
  emit done();
}

Plugin * create ()
{
  NYMEX *o = new NYMEX;
  return ((Plugin *) o);
}


