/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "PrefDialog.h"
#include "Bar.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qstringlist.h>
#include <stdlib.h>
#include <qtimer.h>
#include <qsettings.h>

CME::CME ()
{
  pluginName = "CME";
  op = 0;
  symbolCombo = 0;
  
  connect(&opHistory, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opHistoryDone(QNetworkOperation *)));
  
  loadSettings();
  qInitNetworkProtocols();
}

CME::~CME ()
{
}

void CME::update ()
{
  if (! method.compare(tr("Today")))
  {
    urlList.clear();
    symbolLoop = 0;
    op = 0;

    QDir dir = QDir::home();
    file = dir.path();
    file.append("/Qtstalker/download");

    urlList.append("ftp://ftp.cme.com//pub/settle/stlags");
    urlList.append("ftp://ftp.cme.com//pub/settle/stlcur");
    urlList.append("ftp://ftp.cme.com//pub/settle/stleqt");
    urlList.append("ftp://ftp.cme.com//pub/settle/stlint");

    QTimer::singleShot(250, this, SLOT(getFile()));
  }
  else  
    QTimer::singleShot(250, this, SLOT(getFileHistory()));
}

void CME::opDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() != QNetworkProtocol::StDone)
    return;

  if (o->errorCode() != QNetworkProtocol::NoError)
  {
    QString s = o->protocolDetail();
    qDebug(s.latin1());
    delete op;
    emit statusLogMessage("Done");
    emit done();
    return;
  }
  
  QDir dir(file);
  if (! dir.exists(file, TRUE))
    return;

  parseToday();

  symbolLoop++;

  if (symbolLoop >= (int) urlList.count())
  {
    emit statusLogMessage("Done");
    emit done();
    delete op;
    return;
  }

  getFile();
}

void CME::opHistoryDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() != QNetworkProtocol::StDone)
    return;

  if (o->errorCode() != QNetworkProtocol::NoError)
  {
    emit statusLogMessage(tr("Download error: bailing out"));
    QString s = o->protocolDetail();
    qDebug(s.latin1());
    emit done();
    return;
  }

  QDir dir(file);
  if (! dir.exists(file, TRUE))
    return;

  parseHistory();

  emit statusLogMessage(tr("Done"));
  emit done();
}

void CME::getFile ()
{
  if (op)
    delete op;
    
  QDir dir(file);
  dir.remove(file);

  op = new QUrlOperator();
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  op->copy(urlList[symbolLoop], file, FALSE, FALSE);
  
  QString s = "Downloading ";
  s.append(urlList[symbolLoop]);
  emit statusLogMessage(s);
}

void CME::getFileHistory ()
{
  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download.zip");
  dir.remove(file);

  file2 = dir.path();
  file2.append("/Qtstalker");

  url = "ftp://ftp.cme.com//pub/hist_eod/";
  url.append(currentSymbol.lower());
  url.append("ytd.zip");

  opHistory.copy(url, file, FALSE, FALSE);
  
  QString s = tr("Downloading");
  s.append(" ");
  s.append(url);
  emit statusLogMessage(s);
}

void CME::parseToday ()
{
  QFile f(file);
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

void CME::parseHistory ()
{
  QString s2 = file2;
  s2.append("/");
  s2.append(currentSymbol.lower());
  s2.append("ytd.eod");
  QDir dir(s2);
  dir.remove(s2);

  QString s = "unzip ";
  s.append(file);
  s.append(" -d ");
  s.append(file2);
  if (system(s))
    return;

  QFile f(s2);
  if (! f.open(IO_ReadOnly))
  {
    emit statusLogMessage("could not open parse history file");
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
    s2 = s.mid(31, 6);
    s2.prepend("20");
    s2.append("000000");
    set->setData("Date", s2);

    // csymbol
    s2 = s.mid(37, 2);
    set->setData("CSymbol", s2);

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
    set->setData("Symbol", s2);

    set->setData("Month", month);

    QString dec = s.mid(30, 1);
    
    // fix decimal for JY
    s2 = set->getData("CSymbol");
    if (! s2.compare("JY"))
      dec = QString::number(dec.toInt() - 2);

    // open
    s2 = s.mid(53, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->setData("Open", s2);

    // high
    s2 = s.mid(73, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->setData("High", s2);

    // low
    s2 = s.mid(83, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->setData("Low", s2);

    // close
    s2 = s.mid(113, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->setData("Close", s2);

    // volume
    s2 = s.mid(122, 7);
    set->setData("Volume", s2);

    // oi
    s2 = s.mid(136, 7);
    set->setData("OI", s2);

    parse(set);

    delete set;
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

  set->setData("CSymbol", l[0]);
  set->setData("Symbol", symbol);

  s = l[1];
  s.append("000000");
  set->setData("Date", s);

  set->setData("Month", month);
  
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

  set->setData("Open", open);
  set->setData("High", high);
  set->setData("Low", low);
  set->setData("Close", close);
  set->setData("Volume", volume);
  set->setData("OI", oi);

  parse(set);

  delete set;
}

void CME::parse (Setting *data)
{
  if (fd.setSymbol(data->getData("CSymbol")))
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

  QString path = createDirectory("Futures");
  if (! path.length())
  {
    emit statusLogMessage("Unable to create futures directory");
    return;
  }

  QString s = "Futures/";
  s.append(fd.getSymbol());
  path = createDirectory(s);
  if (! path.length())
  {
    emit statusLogMessage("Unable to create symbol directory");
    return;
  }

  s = tr("Updating ") + data->getData("Symbol");
  emit statusLogMessage(s);

  ChartDb *db = new ChartDb;
  db->setPlugin("Futures");
  s = path;
  s.append("/");
  s.append(data->getData("Symbol"));
  if (db->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    delete db;
    return;
  }
  
  s = db->getData("Symbol");
  if (! s.length())
  {
    Setting *set = new Setting;
    set->setData("BarType", QString::number(BarData::Daily));
    set->setData("Symbol", data->getData("Symbol"));
    set->setData("Title", fd.getName());
    set->setData("FuturesType", fd.getSymbol());
    set->setData("FuturesMonth", data->getData("Month"));
    db->saveDbDefaults(set);
    delete set;
  }
  
  Bar *bar = new Bar;
  if (bar->setDate(data->getData("Date")))
  {
    emit statusLogMessage("Bad date " + data->getData("Date"));
    delete bar;
    return;
  }
  bar->setOpen(open.toDouble());
  bar->setHigh(high.toDouble());
  bar->setLow(low.toDouble());
  bar->setClose(close.toDouble());
  bar->setVolume(volume.toDouble());
  bar->setOI(oi.toInt());
  db->setBar(bar);
  delete bar;
	     
  delete db;

//  emit dataLogMessage(data->getData("Symbol"));
}

void CME::cancelUpdate ()
{
  if (! method.compare(tr("Today")))
  {
    if (op)
      op->stop();
  }
  else
    opHistory.stop();
  
  emit done();
  emit statusLogMessage("Canceled");
}

void CME::prefDialog (QWidget *w)
{
  QStringList l;
  l.append("AD");
  l.append("CD");
  l.append("EC");
  l.append("ES");
  l.append("JY");
  l.append("FC");
  l.append("GI");
  l.append("LB");
  l.append("LC");
  l.append("LN");
  l.append("NB");
  l.append("ND");
  l.append("PB");
  l.append("SF");
  l.append("NQ");
  l.append("SP");
  l.append("ED");
  l.sort();

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("CME Prefs"));
  dialog->createPage (tr("Details"));

  QStringList l2;
  l2.append("Today");
  l2.append("History");
  dialog->addComboItem(tr("Method"), tr("Details"), l2, method);
  connect(dialog->getComboWidget(tr("Method")),
          SIGNAL(activated(const QString &)),
	  this,
	  SLOT(methodChanged(const QString &)));
    
  dialog->addComboItem(tr("Symbol"), tr("Details"), l, currentSymbol);
  symbolCombo = dialog->getComboWidget(tr("Symbol"));
  
  methodChanged (method);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    currentSymbol = dialog->getCombo(tr("Symbol"));
    saveFlag = TRUE;
  }
  
  delete dialog;
}

void CME::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CME plugin");

  method = settings.readEntry("/Method", "Today");
  currentSymbol = settings.readEntry("/Symbol", "AD");
  
  settings.endGroup();
}

void CME::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/CME plugin");
  
  settings.writeEntry("/Method", method);
  settings.writeEntry("/Symbol", currentSymbol);
  
  settings.endGroup();
}

void CME::methodChanged (const QString & d)
{
  method = d;
  
  if (! method.compare(tr("Today")))
    symbolCombo->setEnabled(FALSE);
  else  
    symbolCombo->setEnabled(TRUE);
}

Plugin * create ()
{
  CME *o = new CME;
  return ((Plugin *) o);
}

