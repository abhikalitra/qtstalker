/*
 *  MySQL plugin for Qtstalker stock charter
 *
 *  Copyright (C) 2003 Marco van Zwetselaar
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

#include "MySQLPlugin.h"
#include "PrefDialog.h"
#include "Setting.h"
#include "Bar.h"
#include <qdir.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qsettings.h>
#include <qmessagebox.h>
#include <qstringlist.h>


/**
 * constructor
 */
MySQLPlugin::MySQLPlugin ()
{
  // plugin defaults
  pluginName = "MySQL";
  helpFile = "mysql.html";
  cancelFlag = FALSE;
  plug = 0;

  QString plugin("Stocks");  
  plug = config.getDbPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    qDebug("MySQL::cannot load Stocks plugin");
  }
  
  // get settings persisted last time
  retrieveSettings();
}

/**
 * destructor - don't count on it to be called
 */
MySQLPlugin::~MySQLPlugin ()
{
  if (plug)
  {
    QString plugin("Stocks");  
    config.closePlugin(plugin);
  }
}

/**
 * retrieveSettings
 * 
 * Retrieve settings from Qt settings registry (~/.qt/qtstalkerrc)
 */
void MySQLPlugin::retrieveSettings() 
{
  QSettings settings;

  settings.beginGroup("/Qtstalker/MySQL plugin");
    database = settings.readEntry("/database");
    host = settings.readEntry("/host", "localhost");
    username = settings.readEntry("/username");
    password = settings.readEntry("/password");
    symbols = settings.readEntry("/symbols");
    sqlquery = settings.readEntry("/sqlquery", 
				  "SELECT day,open,high,low,close,volume FROM Quotes"
				  " WHERE symbol = '$SYMBOL$' AND day > '$LASTDAY$'"
				  " ORDER BY day");
    incremental = settings.readNumEntry("/incremental", TRUE);
  settings.endGroup();
}

/**
 * storeSettings
 * 
 * Store settings in Qt settings registry (~/.qt/qtstalkerrc)
 */
void MySQLPlugin::storeSettings()
{
  QSettings settings;
  
  settings.beginGroup("/Qtstalker/MySQL plugin");
    settings.writeEntry("/database", database);
    settings.writeEntry("/host", host);
    settings.writeEntry("/username", username);
    settings.writeEntry("/password", password);
    settings.writeEntry("/sqlquery", sqlquery);
    settings.writeEntry("/symbols", symbols);
    settings.writeEntry("/incremental", incremental);
  settings.endGroup();
}

/**
 * slot update()
 * overrides Plugin::update()
 *
 * Called by application in reaction to 'go' buttonpress
 */
void MySQLPlugin::update () 
{
  if (! plug)
    return;
    
  plug->close();
  
  QTimer::singleShot(250, this, SLOT(performUpdate()));
}

/**
 * slot performUpdate()
 *
 * Do the actual work of looping over all symbols to be updated.
 */
void MySQLPlugin::performUpdate ()
{
  if (! plug)
    return;
    
  if (openDatabase()) 
  {
    // verify and create the directory if needed  
    QString s = config.getData(Config::DataPath) + "/Stocks";
    QDir dir(s);
    if (! dir.exists() && ! dir.mkdir(s))
    {
      QString errstr = "Qtstalker::MySQL::performUpdate:unable to create /Stocks directory: ";
      errstr.append(s);
      QMessageBox::critical(0, "MySQL Plugin Error", errstr);
      emit statusLogMessage("MySQL Plugin Error: "+ errstr);
      closeDatabase();
      return;
    }
  
    s.append("/MySQL");
    if (! dir.exists() && ! dir.mkdir(s))
    {
      QString errstr = "Qtstalker::MySQL::performUpdate:unable to create /Stocks/MySQL directory: ";
      errstr.append(s);
      QMessageBox::critical(0, "MySQL Plugin Error", errstr);
      emit statusLogMessage("MySQL Plugin Error: "+ errstr);
      closeDatabase();
      return;
    }

    QStringList symbolList = QStringList::split(' ', symbols, FALSE);
    QStringList::const_iterator iter = symbolList.begin();
    
    while (iter != symbolList.end())
    {
      if (cancelFlag)
	break;
      else
      {
        QString t = *iter++;
        updateSymbol(t);
      }
    }

    closeDatabase();
    
    // if we get here, things went well, so we persist our settings
    storeSettings();
  }

  emit done();
  if (cancelFlag)
  {
    cancelFlag = FALSE;
    emit statusLogMessage(tr("Update cancelled."));
  }
  else
    emit statusLogMessage(tr("Done"));
}

/**
 * updateSymbol(symbol)
 *
 * Update the quotes for given stock.
 */
void MySQLPlugin::updateSymbol(QString &symbol) 
{
  emit statusLogMessage("Updating " + symbol);

  // create the new chart
  QString chartpath = config.getData(Config::DataPath) + "/Stocks/MySQL/" + symbol;
  
  if (plug->openChart(chartpath))
  {
    emit statusLogMessage("Qtstalker::MySQL::updateSymbol:Could not open db.");
    return;
  }
  
  // verify if this chart can be updated by this plugin
  QString s;
  plug->getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = symbol + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      plug->close();
      return;
    }
  }
  
  plug->getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    plug->createNew();
    plug->setHeaderField(DbPlugin::Symbol, symbol);
    plug->setHeaderField(DbPlugin::Title, symbol);
  }

  QDate lastdate;

  if (incremental == TRUE) 
  {
    Bar *bar = plug->getLastBar();
    if (bar)
    {
      lastdate = bar->getDate().getDate();
      delete bar;
    }
  }
 
  if (! lastdate.isValid()) 
    lastdate.setYMD(1800, 1, 1);
  
  QString sql = sqlquery;
  sql.replace("$SYMBOL$", symbol);
  sql.replace("$LASTDAY$", lastdate.toString(Qt::ISODate));

  doQuery(sql);
  
  plug->close();
}

/**
 * openDatabase()
 */
bool MySQLPlugin::openDatabase()
{
  if (!(mysql_init(&mysql) 
      && mysql_real_connect(&mysql,
	host, 
	username, 
	password,
	database, 
	0     /* port */, 
	NULL  /* socket */, 
	0     /* flags */)))
  {
    QString errstr = "Could not connect to database.\n";
    errstr.append(mysql_error(&mysql));
    QMessageBox::critical(0, "Database error", errstr); 
    emit statusLogMessage("Database error: " + errstr);
    return false;
  }
  
  return true;
}

/**
 * closeDatabase()
 */
void MySQLPlugin::closeDatabase()
{
  mysql_close(&mysql);
}  

/**
 * doQuery(sql,db)
 * 
 * Perform sql query and store results in Qtstalker db
 */
void MySQLPlugin::doQuery (QString &sql)
{
  MYSQL_RES *res;
 
  // qDebug(sql);

  if (mysql_query (&mysql, sql) == 0 
      && (res = mysql_store_result(&mysql)) != NULL) 
  {
    MYSQL_ROW row;
    bool with_oi = mysql_num_fields(res) == 7;

    while ((row = mysql_fetch_row(res)) != NULL) 
    {
      QString d = row[0];
      d = d.remove('-');
      d.append("000000");
      
      Bar bar;
      if (bar.setDate(d))
      {
        emit statusLogMessage("Bad date " + d);
        continue;
      }
      
      QString open = row[1];
      QString high = row[2];
      QString low = row[3];
      QString close = row[4];
      QString volume = row[5];
      QString oi("0");
      
      if (with_oi)
        oi = row[6];
      
      bar.setOpen(open.toDouble());
      bar.setHigh(high.toDouble());
      bar.setLow(low.toDouble());
      bar.setClose(close.toDouble());
      bar.setVolume(volume.toDouble());
      bar.setOI(oi.toInt());
      plug->setBar(bar);
    }
   
    mysql_free_result(res);
  }
  else 
  {
    QString errstr = "Database query failed.\n";
    errstr.append(mysql_error(&mysql));
    QMessageBox::critical(0, "Database Query problem", errstr);
    emit statusLogMessage("Database Query problem: " + errstr);
  }
}

// show the prefs dialog to edit settings
void MySQLPlugin::prefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("MySQL Prefs"));
  QString s = tr("Details");
  dialog->createPage (s);
  dialog->setHelpFile(helpFile);
  QString s2 = tr("Database");
  dialog->addTextItem(s2, s, database);
  s2 = tr("Host");
  dialog->addTextItem(s2, s, host);
  s2 = tr("Username");
  dialog->addTextItem(s2, s, username);
  s2 = tr("Password");
  dialog->addTextItem(s2, s, password);
  s2 = tr("SQL Query");
  dialog->addTextItem(s2, s, sqlquery);
  s2 = tr("Symbols");
  dialog->addTextItem(s2, s, symbols);
  s2 = tr("Incremental");
  dialog->addCheckItem(s2, s, incremental);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    s = tr("Database");
    database = dialog->getText(s);
    s = tr("Host");
    host = dialog->getText(s);
    s = tr("Username");
    username = dialog->getText(s);
    s = tr("Password");
    password = dialog->getText(s);
    s = tr("SQL Query");
    sqlquery = dialog->getText(s);
    s = tr("Symbols");
    symbols = dialog->getText(s);
    s = tr("Incremental");
    incremental = dialog->getCheck(s);
  }
  
  delete dialog;
}

// user has cancelled update
void MySQLPlugin::cancelUpdate ()
{
  cancelFlag = TRUE;
}

/**
 * create()
 *
 * Return new instance of this plugin.
 */
QuotePlugin * createQuotePlugin ()
{
  MySQLPlugin *o = new MySQLPlugin;
  return ((QuotePlugin *) o);
}


