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

#include "CMEHistory.h"
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qstringlist.h>
#include <stdlib.h>
#include <qtimer.h>

CMEHistory::CMEHistory ()
{
  pluginName = "CMEHistory";
  version = 0.2;
  fd = new FuturesData;
  createFlag = FALSE;
  op = 0;

  about = "Downloads daily settlement quotes from CME\n";
  about.append("and imports it directly into qtstalker.");
}

CMEHistory::~CMEHistory ()
{
  if (op)
    delete op;

  delete fd;
}

void CMEHistory::update ()
{
  symbolList.clear();
  urlList.clear();
  symbolLoop = 0;
  op = 0;

  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download.zip");

  file2 = dir.path();
  file2.append("/Qtstalker");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/fcytd.zip");
  symbolList.append("FC");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/lbytd.zip");
  symbolList.append("LB");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/lcytd.zip");
  symbolList.append("LC");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/lnytd.zip");
  symbolList.append("LN");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/pbytd.zip");
  symbolList.append("PB");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/adytd.zip");
  symbolList.append("AD");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/nbytd.zip");
  symbolList.append("NB");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/cdytd.zip");
  symbolList.append("CD");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/ecytd.zip");
  symbolList.append("EC");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/jyytd.zip");
  symbolList.append("JY");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/sfytd.zip");
  symbolList.append("SF");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/esytd.zip");
  symbolList.append("ES");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/giytd.zip");
  symbolList.append("GI");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/ndytd.zip");
  symbolList.append("ND");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/nqytd.zip");
  symbolList.append("NQ");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/spytd.zip");
  symbolList.append("SP");

  urlList.append("ftp://ftp.cme.com//pub/hist_eod/edytd.zip");
  symbolList.append("ED");

  qInitNetworkProtocols();

  QTimer::singleShot(250, this, SLOT(getFile()));
}

void CMEHistory::getFile ()
{
  if (op)
    delete op;

  QDir dir(file);
  dir.remove(file);

  op = new QUrlOperator();
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  op->copy(urlList[symbolLoop], file, FALSE, FALSE);
}

void CMEHistory::opDone (QNetworkOperation *o)
{
  if (o->state() != QNetworkProtocol::StDone)
    return;

  if (o->errorCode() != QNetworkProtocol::NoError)
  {
    QString s = o->protocolDetail();
    qDebug(s.latin1());
    delete op;
    emit done();
    return;
  }

  QDir dir(file);
  if (! dir.exists(file, TRUE))
    return;

  parseHistory();

  symbolLoop++;

  if (symbolLoop >= (int) symbolList.count())
  {
    emit done();
    delete op;
    return;
  }

  getFile();
}

void CMEHistory::parseHistory ()
{
  QString s2 = file2;
  s2.append("/");
  s2.append(symbolList[symbolLoop].lower());
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
    s2 = s.mid(31, 6);
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

void CMEHistory::parse (Setting *data)
{
  if (fd->setSymbol(data->getData("CSymbol")))
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
    
  QString s = dataPath;
  s.append("/");
  s.append(symbolList[symbolLoop]);
  QDir dir(s);
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
    {
      qDebug("CMEHistory plugin: Unable to create directory");
      return;
    }
  }

  Setting *r = new Setting;
  r->set("Date", data->getData("Date"), Setting::Date);
  r->set("Open", open, Setting::Float);
  r->set("High", high, Setting::Float);
  r->set("Low", low, Setting::Float);
  r->set("Close", close, Setting::Float);
  r->set("Volume", volume, Setting::Float);
  r->set("Open Interest", oi, Setting::Float);

  s = dataPath;
  s.append("/");
  s.append(symbolList[symbolLoop]);
  s.append("/");
  s.append(data->getData("Symbol"));
  ChartDb *db = new ChartDb();
  db->openChart(s);

  Setting *details = db->getDetails();
  if (! details->count())
  {
    details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
    details->set("Chart Type", tr("Futures"), Setting::None);
    details->set("Symbol", data->getData("Symbol"), Setting::None);
    details->set("Source", pluginName, Setting::None);
    details->set("Futures Month", data->getData("Month"), Setting::None);
    details->set("Futures Type", fd->getSymbol(), Setting::None);
    details->set("Title", fd->getName(), Setting::Text);
  }

  db->setRecord(r);
  delete db;
  delete r;
}

void CMEHistory::cancelUpdate ()
{
  op->stop();
  emit done();
}

Plugin * create ()
{
  CMEHistory *o = new CMEHistory;
  return ((Plugin *) o);
}

