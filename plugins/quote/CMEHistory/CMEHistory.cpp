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

#include "CMEHistory.h"
#include "ChartDb.h"
#include "PrefDialog.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qstringlist.h>
#include <stdlib.h>
#include <qtimer.h>
#include <qsettings.h>

CMEHistory::CMEHistory ()
{
  pluginName = "CMEHistory";
  fd = new FuturesData;
  connect(&op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  
  loadSettings();
  qInitNetworkProtocols();
}

CMEHistory::~CMEHistory ()
{
  delete fd;
}

void CMEHistory::update ()
{
  QTimer::singleShot(250, this, SLOT(getFile()));
}

void CMEHistory::getFile ()
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

  op.copy(url, file, FALSE, FALSE);
  
  QString s = tr("Downloading");
  s.append(" ");
  s.append(url);
  emit statusLogMessage(s);
}

void CMEHistory::opDone (QNetworkOperation *o)
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

void CMEHistory::parseHistory ()
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

  QString path = createDirectory("Futures");
  if (! path.length())
  {
    emit statusLogMessage("Unable to create futures directory");
    return;
  }

  QString s = "Futures/";
  s.append(currentSymbol);
  path = createDirectory(s);
  if (! path.length())
  {
    emit statusLogMessage("Unable to create symbol directory");
    return;
  }

  s = tr("Updating ") + data->getData("Symbol");
  emit statusLogMessage(s);

  Bar *r = new Bar;
  if (r->setDate(data->getData("Date")))
  {
    delete r;
    emit statusLogMessage("Bad date " + data->getData("Date"));
    return;
  }
  r->setOpen(open.toDouble());
  r->setHigh(high.toDouble());
  r->setLow(low.toDouble());
  r->setClose(close.toDouble());
  r->setVolume(volume.toDouble());
  r->setOI(oi.toInt());

  s = path;
  s.append("/");
  s.append(data->getData("Symbol"));
  ChartDb *db = new ChartDb();
  db->openChart(s);

  s = db->getDetail(ChartDb::Symbol);
  if (! s.length())
  {
    db->setDetail(ChartDb::Symbol, data->getData("Symbol"));
    db->setDetail(ChartDb::Title, fd->getName());
    db->setDetail(ChartDb::Type, "Futures");
    db->setDetail(ChartDb::FuturesType, fd->getSymbol());
    db->setDetail(ChartDb::FuturesMonth, data->getData("Month"));
    db->setDetail(ChartDb::BarType, QString::number(BarData::Daily));
  }

  db->setBar(r);
  delete db;
  
  emit dataLogMessage(data->getData("Symbol") + " " + r->getString());
  delete r;
}

void CMEHistory::cancelUpdate ()
{
  op.stop();
  emit done();
  emit statusLogMessage("Cancelled");
}

void CMEHistory::prefDialog ()
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

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("CMEHistory Prefs"));
  dialog->createPage (tr("Details"));
  dialog->addComboItem(tr("Symbol"), tr("Details"), l, currentSymbol);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    currentSymbol = dialog->getCombo(tr("Symbol"));
    saveFlag = TRUE;
  }
  
  delete dialog;
}

void CMEHistory::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CMEHistory plugin");

  currentSymbol = settings.readEntry("/Symbol", "AD");
  
  settings.endGroup();
}

void CMEHistory::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/CMEHistory plugin");
  
  settings.writeEntry("/Symbol", currentSymbol);
  
  settings.endGroup();
}

Plugin * create ()
{
  CMEHistory *o = new CMEHistory;
  return ((Plugin *) o);
}

