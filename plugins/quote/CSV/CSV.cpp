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

#include "CSV.h"
#include "CSVDialog.h"
#include "Bar.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qdir.h>
#include <qsettings.h>


CSV::CSV ()
{
  pluginName = "CSV";
  delim = ",";
  db = 0;
  dateFlag = FALSE;
  helpFile = "csv.html";
  cancelFlag = FALSE;
  reloadInterval = 0;
  
  edate = QDateTime::currentDateTime();
  if (edate.date().dayOfWeek() == 6)
    edate = edate.addDays(-1);
  else
  {
    if (edate.date().dayOfWeek() == 7)
      edate = edate.addDays(-2);
  }

  sdate = QDateTime::currentDateTime();
  sdate = sdate.addDays(-1);
  if (sdate.date().dayOfWeek() == 6)
    sdate = sdate.addDays(-1);
  else
  {
    if (sdate.date().dayOfWeek() == 7)
      sdate = sdate.addDays(-2);
  }
  
  reloadTimer = new QTimer(this);
  connect(reloadTimer, SIGNAL(timeout()), SLOT(parse()));
  
  ruleDir = config.getData(Config::QuotePluginStorage) + "/CSV";
  
  loadSettings();
}

CSV::~CSV ()
{
}

void CSV::update ()
{
  QTimer::singleShot(250, this, SLOT(parse()));
}

void CSV::parse ()
{
  Setting rule;
  getRule(rule);
  
  if (! rule.count())
  {
    emit statusLogMessage("Empty rule");
    emit done();
    return;
  }

  if (! rule.getData("Rule").contains("Date:"))
  {
    emit statusLogMessage("Rule missing Date field");
    emit done();
    return;
  }

  QString s = rule.getData("Delimiter");
  if (! s.length())
  {
    emit statusLogMessage(tr("Delimiter not found"));
    emit done();
    return;
  }
  setDelimiter(s);

  if (dateFlag)
  {
    if (sdate >= edate || edate <= sdate)
    {
      emit statusLogMessage(tr("Done"));
      emit done();
      return;
    }
  }

  QString type = rule.getData("Type");
  if (! type.length())
  {
    emit statusLogMessage(tr("Type not found"));
    emit done();
    return;
  }
  
  QStringList fieldList = QStringList::split(",", rule.getData("Rule"), FALSE);
  if (! fieldList.count())
  {
    emit statusLogMessage(tr("No rule found"));
    emit done();
    return;
  }

  // get the directory path offset
  QString directory = rule.getData("Directory");
  if (! directory.length())
  {
    emit statusLogMessage(tr("Directory not found"));
    emit done();
    return;
  }
  
  // get the symbol filter
  QStringList symbolFilter = QStringList::split(",", rule.getData("SymbolFilter"), FALSE);
  
  // check for time field and set the tickflag  
  bool tickFlag = FALSE;
  if (rule.getData("Rule").contains("Time"))
    tickFlag = TRUE;
  else
  {
    if (rule.getData("Rule").contains("HHMMSS"))
      tickFlag = TRUE;
  }

  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    if (cancelFlag)
      break;
      
    QFile f(list[loop]);
    if (! f.open(IO_ReadOnly))
      continue;
    QTextStream stream(&f);

    QString symbol = symbolOveride;
    if (! symbol.length() && fieldList.findIndex("Symbol") == -1)
    {
      QStringList l = QStringList::split("/", list[loop], FALSE);
      symbol = l[l.count() - 1];

      if (symbol.right(4).contains(".txt"))
        symbol.truncate(symbol.length() - 4);

      if (symbol.right(4).contains(".TXT"))
        symbol.truncate(symbol.length() - 4);
      
      while (symbol.contains("_"))
        symbol = symbol.remove(symbol.find("_", 0, TRUE), 1);
    }

    QString path;
    if (! type.compare("Stocks"))
    {
      QString tmp = "Stocks/" + directory;
      path = createDirectory(tmp);
      if (! path.length())
      {
        emit statusLogMessage("Unable to create directory");
        emit done();
        f.close();
        return;
      }
    }
    else
    {
      if (! type.compare("Futures"))
      {
        QString tmp = "Futures/" + directory;
        path = createDirectory(tmp);
        if (! path.length())
        {
          emit statusLogMessage("Unable to create directory");
          emit done();
          f.close();
          return;
        }

/*		
	if (symbol.length() == 7)
	  futuresSymbol = symbol.left(2);
	else
	  futuresSymbol = symbol.left(1);
	futuresMonth = symbol.right(1);

	if (fd.setSymbol(futuresSymbol))
	{
          emit statusLogMessage("Bad futures symbol");
          emit done();
          f.close();
          return;
	}
*/	
      }
    }

    path.append("/");

    if (symbol.length())
    {
      QString s = path;
      s.append(symbol);
      if (openDb(s, symbol, type, tickFlag))
        continue;
      emit statusLogMessage("Updating " + symbol);
    }

    while(stream.atEnd() == 0)
    {
      QString s = stream.readLine();
      s = stripJunk(s);

      QStringList l = QStringList::split(delim, s, FALSE);
      if (l.count() != fieldList.count())
      {
        qDebug("CSV::parse:File fields (%i) != rule format (%i)", l.count(), fieldList.count());
        emit statusLogMessage("File fields != rule format");
	continue;
      }
      
      int fieldLoop;
      bool flag = FALSE;
      Setting r;
      for (fieldLoop = 0; fieldLoop < (int) fieldList.count(); fieldLoop++)
      {
        if (fieldList[fieldLoop].contains("Date:"))
	{
          QDate dt = getDate(fieldList[fieldLoop], l[fieldLoop], r);
          if (! dt.isValid())
	  {
	    qDebug("CSV::parse:Bad date %s", l[fieldLoop].latin1());
            emit statusLogMessage("Bad date");
	    flag = TRUE;
	    break;
	  }
	  
          if (dateFlag)
          {
            if (dt < sdate.date() || dt > edate.date())
	    {
	      flag = TRUE;
	      break;
	    }
          }
	  r.setData("Date", dt.toString("yyyyMMdd"));
	  continue;
	}

        if (! fieldList[fieldLoop].compare("Time"))
	{
          s = getTime(l[fieldLoop]);
          if (! s.length())
	  {
	    qDebug("CSV::parse:Bad time %s", l[fieldLoop].latin1());
            emit statusLogMessage("Bad time");
	    flag = TRUE;
	    break;
	  }
	  r.setData("Time", s);
	  continue;
	}
	
        if (! fieldList[fieldLoop].compare("Symbol"))
	{
	  if (symbolFilter.count())
	  {
	    if (symbolFilter.findIndex(l[fieldLoop]) == -1)
	    {
	      flag = TRUE;
	      break;
	    }
	  }
	  
	  r.setData("Symbol", l[fieldLoop]);
	  continue;
	}
	
        if (! fieldList[fieldLoop].compare("Name"))
	{
	  r.setData("Name", l[fieldLoop]);
	  continue;
	}
        
	if (! fieldList[fieldLoop].compare("Open") ||
	    ! fieldList[fieldLoop].compare("High") ||
	    ! fieldList[fieldLoop].compare("Low") ||
	    ! fieldList[fieldLoop].compare("Close"))
	{
          if (setTFloat(l[fieldLoop], TRUE))
	  {
	    qDebug("CSV::parse:Bad %s value", fieldList[fieldLoop].latin1());
            emit statusLogMessage("Bad value");
	    flag = TRUE;
	    break;
	  }
	  r.setData(fieldList[fieldLoop], QString::number(tfloat));
	  continue;
	}
	
	if (! fieldList[fieldLoop].compare("Volume") ||
	    ! fieldList[fieldLoop].compare("OI"))
	{
          if (setTFloat(l[fieldLoop], FALSE))
	  {
	    qDebug("CSV::parse:Bad %s value", fieldList[fieldLoop].latin1());
            emit statusLogMessage("Bad value");
	    flag = TRUE;
	    break;
	  }
	  r.setData(fieldList[fieldLoop], QString::number(tfloat));
	  continue;
	}
      }
      
      if (flag)
	continue;

      s = r.getData("Date");
      if (! s.length())
	continue;
      
      if (r.getData("Time"))
        s.append(r.getData("Time"));
      else
        s.append("000000");
	
      Bar bar;
      if (bar.setDate(s))
      {
        emit statusLogMessage("Bad date " + r.getData("Date"));
        continue;
      }
      bar.setTickFlag(tickFlag);
      bar.setOpen(r.getData("Open").toDouble());
      bar.setHigh(r.getData("High").toDouble());
      bar.setLow(r.getData("Low").toDouble());
      bar.setClose(r.getData("Close").toDouble());
      bar.setVolume(r.getData("Volume").toDouble());
      bar.setOI(r.getData("OI").toInt());
      
      if (! symbol.length())
      {
	s = path;
	s.append(r.getData("Symbol"));
	if (openDb(s, r.getData("Symbol"), type, tickFlag))
	  continue;
	
	s = r.getData("Name");
	if (s.length())
	  db->setHeaderField(DbPlugin::Title, s);

        db->setBar(bar);
	
	r.getString(s);
	emit dataLogMessage(r.getData("Symbol") + " " + s);
        emit statusLogMessage("Updating " + r.getData("Symbol"));
	config.closePlugin(type);
	db = 0;
      }
      else
      {
	s = r.getData("Name");
	if (s.length())
	  db->setHeaderField(DbPlugin::Title, s);
        db->setBar(bar);
	
	r.getString(s);
	emit dataLogMessage(symbol + " " + s);
      }
    }

    if (db)
    {
      config.closePlugin(type);
      db = 0;
    }
    f.close();
  }

  emit done();
  if (cancelFlag)
  {
    cancelFlag = FALSE;
    emit statusLogMessage(tr("Update cancelled"));
  }
  else
    emit statusLogMessage(tr("Done"));
}

void CSV::setDelimiter (QString d)
{
  if (! d.compare("Comma"))
  {
    delim = ",";
    return;
  }

  if (! d.compare("Tab"))
  {
    delim = "	";
    return;
  }

  if (! d.compare("Space"))
  {
    delim = " ";
    return;
  }

  if (! d.compare("Semicolon"))
  {
    delim = ";";
    return;
  }
}

QString CSV::getTime (QString d)
{
  QString time;
  
  if (! d.contains(":"))
  {
    if (d.length() == 6)
    {
      time = d;
      time.insert(4, ":");
      time.insert(2, ":");
      return time;
    }
    else
      return time;
  }
    
  QStringList l = QStringList::split(":", d, FALSE);
  if (l.count() != 3)
    return time;
    
  time = l[0] + l[1];
   
  if (l[2].toInt() < 10)
    time.append("0");
  time.append(QString::number(l[2].toInt()));
    
  return time;    
}

QDate CSV::getDate (QString k, QString d, Setting &r)
{
  QDate date;
  QStringList l;
  QString dateString = d;
  QString timeString;
  
  if (d.contains(" "))
  {
    QStringList l2 = QStringList::split(" ", d, FALSE);
    dateString = l2[0];
    timeString = l2[1];
  }

  while (1)
  {
    if (dateString.contains("/"))
    {
      l = QStringList::split("/", dateString, FALSE);
      if (l.count() != 3)
        return date;
      break;
    }

    if (dateString.contains("-"))
    {
      l = QStringList::split("-", dateString, FALSE);
      if (l.count() != 3)
        return date;
      break;
    }

    if (dateString.contains("."))
    {
      l = QStringList::split(".", dateString, FALSE);
      if (l.count() != 3)
        return date;
      break;
    }

    break;
  }

  while (1)
  {
    if (! k.compare("Date:YYYYMMDD"))
    {
      if (l.count())
        date.setYMD(l[0].toInt(), l[1].toInt(), l[2].toInt());
      else
      {
        if (dateString.length() == 8)
          date.setYMD(dateString.left(4).toInt(), dateString.mid(4, 2).toInt(), dateString.right(2).toInt());
      }
      break;
    }

    if (! k.compare("Date:YYMMDD"))
    {
      if (l.count())
        date.setYMD(l[0].toInt(), l[1].toInt(), l[2].toInt());
      else
      {
        if (dateString.length() == 6)
          date.setYMD(dateString.left(2).toInt(), dateString.mid(2, 2).toInt(), dateString.right(2).toInt());
      }
      break;
    }

    if (! k.compare("Date:MMDDYYYY"))
    {
      if (l.count())
        date.setYMD(l[2].toInt(), l[0].toInt(), l[1].toInt());
      else
      {
        if (dateString.length() == 8)
          date.setYMD(dateString.right(4).toInt(), dateString.left(2).toInt(), dateString.mid(2, 2).toInt());
      }
      break;
    }

    if (! k.compare("Date:MMDDYY"))
    {
      if (l.count())
        date.setYMD(l[2].toInt(), l[0].toInt(), l[1].toInt());
      else
      {
        if (dateString.length() == 6)
          date.setYMD(dateString.right(2).toInt(), dateString.left(2).toInt(), dateString.mid(2, 2).toInt());
      }
      break;
    }

    if (! k.compare("Date:DDMMYYYY"))
    {
      if (l.count())
        date.setYMD(l[2].toInt(), l[1].toInt(), l[0].toInt());
      else
      {
        if (dateString.length() == 8)
          date.setYMD(dateString.right(4).toInt(), dateString.mid(2, 2).toInt(), dateString.left(2).toInt());
      }
      break;
    }

    if (! k.compare("Date:MMDDYYYYHHMMSS"))
    {
      QString s = getTime(timeString);
      if (s.length())
        r.setData("Time", s);
      else
        break;
      
      if (l.count())
        date.setYMD(l[2].toInt(), l[0].toInt(), l[1].toInt());
      else
      {
        if (dateString.length() == 8)
          date.setYMD(dateString.right(4).toInt(), dateString.left(2).toInt(), dateString.mid(2, 2).toInt());
      }
      
      break;
    }
    
    break;
  }

  return date;
}

bool CSV::openDb (QString path, QString symbol, QString type, bool tickFlag)
{
  db = config.getDbPlugin(type);
  if (! db)
  {
    qDebug("CSV::openDb:can't open plugin");
    config.closePlugin(type);
    return TRUE;
  }

  if (db->openChart(path))
  {
    qDebug("CSV::openDb:can't open chart");
    emit statusLogMessage("CSV::OpenDb:Could not open db.");
    config.closePlugin(type);
    db = 0;
    return TRUE;
  }
  
  // verify if this chart can be updated by this plugin
  QString s;
  db->getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    db->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = symbol + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      config.closePlugin(type);
      db = 0;
      return TRUE;
    }
  }

  db->getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    db->createNew();
    
    db->setHeaderField(DbPlugin::Symbol, symbol);
    db->setHeaderField(DbPlugin::Title, symbol);
    
    s = QString::number(tickFlag);
    db->setHeaderField(DbPlugin::BarType, s);
    
    if (! type.compare("Futures"))
    {
//      s = fd.getName();
//      db->setHeaderField(DbPlugin::Title, s);
      
//      QString s2 = fd.getSymbol();
//      s = "FuturesType";
//      db->setData(s, s2);
      
//      s = "FuturesMonth";
//      db->setData(s, futuresMonth);
    }
  }
  
  return FALSE;
}

void CSV::prefDialog (QWidget *w)
{
  CSVDialog *dialog = new CSVDialog(w, helpFile, lastPath);
  dialog->setCaption(tr("CSV Prefs"));
  dialog->setStartDate(sdate);
  dialog->setEndDate(edate);
  dialog->setDateRange(dateFlag);
  dialog->setRuleName(ruleName);
  dialog->setFiles(list);
  dialog->setReloadInterval(reloadInterval);
          
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    list = dialog->getFiles();
    symbolOveride = dialog->getSymbol();
    ruleName = dialog->getRuleName();
    sdate = dialog->getStartDate();
    edate = dialog->getEndDate();
    dateFlag = dialog->getDateRange();
    
    reloadInterval = dialog->getReloadInterval();
    reloadTimer->stop();
    if (reloadInterval)
      reloadTimer->start(60000 * reloadInterval, FALSE);
    
    saveFlag = TRUE;
    
    if (list.count())
    {
      QFileInfo fi(list[0]);
      lastPath = fi.dirPath(TRUE);
      saveSettings();
    }
  }
  
  delete dialog;
}

void CSV::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");

  ruleName = settings.readEntry("/RuleName");

  QString s = settings.readEntry("/DateRange", "0");
  dateFlag = s.toInt();
  
  lastPath = settings.readEntry("/lastPath", QDir::homeDirPath());

  s = settings.readEntry("/ReloadInterval", "0");
  reloadInterval = s.toInt();
    
  // tmp function to convert CSV rule files, remove after a few more versions are released
  QStringList l = QStringList::split(",", settings.readEntry("/RuleList"), FALSE);
  if (l.count())
  {
    Config config;
    QDir dir;
    QString ruleDir = config.getData(Config::QuotePluginStorage) + "/CSV";
    if (! dir.exists(ruleDir))
    {
      if (! dir.mkdir(ruleDir, TRUE))
      {
        qDebug("CSV::loadSettings:could not create storage directory %s", ruleDir.latin1());
        settings.endGroup();
	return;
      }
    }
    
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      QString s = "/Rule_" + l[loop];
      if (! s.length())
        continue;
	
      Setting set;
      QString t = settings.readEntry(s);
      set.parse(t);
      
      QString s2 = ruleDir + "/" + l[loop];
      QFile f(s2);
      if (! f.open(IO_WriteOnly))
      {
        qDebug("CSV::loadSettings:cannot save rule.");
        continue;
      }
      QTextStream stream(&f);
  
      QStringList l2;
      set.getKeyList(l2);
      int loop2;
      for (loop2 = 0; loop2 < (int) l2.count(); loop2++)
        stream << l2[loop2] << "=" << set.getData(l2[loop2]) << "\n";
	
      f.close();  
      
      settings.removeEntry(s);
    }
  
    settings.removeEntry("/RuleList");
  }
  
  settings.endGroup();
}

void CSV::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");

  settings.writeEntry("/RuleName", ruleName);
  settings.writeEntry("/DateRange", QString::number(dateFlag));
  settings.writeEntry("/lastPath", lastPath);
  settings.writeEntry("/ReloadInterval", QString::number(reloadInterval));
  
  settings.endGroup();
}

void CSV::getRule (Setting &set)
{
  QString s = ruleDir +"/" + ruleName;
  QFile f(s);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("CSV::getRule:cannot read file.");
    return;
  }
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
      
    QStringList l = QStringList::split("=", s, FALSE);
    if (l.count() != 2)
      continue;
      
    set.setData(l[0], l[1]);
  }
  
  f.close();
}

void CSV::cancelUpdate ()
{
  cancelFlag = TRUE;
}

//**********************************************************
//**********************************************************
//**********************************************************

QuotePlugin * createQuotePlugin ()
{
  CSV *o = new CSV;
  return ((QuotePlugin *) o);
}


