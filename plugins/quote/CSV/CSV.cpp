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
  Setting *rule = getRule();
  if (! rule->count())
  {
    emit statusLogMessage("Empty rule");
    emit done();
    delete rule;
    return;
  }

  if (! rule->getData("Rule").contains("Date:"))
  {
    emit statusLogMessage("Rule missing Date field");
    emit done();
    delete rule;
    return;
  }
    
  setDelimiter(rule->getData("Delimiter"));

  if (dateFlag)
  {
    if (sdate >= edate || edate <= sdate)
    {
      emit statusLogMessage(tr("Done"));
      emit done();
      delete rule;
      return;
    }
  }

  QString type = rule->getData("Type");
  QStringList fieldList = QStringList::split(",", rule->getData("Rule"), FALSE);
  
  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
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
    if (! type.compare("Stock"))
    {
      path = createDirectory("Stocks");
      if (! path.length())
      {
        emit statusLogMessage("Unable to create stocks directory");
        emit done();
        f.close();
	delete rule;
        return;
      }
    }
    else
    {
      if (! type.compare("Futures"))
      {
        path = createDirectory("Futures");
        if (! path.length())
	{
          emit statusLogMessage("Unable to create futures directory");
          emit done();
          f.close();
	  delete rule;
          return;
        }
	
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
	  delete rule;
          return;
	}

	QString s = "Futures/";
	s.append(fd.getSymbol());
        path = createDirectory(s);
        if (! path.length())
        {
          emit statusLogMessage("Unable to create futures symbol directory");
          emit done();
          f.close();
	  delete rule;
          return;
        }
      }
    }

    path.append("/");

    if (symbol.length())
    {
      QString s = path;
      s.append(symbol);
      openDb(s, symbol, type);
      emit statusLogMessage("Updating " + symbol);
    }

    while(stream.atEnd() == 0)
    {
      QString s = stream.readLine();
      s = stripJunk(s);

      QStringList l = QStringList::split(delim, s, FALSE);
      if (l.count() != fieldList.count())
      {
        qDebug("CSV::parse:File fields (%i) != rule format (%i)", fieldList.count(), l.count());
        emit statusLogMessage("File fields != rule format");
	continue;
      }
      
      int fieldLoop;
      bool flag = FALSE;
      Setting *r = new Setting;
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
	  r->setData("Date", dt.toString("yyyyMMdd"));
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
	  r->setData("Time", s);
	  continue;
	}
	
        if (! fieldList[fieldLoop].compare("Symbol"))
	{
	  r->setData("Symbol", l[fieldLoop]);
	  continue;
	}
	
        if (! fieldList[fieldLoop].compare("Open") ||
	    ! fieldList[fieldLoop].compare("High") ||
	    ! fieldList[fieldLoop].compare("Low") ||
	    ! fieldList[fieldLoop].compare("Close") ||
	    ! fieldList[fieldLoop].compare("Volume") ||
	    ! fieldList[fieldLoop].compare("OI"))
	{
          if (setTFloat(l[fieldLoop]))
	  {
	    qDebug("CSV::parse:Bad %s value", fieldList[fieldLoop].latin1());
            emit statusLogMessage("Bad value");
	    flag = TRUE;
	    break;
	  }
	  r->setData(fieldList[fieldLoop], QString::number(tfloat));
	  continue;
	}
      }
      
      if (flag)
      {
        delete r;
	continue;
      }      

      s = r->getData("Date");
      if (! s.length())
      {
	delete r;
	continue;
      }
      if (r->getData("Time"))
        s.append(r->getData("Time"));
      else
        s.append("000000");
	
      Bar *bar = new Bar;
      if (bar->setDate(s))
      {
        emit statusLogMessage("Bad date " + r->getData("Date"));
        delete r;
	delete bar;
        continue;
      }
      bar->setOpen(r->getData("Open").toDouble());
      bar->setHigh(r->getData("High").toDouble());
      bar->setLow(r->getData("Low").toDouble());
      bar->setClose(r->getData("Close").toDouble());
      bar->setVolume(r->getData("Volume").toDouble());
      bar->setOI(r->getData("OI").toInt());
      
      if (! symbol.length())
      {
	s = path;
	s.append(r->getData("Symbol"));
	openDb(s, r->getData("Symbol"), type);
        db->setBar(bar);
	emit dataLogMessage(r->getData("Symbol") + " " + r->getString());
        emit statusLogMessage("Updating " + r->getData("Symbol"));
        delete db;
	db = 0;
      }
      else
      {
        db->setBar(bar);
	emit dataLogMessage(symbol + " " + r->getString());
      }

      delete r;
      delete bar;
    }

    if (db)
    {
      delete db;
      db = 0;
    }
    f.close();
  }

  delete rule;
  
  emit statusLogMessage(tr("Done"));
  emit done();
}

void CSV::setDelimiter (QString d)
{
  if (! d.compare(tr("Comma")))
  {
    delim = ",";
    return;
  }

  if (! d.compare(tr("Tab")))
  {
    delim = "	";
    return;
  }

  if (! d.compare(tr("Space")))
  {
    delim = " ";
    return;
  }
}

QString CSV::getTime (QString d)
{
  QString time;
  
  if (! d.contains(":"))
    return time;

  QStringList l = QStringList::split(":", d, FALSE);
  if (l.count() != 3)
    return time;
    
  time = l[0] + l[1];
   
  if (l[2].toInt() < 10)
    time.append("0");
  time.append(QString::number(l[2].toInt()));
    
  return time;    
}

QDate CSV::getDate (QString k, QString d, Setting *r)
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
        r->setData("Time", s);
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

void CSV::openDb (QString path, QString symbol, QString type)
{
  db = new ChartDb;
  if (! type.compare("Futures"))
    db->setPlugin("Futures");
  else
    db->setPlugin("Stocks");
  if (db->openChart(path))
  {
    emit statusLogMessage("Could not open db.");
    delete db;
    db = 0;
    return;
  }

  QString s = db->getData("Symbol");
  if (! s.length())
  {
    Setting *set = new Setting;
    set->setData("BarType", QString::number(BarData::Daily));
    set->setData("Symbol", symbol);
    set->setData("Title", symbol);
    db->saveDbDefaults(set);
    
    if (! type.compare("Futures"))
    {
      set->setData("Title", fd.getName());
      set->setData("FuturesType", fd.getSymbol());
      set->setData("FuturesMonth", futuresMonth);
      db->saveDbDefaults(set);
    }
    
    delete set;
  }
}

void CSV::prefDialog (QWidget *w)
{
  CSVDialog *dialog = new CSVDialog(w, helpFile);
  dialog->setCaption(tr("CSV Prefs"));
  dialog->setStartDate(sdate);
  dialog->setEndDate(edate);
  dialog->setDateRange(dateFlag);
  dialog->setRuleName(ruleName);
          
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    list = dialog->getFiles();
    symbolOveride = dialog->getSymbol();
    ruleName = dialog->getRuleName();
    sdate = dialog->getStartDate();
    edate = dialog->getEndDate();
    dateFlag = dialog->getDateRange();
    
    saveFlag = TRUE;
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
  
  settings.endGroup();
}

Setting * CSV::getRule ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");
  QStringList l = QStringList::split(",", settings.readEntry("/RuleList"), FALSE);
  
  Setting *set = new Setting;
  QString s = "/Rule_" + ruleName;
  set->parse(settings.readEntry(s));
    
  settings.endGroup();
  return set;
}


//**********************************************************
//**********************************************************
//**********************************************************

QuotePlugin * createQuotePlugin ()
{
  CSV *o = new CSV;
  return ((QuotePlugin *) o);
}


