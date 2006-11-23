/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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
#include "Bar.h"
#include "../../../pics/newchart.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/delete.xpm"
#include "HelpWindow.h"
#include "SymbolDialog.h"
#include "CSVRuleDialog.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qsettings.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qinputdialog.h>

CSV::CSV ()
{
  pluginName = "CSV";
  delim = ",";
  helpFile = "csv.html";
  cancelFlag = FALSE;

  reloadTimer = new QTimer(this);
  connect(reloadTimer, SIGNAL(timeout()), SLOT(parse()));
  
  config.getData(Config::QuotePluginStorage, ruleDir);
  ruleDir.append("/CSV");
  QDir dir;
  if (! dir.exists(ruleDir))
  {
    if (! dir.mkdir(ruleDir, TRUE))
      qDebug("CSVDialog::could not create storage directory %s", ruleDir.latin1());
  }
  
  createMainPage();
  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));

  loadSettings();

  updateRules();
  
  resize(400, 400);
}

CSV::~CSV ()
{
  saveSettings();
}

void CSV::update ()
{
  parse();
}

void CSV::parse ()
{
  Setting rule;
  getRule(rule);
  
  if (! rule.count())
  {
    QString s(tr("Empty rule"));
    printStatusLogMessage(s);
    downloadComplete();
    return;
  }

  QString ts = "Rule";
  QString ts2, s;
  rule.getData(ts, ts2);
  if (! ts2.contains("Date:"))
  {
    QString ss(tr("Rule missing Date field"));
    printStatusLogMessage(ss);
    downloadComplete();
    return;
  }

  ts = "Delimiter";
  rule.getData(ts, s);
  if (! s.length())
  {
    QString ss(tr("Delimiter not found"));
    printStatusLogMessage(ss);
    downloadComplete();
    return;
  }
  setDelimiter(s);

  if (dateRange->isChecked())
  {
    if (sdate->date() >= edate->date() || edate->date() <= sdate->date())
    {
      QString ss(tr("Done"));
      printStatusLogMessage(ss);
      downloadComplete();
      return;
    }
  }

  QString type;
  ts = "Type";
  rule.getData(ts, type);
  if (! type.length())
  {
    QString ss(tr("Type not found"));
    printStatusLogMessage(ss);
    downloadComplete();
    return;
  }
  
  ts = "Rule";
  rule.getData(ts, ts2);
  QStringList fieldList = QStringList::split(",", ts2, FALSE);
  if (! fieldList.count())
  {
    QString ss(tr("No rule found"));
    printStatusLogMessage(ss);
    downloadComplete();
    return;
  }

  // get the directory path offset
  ts = "Directory";
  QString directory;
  rule.getData(ts, directory);
  if (! directory.length())
  {
    QString ss(tr("Directory not found"));
    printStatusLogMessage(ss);
    downloadComplete();
    return;
  }

  // get the symbol filter
  ts = "SymbolFilter";
  rule.getData(ts, ts2);
  QStringList symbolFilter = QStringList::split(",", ts2, FALSE);
  
  // check for time field and set the tickflag  
  bool tickFlag = FALSE;
  ts = "Rule";
  rule.getData(ts, ts2);
  if (ts2.contains("Time"))
    tickFlag = TRUE;
  else
  {
    if (ts2.contains("HHMMSS"))
      tickFlag = TRUE;
  }

  QStringList list;
  file->getFile(list);

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
      createDirectory(tmp, path);
      if (! path.length())
      {
        QString ss(tr("Unable to create directory"));
        printStatusLogMessage(ss);
        downloadComplete();
        f.close();
        return;
      }
    }
    else
    {
      if (! type.compare("Futures"))
      {
        QString tmp = "Futures/" + directory;
        createDirectory(tmp, path);
        if (! path.length())
        {
          QString ss(tr("Unable to create directory"));
          printStatusLogMessage(ss);
          downloadComplete();
          f.close();
          return;
        }
      }
    }

    path.append("/");

    if (symbol.length())
    {
      QString s = path;
      s.append(symbol);
      if (openDb(s, symbol, type, tickFlag))
        continue;
      QString ss = tr("Updating") + " " + symbol;
      printStatusLogMessage(ss);
    }

    while(stream.atEnd() == 0)
    {
      QString ts = stream.readLine();
      QString s;
      stripJunk(ts, s);

      QStringList l = QStringList::split(delim, s, FALSE);
      if (l.count() != fieldList.count())
      {
        qDebug("CSV::parse:File fields (%i) != rule format (%i)", l.count(), fieldList.count());
        QString ss(tr("File fields != rule format"));
        printStatusLogMessage(ss);
	continue;
      }
      
      int fieldLoop;
      bool flag = FALSE;
      Setting r;
      for (fieldLoop = 0; fieldLoop < (int) fieldList.count(); fieldLoop++)
      {
        if (fieldList[fieldLoop].contains("Date:"))
	{
          QDate dt;
          getDate(fieldList[fieldLoop], l[fieldLoop], r, dt);
          if (! dt.isValid())
	  {
	    qDebug("CSV::parse:Bad date %s", l[fieldLoop].latin1());
            QString ss(tr("Bad date"));
            printStatusLogMessage(ss);
	    flag = TRUE;
	    break;
	  }
	  
          if (dateRange->isChecked())
          {
            if (dt < sdate->date() || dt > edate->date())
	    {
	      flag = TRUE;
	      break;
	    }
          }
          ts = "Date";
          ts2 = dt.toString("yyyyMMdd");
	  r.setData(ts, ts2);
	  continue;
	}

        if (! fieldList[fieldLoop].compare("Time"))
	{
          getTime(l[fieldLoop], s);
          if (! s.length())
	  {
	    qDebug("CSV::parse:Bad time %s", l[fieldLoop].latin1());
            QString ss(tr("Bad time"));
            printStatusLogMessage(ss);
	    flag = TRUE;
	    break;
	  }
          ts = "Time";
	  r.setData(ts, s);
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
	  
          ts = "Symbol";
	  r.setData(ts, l[fieldLoop]);
	  continue;
	}
	
        if (! fieldList[fieldLoop].compare("Name"))
	{
          ts = "Name";
	  r.setData(ts, l[fieldLoop]);
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
            QString ss(tr("Bad value"));
            printStatusLogMessage(ss);
	    flag = TRUE;
	    break;
	  }
          ts = QString::number(tfloat);
	  r.setData(fieldList[fieldLoop], ts);
	  continue;
	}
	
	if (! fieldList[fieldLoop].compare("Volume") ||
	    ! fieldList[fieldLoop].compare("OI"))
	{
          if (setTFloat(l[fieldLoop], FALSE))
	  {
	    qDebug("CSV::parse:Bad %s value", fieldList[fieldLoop].latin1());
            QString ss(tr("Bad value"));
            printStatusLogMessage(ss);
	    flag = TRUE;
	    break;
	  }
          ts = QString::number(tfloat);
	  r.setData(fieldList[fieldLoop], ts);
	  continue;
	}
      }
      
      if (flag)
	continue;

      ts = "Date";
      r.getData(ts, s);
      if (! s.length())
	continue;
      
      ts = "Time";
      r.getData(ts, ts2);
      if (ts2.length())
        s.append(ts2);
      else
        s.append("000000");
	
      Bar bar;
      if (bar.setDate(s))
      {
        ts = "Date";
        r.getData(ts, ts2);
        QString ss = tr("Bad date") + " " + ts2;
        printStatusLogMessage(ss);
        continue;
      }
      bar.setTickFlag(tickFlag);
      ts = "Open";
      bar.setOpen(r.getDouble(ts));
      ts = "High";
      bar.setHigh(r.getDouble(ts));
      ts = "Low";
      bar.setLow(r.getDouble(ts));
      ts = "Close";
      bar.setClose(r.getDouble(ts));
      ts = "Volume";
      bar.setVolume(r.getDouble(ts));
      ts = "OI";
      bar.setOI(r.getInt(ts));
      
      if (! symbol.length())
      {
        ts = "Symbol";
        QString t;
        r.getData(ts, t);
	s = path;
	s.append(t);
	if (openDb(s, t, type, tickFlag))
	  continue;
	
        ts = "Name";
	r.getData(ts, s);
	if (s.length())
	  db.setHeaderField(DbPlugin::Title, s);

        db.setBar(bar);
	
        ts = "Symbol";
        r.getData(ts, ts2);
        QString ss = tr("Updating") + " " + ts2;
        printStatusLogMessage(ss);
	config.closePlugin(type);
      }
      else
      {
        ts = "Name";
	r.getData(ts, s);
	if (s.length())
	  db.setHeaderField(DbPlugin::Title, s);
        db.setBar(bar);
      }
    }

    db.close();
    f.close();
  }

  downloadComplete();
  if (cancelFlag)
  {
    cancelFlag = FALSE;
    printStatusLogMessage(stringCanceled);
  }
  else
    printStatusLogMessage(stringDone);
}

void CSV::setDelimiter (QString &d)
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

void CSV::getTime (QString &d, QString &time)
{
  time.truncate(0);
  
  if (! d.contains(":"))
  {
    if (d.length() == 6)
    {
      time = d;
      time.insert(4, ":");
      time.insert(2, ":");
      return;
    }
    else
      return;
  }
    
  QStringList l = QStringList::split(":", d, FALSE);
  if (l.count() != 3)
    return;
    
  time = l[0] + l[1];
   
  if (l[2].toInt() < 10)
    time.append("0");
  time.append(QString::number(l[2].toInt()));
}

void CSV::getDate (QString &k, QString &d, Setting &r, QDate &date)
{
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
        return;
      else
        dateString = l.join(QString::null);
      break;
    }

    if (dateString.contains("-"))
    {
      l = QStringList::split("-", dateString, FALSE);
      if (l.count() != 3)
        return;
      else
        dateString = l.join(QString::null);
      break;
    }

    if (dateString.contains("."))
    {
      l = QStringList::split(".", dateString, FALSE);
      if (l.count() != 3)
        return;
      else
        dateString = l.join(QString::null);
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
          date.setYMD(dateString.left(4).toInt(), dateString.mid(4, 2).toInt(),
          dateString.right(2).toInt());
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
          date.setYMD(dateString.left(2).toInt(), dateString.mid(2, 2).toInt(),
          dateString.right(2).toInt());
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
          date.setYMD(dateString.right(4).toInt(), dateString.left(2).toInt(),
          dateString.mid(2, 2).toInt());
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
          date.setYMD(dateString.right(2).toInt(), dateString.left(2).toInt(),
          dateString.mid(2, 2).toInt());
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
          date.setYMD(dateString.right(4).toInt(), dateString.mid(2, 2).toInt(),
          dateString.left(2).toInt());
      }
      break;
    }

    if (! k.compare("Date:MMDDYYYYHHMMSS"))
    {
      QString s;
      getTime(timeString, s);
      if (s.length())
      {
        QString ts = "Time";
        r.setData(ts, s);
      }
      else
        break;
      
      if (l.count())
        date.setYMD(l[2].toInt(), l[0].toInt(), l[1].toInt());
      else
      {
        if (dateString.length() == 8)
          date.setYMD(dateString.right(4).toInt(), dateString.left(2).toInt(),
          dateString.mid(2, 2).toInt());
      }
      
      break;
    }
    
    if (! k.compare("Date:YYYYMMDDHHMMSS"))
    {
      timeString = d.right(6);
      dateString = d.left(8);
      QString s;
      getTime(timeString, s);
      if (s.length())
      {
        QString ts = "Time";
        r.setData(ts, s);
      }
      else
        break;
      
      if (l.count())
        date.setYMD(l[0].toInt(), l[1].toInt(), l[2].toInt());
      else
      {
        if (dateString.length() == 8)
          date.setYMD(dateString.left(4).toInt(), dateString.mid(4, 2).toInt(),
          dateString.right(2).toInt());
      }
      
      break;
    }
    
    break;
  }
}

bool CSV::openDb (QString &path, QString &symbol, QString &type, bool tickFlag)
{
  if (db.openChart(path))
  {
    qDebug("CSV::openDb:can't open chart");
    QString ss("CSV::OpenDb:Could not open db.");
    printStatusLogMessage(ss);
    db.close();
    return TRUE;
  }
  
  // verify if this chart can be updated by this plugin
  QString s;
  db.getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    db.setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = symbol + " - skipping update. Source does not match destination.";
      printStatusLogMessage(s);
      db.close();
      return TRUE;
    }
  }

  db.getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    if (! type.compare("Futures"))
      db.createNew(DbPlugin::Futures);
    else
      db.createNew(DbPlugin::Stock);
    
    db.setHeaderField(DbPlugin::Symbol, symbol);
    
    s = QString::number(tickFlag);
    db.setHeaderField(DbPlugin::BarType, s);
  }
  
  return FALSE;
}

void CSV::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");

  QString s = settings.readEntry("/RuleName");
  ruleCombo->setCurrentText(s);

  s = settings.readEntry("/DateRange", "0");
  dateRange->setChecked(s.toInt());
  dateRangeChanged(s.toInt());
  
  lastPath = settings.readEntry("/lastPath", QDir::homeDirPath());
  QStringList l;
  l.append(lastPath);
  file->setFile(l);

  s = settings.readEntry("/ReloadInterval", "0");
  minutes->setValue(s.toInt());
    
  settings.endGroup();
}

void CSV::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");

  settings.writeEntry("/RuleName", ruleCombo->currentText());
  settings.writeEntry("/DateRange", QString::number(dateRange->isChecked()));
  settings.writeEntry("/ReloadInterval", minutes->text());

  QStringList l;
  file->getFile(l);
  if (l.count())
    lastPath = l[0];
  settings.writeEntry("/lastPath", lastPath);
  
  settings.endGroup();
}

void CSV::getRule (Setting &set)
{
  QString s = ruleDir +"/" + ruleCombo->currentText();
  QFile f(s);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("CSV::getRule:cannot read file.");
    return;
  }
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
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

void CSV::createMainPage ()
{
  setCaption(tr("CSV Quotes"));

  QString s = "new";
  QString s2 = tr("New Rule");
  toolbar->addButton(s, newchart, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(newRule()));
  
  s = "edit";
  s2 = tr("Edit Rule");
  toolbar->addButton(s, edit, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(editRule()));
  
  s = "delete";
  s2 = tr("Delete Rule");
  toolbar->addButton(s, deleteitem, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(deleteRule()));
  
  QLabel *label = new QLabel(tr("Rule"), baseWidget);
  grid->addWidget(label, 0, 0);

  ruleCombo = new QComboBox(baseWidget);
  updateRules();
  grid->addWidget(ruleCombo, 0, 1);
  
  label = new QLabel(tr("Input"), baseWidget);
  grid->addWidget(label, 1, 0);
  
  QStringList l;
  file = new FileButton(baseWidget, l, lastPath);
  grid->addWidget(file, 1, 1);

  label = new QLabel(tr("Symbol"), baseWidget);
  grid->addWidget(label, 2, 0);
  
  symbol = new QLineEdit(baseWidget);
  grid->addWidget(symbol, 2, 1);
  
  label = new QLabel(tr("Auto Reload"), baseWidget);
  grid->addWidget(label, 3, 0);
  
  minutes = new QSpinBox(baseWidget);
  minutes->setMinValue(0);
  minutes->setMaxValue(99);
  minutes->setLineStep(0);
  grid->addWidget(minutes, 3, 1);
  
  dateRange = new QCheckBox(tr("Select Date Range"), baseWidget);
  QObject::connect(dateRange, SIGNAL(toggled(bool)), this, SLOT(dateRangeChanged(bool)));
  grid->addWidget(dateRange, 5, 0);
  
  label = new QLabel(tr("Date Start"), baseWidget);
  grid->addWidget(label, 6, 0);
  
  sdate = new QDateEdit(QDate::currentDate(), baseWidget);
  sdate->setAutoAdvance(TRUE);
  sdate->setOrder(QDateEdit::YMD);
  grid->addWidget(sdate, 6, 1);
  
  label = new QLabel(tr("Date End"), baseWidget);
  grid->addWidget(label, 7, 0);
  
  edate = new QDateEdit(QDate::currentDate(), baseWidget);
  edate->setAutoAdvance(TRUE);
  edate->setOrder(QDateEdit::YMD);
  grid->addWidget(edate, 7, 1);

  // set the default end date
  QDate dt = QDate::currentDate();
  if (dt.dayOfWeek() == 6)
    dt = dt.addDays(-1);
  else
  {
    if (dt.dayOfWeek() == 7)
      dt = dt.addDays(-2);
  }
  edate->setDate(dt);
  sdate->setDate(dt);

//  reloadTimer->stop();
//  if (reloadInterval)
//    reloadTimer->start(60000 * reloadInterval, FALSE);
}

void CSV::newRule ()
{
  bool ok = FALSE;
  QString s = QInputDialog::getText(tr("New Rule"),
  				    tr("Enter new rule name."),
				    QLineEdit::Normal,
				    tr("NewRule"),
				    &ok,
				    this);
  if ((! ok) || (s.isNull()))
    return;
    
  // remove any bad chars
  int loop;
  QString name;
  for (loop = 0; loop < (int) s.length(); loop++)
  {
    QChar c = s.at(loop);
    if (c.isLetterOrNumber())
      name.append(c);
  }
    
  QDir dir;
  s = ruleDir + "/" + name;
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Warning"), tr("This rule already exists."));
    return;
  }
    
  CSVRuleDialog *dialog = new CSVRuleDialog(this, s);
          
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    updateRules();
  }
  
  delete dialog;
}

void CSV::editRule ()
{
  QString s("*");
  SymbolDialog *dialog = new SymbolDialog(this,
  				          ruleDir,
                                          ruleDir,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Rule To Edit"));

  int rc = dialog->exec();

  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  QStringList l = dialog->selectedFiles();
  delete dialog;
  if (! l.count())
    return;

  CSVRuleDialog *rdialog = new CSVRuleDialog(this, l[0]);
          
  rc = rdialog->exec();
  
  delete rdialog;
}

void CSV::deleteRule ()
{
  QString s("*");
  SymbolDialog *dialog = new SymbolDialog(this,
  				          ruleDir,
                                          ruleDir,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Rules To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure you want to delete this rule?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QStringList l = dialog->selectedFiles();
    delete dialog;
    if (! l.count())
      return;
    
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
      dir.remove(l[loop]);
    
    updateRules();
  }
  else
    delete dialog;
}

void CSV::dateRangeChanged (bool d)
{
  sdate->setEnabled(d);
  edate->setEnabled(d);
}

void CSV::updateRules ()
{
  QString current = ruleCombo->currentText();
  
  ruleCombo->clear();
  
  QStringList l;
  QDir dir(ruleDir);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (! fi.isDir())
      l.append(dir[loop]);
  }
  ruleCombo->insertStringList(l, -1);

  if (current.length())
    ruleCombo->setCurrentItem(l.findIndex(current));
}

void CSV::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

//**********************************************************
//**********************************************************
//**********************************************************

QuotePlugin * createQuotePlugin ()
{
  CSV *o = new CSV;
  return ((QuotePlugin *) o);
}


