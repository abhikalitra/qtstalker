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

#include "COT.h"
#include "DbPlugin.h"
#include "PrefDialog.h"
#include "Bar.h"
#include "Config.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <stdlib.h>
#include <qdatetime.h>
#include <qsettings.h>


COT::COT ()
{
  pluginName = "COT";
  helpFile = "cot.html";
  
  connect(this, SIGNAL(signalCopyFileDone(QString)), this, SLOT(slotCopyFileDone(QString)));
  connect(this, SIGNAL(signalGetFileDone(bool)), this, SLOT(slotGetFileDone(bool)));
  connect(this, SIGNAL(signalTimeout()), this, SLOT(timeoutError()));

  loadSettings();
  qInitNetworkProtocols();
}

COT::~COT ()
{
}

void COT::update ()
{
  errorLoop = 0;

  if (! format.compare("Current"))
  {
    Config config;
    file = config.getData(Config::Home) + "/download";
  
    url = "http://www.cftc.gov/dea/newcot/deafut.txt";
    QString s = tr("Downloading ") + url;
    emit statusLogMessage(s);

    getFile(url);
    
//    QTimer::singleShot(250, this, SLOT(getFile()));
  }
  else
  {
    Config config;
    file = config.getData(Config::Home) + "/download.zip";
    QDir dir;
    dir.remove(file);
    
    url = "http://www.cftc.gov/files/dea/history/deacot";
    QDate dt = QDate::currentDate();
    url.append(QString::number(dt.year()));
    url.append(".zip");
    
    QString s = tr("Downloading ") + url;
    emit statusLogMessage(s);
    
    copyFile(url, file);
    
//    QTimer::singleShot(250, this, SLOT(getFile2()));
  }
}

void COT::slotGetFileDone (bool d)
{
  if (d)
  {
    emit statusLogMessage(tr("Download error"));
    emit done();
    return;
  }

  QFile f(file);
  if (! f.open(IO_WriteOnly))
  {
    emit statusLogMessage(tr("Unable to create temp file"));
    emit done();
    return;
  }
  QTextStream stream(&f);
  stream << data;
  f.close();

  parse();
  
  emit statusLogMessage(tr("Done"));
  emit done();
}

void COT::slotCopyFileDone (QString d)
{
  if (d.length())
  {
    emit statusLogMessage(d);
    emit done();
    return;
  }

  QDir dir(file);
  if (! dir.exists(file, TRUE))
  {
    emit statusLogMessage(tr("Unable to create temp file"));
    emit done();
    return;
  }

  Config config;
  QString tfile = config.getData(Config::Home) + "/ANNUAL.TXT";
  if (dir.exists(tfile))
    dir.remove(tfile, TRUE);
  else
  {
    tfile = config.getData(Config::Home) + "/annual.txt";
    dir.remove(tfile, TRUE);
  }

  QString s = config.getData(Config::Home);
  QString s2 = "unzip ";
  s2.append(file);
  s2.append(" -d ");
  s2.append(s);
  if (system(s2))
  {
    emit statusLogMessage(tr("Unzip file failed."));
    emit done();
    return;
  }

  file = tfile;

  parse();

  emit statusLogMessage(tr("Done"));
  emit done();
}

void COT::timeoutError ()
{
  errorLoop++;
  
  if (errorLoop == retries)
  {
    emit statusLogMessage(tr("Timeout: retry limit."));
    emit done();
    return;
  }
  else
  {
    QString s = tr("Timeout: retry ") + QString::number(errorLoop + 1);
    emit statusLogMessage(s);
    
    if (! format.compare("Current"))
      getFile(url);
    else
      copyFile(url, file);
  }
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

    if (setTFloat(l[7], FALSE))
      continue;
    QString oi = QString::number(tfloat);

    if (setTFloat(l[8], FALSE))
      continue;
    QString ncl = QString::number(tfloat);

    if (setTFloat(l[9], FALSE))
      continue;
    QString ncs = QString::number(tfloat);

    if (setTFloat(l[11], FALSE))
      continue;
    QString cl = QString::number(tfloat);

    if (setTFloat(l[12], FALSE))
      continue;
    QString cs = QString::number(tfloat);

    if (setTFloat(l[15], FALSE))
      continue;
    QString nrl = QString::number(tfloat);

    if (setTFloat(l[16], FALSE))
      continue;
    QString nrs = QString::number(tfloat);

    Setting *set = new Setting;
    set->setData("Symbol", symbol);
    set->setData("Date", date);
    set->setData("Open Interest", oi);
    set->setData("Non Commercial", QString::number(ncl.toFloat() - ncs.toFloat()));
    set->setData("Commercial", QString::number(cl.toFloat() - cs.toFloat()));
    set->setData("Non Reportable", QString::number(nrl.toFloat() - nrs.toFloat()));
    set->setData("Title", l[0]);

    saveData(set);

    delete set;
  }

  f.close();
}

void COT::saveData (Setting *set)
{
  QString s = createDirectory("COT");
  if (! s.length())
  {
    emit statusLogMessage("Unable to create directory");
    return;
  }
  
  BarDate bd;
  if (bd.setDate(set->getData("Date")))
  {
    emit statusLogMessage("Bad date " + set->getData("Date"));
    return;
  }

  Config config;
  DbPlugin *db = config.getDbPlugin("Futures");
  if (! db)
  {
    config.closePlugin("Futures");
    return;
  }
  
  s.append("/");
  s.append(set->getData("Symbol"));
  if (db->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    config.closePlugin("Futures");
    return;
  }
  
  // verify if this chart can be updated by this plugin
  s = db->getHeaderField(DbPlugin::QuotePlugin);
  if (! s.length())
    db->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = set->getData("Symbol") + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      config.closePlugin("Futures");
      return;
    }
  }

  s = tr("Updating ");
  s.append(set->getData("Symbol"));
  emit statusLogMessage(s);

  s = db->getHeaderField(DbPlugin::Symbol);
  if (! s.length())
  {
    db->createNew();
    db->setHeaderField(DbPlugin::BarType, QString::number(BarData::Daily));
    db->setHeaderField(DbPlugin::Symbol, set->getData("Symbol"));
    db->setHeaderField(DbPlugin::Title, set->getData("Title"));
    db->setData("FuturesType", set->getData("Symbol"));
    db->setData("FuturesMonth", "Z");
  }

  Bar *bar = new Bar;
  if (bar->setDate(set->getData("Date")))
  {
    emit statusLogMessage("Bad date " + set->getData("Date"));
    delete bar;
    config.closePlugin("Futures");
    return;
  }
  bar->setOpen(set->getFloat("Non Commercial"));
  bar->setHigh(set->getFloat("Commercial"));
  bar->setLow(set->getFloat("Non Reportable"));
  bar->setClose(0);
  bar->setVolume(0);
  bar->setOI(set->getInt("Open Interest"));
  db->setBar(bar);
  
  s = set->getData("Symbol") + " " + bar->getString();
  emit dataLogMessage(s);
  
  delete bar;
  config.closePlugin("Futures");
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
  {
    timer->stop();
    op->stop();
  }
  
  emit done();
  emit statusLogMessage(tr("Canceled"));
}

void COT::prefDialog (QWidget *w)
{
  QStringList l;
  l.append("Current");
  l.append("History");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("COT Prefs"));
  dialog->createPage (tr("Details"));
  dialog->setHelpFile(helpFile);
  dialog->addComboItem(tr("Format"), tr("Details"), l, format);
  dialog->addIntItem(tr("Retry"), tr("Details"), retries, 0, 99);  
  dialog->addIntItem(tr("Timeout"), tr("Details"), timeout, 0, 99);  
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    format = dialog->getCombo(tr("Format"));
    timeout = dialog->getInt(tr("Timeout"));
    retries = dialog->getInt(tr("Retry"));
    
    saveFlag = TRUE;
    saveSettings();
  }
  
  delete dialog;
}

void COT::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/COT plugin");

  format = settings.readEntry("/Format", "Current");
  
  QString s = settings.readEntry("/Retry", "3");
  retries = s.toInt();
  
  s = settings.readEntry("/Timeout", "15");
  timeout = s.toInt();
  
  settings.endGroup();
}

void COT::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/COT plugin");
  
  settings.writeEntry("/Format", format);
  settings.writeEntry("/Retry", QString::number(retries));
  settings.writeEntry("/Timeout", QString::number(timeout));
  
  settings.endGroup();
}

//*************************************************************
//*************************************************************
//*************************************************************

QuotePlugin * createQuotePlugin ()
{
  COT *o = new COT;
  return ((QuotePlugin *) o);
}

