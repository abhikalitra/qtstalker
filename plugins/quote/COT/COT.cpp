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

#include "COT.h"
#include "../../../src/ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <stdlib.h>
#include <qdatetime.h>

COT::COT ()
{
  pluginName = "COT";
  version = 0.2;
  createFlag = FALSE;
  op = 0;

  QStringList l;
  l.append("Current");
  l.append("History");

  set(tr("Data"), "Current", Setting::List);
  setList(tr("Data"), l);

  about = "Downloads COT and creates a chart.";

  qInitNetworkProtocols();
}

COT::~COT ()
{
}

void COT::update ()
{
  data.truncate(0);
  op = 0;

  QString s = getData("Data");
  if (! s.compare("Current"))
  {
    url = "http://www.cftc.gov/dea/newcot/deafut.txt";
    QTimer::singleShot(250, this, SLOT(getFile()));
  }
  else
  {
    url = "http://www.cftc.gov/files/dea/history/deacot";
    QDate dt = QDate::currentDate();
    url.append(QString::number(dt.year()));
    url.append(".zip");
    QTimer::singleShot(250, this, SLOT(getFile2()));
  }
}

void COT::getFile ()
{
  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download");

  op = new QUrlOperator(url);
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)), this, SLOT(dataReady(const QByteArray &, QNetworkOperation *)));
  op->get();
  
  emit message(tr("Downloading COT data"));
}

void COT::getFile2 ()
{
  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download.zip");
  dir.remove(file);

  op = new QUrlOperator();
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone2(QNetworkOperation *)));
  op->copy(url, file, FALSE, FALSE);
  
  emit message(tr("Downloading COT data"));
}

void COT::opDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
  {
    QFile f(file);
    if (! f.open(IO_WriteOnly))
      return;
    QTextStream stream(&f);
    stream << data;
    f.close();

    delete op;
    parse();
    emit done();
    return;
  }

  if (o->state() == QNetworkProtocol::StFailed)
  {
    emit message(tr("Download error"));
    emit done();
    delete op;
    return;
  }
}

void COT::opDone2 (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() != QNetworkProtocol::StDone)
    return;

  if (o->errorCode() != QNetworkProtocol::NoError)
  {
    emit message(tr("Download error"));
    QString s = o->protocolDetail();
    qDebug(s.latin1());
    delete op;
    emit done();
    return;
  }

  QDir dir(file);
  if (! dir.exists(file, TRUE))
    return;

  QString tfile = dir.homeDirPath();
  tfile.append("/Qtstalker/ANNUAL.TXT");
  if (dir.exists(tfile))
    dir.remove(tfile, TRUE);
  else
  {
    tfile = dir.homeDirPath();
    tfile.append("/Qtstalker/annual.txt");
    dir.remove(tfile, TRUE);
  }

  QString s = dir.homeDirPath();
  s.append("/Qtstalker");

  QString s2 = "unzip ";
  s2.append(file);
  s2.append(" -d ");
  s2.append(s);
  if (system(s2))
  {
    delete op;
    emit done();
    return;
  }

  file = tfile;

  parse();

  delete op;

  emit done();
}

void COT::dataReady (const QByteArray &d, QNetworkOperation *)
{
  int loop;
  for (loop = 0; loop < (int) d.size(); loop++)
    data.append(d[loop]);
}

void COT::parse ()
{
  QFile f(file);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    s = stripJunk(s);
    if (s.length() < 10)
      continue;

    if (s.contains("CRUDE OIL,") ||
        s.contains("COCOA - COFFEE,") ||
        s.contains("SUGAR NO. 11") ||
        s.contains("COFFEE C") ||
        s.contains("UNLEADED GASOLINE"))
    {
      int i = s.find(',', 0, TRUE);
      if (i != -1)
        s.remove(i, 1);
    }

    QStringList l = QStringList::split(",", s, FALSE);

    QString symbol = getSymbol(l[0]);
    if (! symbol.length())
      continue;

    QString date = l[2];
    date.remove(7, 1);
    date.remove(4, 1);
    date.append("000000");
    date = date.stripWhiteSpace();

    if (setTFloat(l[7]))
      continue;
    QString oi = QString::number(tfloat);

    if (setTFloat(l[8]))
      continue;
    QString ncl = QString::number(tfloat);

    if (setTFloat(l[9]))
      continue;
    QString ncs = QString::number(tfloat);

    if (setTFloat(l[11]))
      continue;
    QString cl = QString::number(tfloat);

    if (setTFloat(l[12]))
      continue;
    QString cs = QString::number(tfloat);

    if (setTFloat(l[15]))
      continue;
    QString nrl = QString::number(tfloat);

    if (setTFloat(l[16]))
      continue;
    QString nrs = QString::number(tfloat);

    Setting *set = new Setting;
    set->set("Symbol", symbol, Setting::None);
    set->set("Date", date, Setting::None);
    set->set("Open Interest", oi, Setting::Float);
    set->set("Non Commercial", QString::number(ncl.toFloat() - ncs.toFloat()), Setting::Float);
    set->set("Commercial", QString::number(cl.toFloat() - cs.toFloat()), Setting::Float);
    set->set("Non Reportable", QString::number(nrl.toFloat() - nrs.toFloat()), Setting::Float);
    set->set("Title", l[0], Setting::Text);

    saveData(set);

    delete set;
  }

  f.close();
}

void COT::saveData (Setting *set)
{
  QString s = dataPath;
  s.append("/COT");
  QDir dir(s);
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
    {
      qDebug("COT plugin: Unable to create directory");
      return;
    }
  }

  s.append("/");
  s.append(set->getData("Symbol"));
  ChartDb *db = new ChartDb();
  db->openChart(s);
  
  s = tr("Updating ");
  s.append(set->getData("Symbol"));
  emit message(s);

  Setting *details = db->getDetails();
  if (! details->count())
  {
    details->set("Format", "Non Commercial|Commercial|Non Reportable|Open Interest", Setting::None);
    details->set("Chart Type", "COT", Setting::None);
    details->set("Symbol", set->getData("Symbol"), Setting::None);
    details->set("Title", set->getData("Title"), Setting::Text);
    details->set("Non Commercial Color", "red", Setting::Color);
    details->set("Non Commercial Line Type", "Line", Setting::LineType);
    details->set("Commercial Color", "green", Setting::Color);
    details->set("Commercial Line Type", "Line", Setting::LineType);
    details->set("Non Reportable Color", "blue", Setting::Color);
    details->set("Non Reportable Line Type", "Line", Setting::LineType);
    details->set("Open Interest Color", "yellow", Setting::Color);
    details->set("Open Interest Line Type", "Line", Setting::LineType);
    db->saveDetails();
    db->setFormat();
  }
  set->remove("Symbol");
  set->remove("Title");

  db->setRecord(set);
  delete db;
}

QString COT::getSymbol (QString dat)
{
  QString symbol;

  while (1)
  {
    if (dat.contains("WHEAT") && dat.contains("CHICAGO"))
    {
      symbol = "W";
      break;
    }

    if (dat.contains("CORN"))
    {
      symbol = "C";
      break;
    }

    if (dat.contains("OATS"))
    {
      symbol = "O";
      break;
    }

    if (dat.contains("SOYBEANS"))
    {
      symbol = "S";
      break;
    }

    if (dat.contains("SOYBEAN OIL"))
    {
      symbol = "BO";
      break;
    }

    if (dat.contains("SOYBEAN MEAL"))
    {
      symbol = "SM";
      break;
    }

    if (dat.contains("TREASURY BONDS"))
    {
      symbol = "US";
      break;
    }

    if (dat.contains("NATURAL GAS"))
    {
      symbol = "NG";
      break;
    }

    if (dat.contains("COTTON NO. 2"))
    {
      symbol = "CT";
      break;
    }

    if (dat.contains("ORANGE JUICE"))
    {
      symbol = "JO";
      break;
    }

    if (dat.contains("10-YEAR") && dat.contains("TREASURY NOTES"))
    {
      symbol = "TY";
      break;
    }

    if (dat.contains("LEAN HOGS"))
    {
      symbol = "LN";
      break;
    }

    if (dat.contains("PORK BELLIES"))
    {
      symbol = "PB";
      break;
    }

    if (dat.contains("LIVE CATTLE"))
    {
      symbol = "LC";
      break;
    }

    if (dat.contains("LUMBER"))
    {
      symbol = "LB";
      break;
    }

    if (dat.contains("FEEDER CATTLE"))
    {
      symbol = "FC";
      break;
    }

    if (dat.contains("CRUDE OIL"))
    {
      symbol = "CL";
      break;
    }

    if (dat.contains("COCOA -"))
    {
      symbol = "CC";
      break;
    }

    if (dat.contains("PALLADIUM"))
    {
      symbol = "PA";
      break;
    }

    if (dat.contains("PLATINUM"))
    {
      symbol = "PL";
      break;
    }

    if (dat.contains("SUGAR NO. 11"))
    {
      symbol = "SB";
      break;
    }

    if (dat.contains("COFFEE C"))
    {
      symbol = "KC";
      break;
    }

    if (dat.contains("SILVER"))
    {
      symbol = "SI";
      break;
    }

    if (dat.contains("COPPER"))
    {
      symbol = "HG";
      break;
    }

    if (dat.contains("GOLD"))
    {
      symbol = "GC";
      break;
    }

    if (dat.contains("CANADIAN DOLLAR"))
    {
      symbol = "CD";
      break;
    }

    if (dat.contains("SWISS FRANC"))
    {
      symbol = "SF";
      break;
    }

    if (dat.contains("POUND STERLING"))
    {
      symbol = "NB";
      break;
    }

    if (dat.contains("JAPANESE YEN"))
    {
      symbol = "JY";
      break;
    }

    if (dat.contains("U.S. DOLLAR INDEX"))
    {
      symbol = "DX";
      break;
    }

    if (dat.contains("EURO FX"))
    {
      symbol = "EC";
      break;
    }

    if (dat.contains("UNLEADED GASOLINE"))
    {
      symbol = "HU";
      break;
    }

    if (dat.contains("DOW JONES INDUSTRIAL AVERAGE"))
    {
      symbol = "DJ";
      break;
    }

    if (dat.contains("EURODOLLARS"))
    {
      symbol = "ED";
      break;
    }

    if (dat.contains("AUSTRALIAN DOLLAR"))
    {
      symbol = "AD";
      break;
    }
    
    if (dat.contains("S&P 500"))
    {
      if (dat.contains("E-MINI"))
        symbol = "ES";
      else
        symbol = "SP";

      break;
    }

    if (dat.contains("NASDAQ-100"))
    {
      if (dat.contains("MINI"))
        symbol = "NQ";
      else
        symbol = "ND";

      break;
    }

    break;
  }

  return symbol;
}

void COT::cancelUpdate ()
{
  if (op)
    op->stop();
  emit done();
}

Plugin * create ()
{
  COT *o = new COT;
  return ((Plugin *) o);
}

