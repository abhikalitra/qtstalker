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
#include <qdir.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qsettings.h>
#include <qmessagebox.h>

/**
 * constructor
 */
MySQLPlugin::MySQLPlugin ()
{
  // plugin defaults
  pluginName = "MySQL";
  about = tr("Imports quotes from a MySQL database.\n");
  createFlag = TRUE;

  // get settings persisted last time
  retrieveSettings();
}

/**
 * destructor - don't count on it to be called
 */
MySQLPlugin::~MySQLPlugin ()
{
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
    set(tr("Database"), settings.readEntry("/database"), Setting::Text);
    set(tr("Host"), settings.readEntry("/host", "localhost"), Setting::Text);
    set(tr("Username"), settings.readEntry("/username"), Setting::Text);
    set(tr("Password"), settings.readEntry("/password"), Setting::Text);
    set(tr("Symbols"), settings.readEntry("/symbols"), Setting::Text);
    set(tr("SQL Query"), settings.readEntry("/sqlquery", 
	"SELECT day,open,high,low,close,volume FROM Quotes"
	" WHERE symbol = '$SYMBOL$' AND day > '$LASTDAY$'"
	" ORDER BY day"), Setting::Text);
    set(tr("Incremental"), settings.readEntry("/incremental", tr("True")), Setting::Bool);
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
    settings.writeEntry("/database", getData(tr("Database")));
    settings.writeEntry("/host", getData(tr("Host")));
    settings.writeEntry("/username", getData(tr("Username")));
    settings.writeEntry("/password", getData(tr("Password")));
    settings.writeEntry("/sqlquery", getData(tr("SQL Query")));
    settings.writeEntry("/symbols", getData(tr("Symbols")));
    settings.writeEntry("/incremental", getData(tr("Incremental")));
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
  QTimer::singleShot(250, this, SLOT(performUpdate()));
}

/**
 * slot performUpdate()
 *
 * Do the actual work of looping over all symbols to be updated.
 */
void MySQLPlugin::performUpdate ()
{
  if (openDatabase()) 
  {
    QStringList symbols = QStringList::split(' ', getData(tr("Symbols")));
    QStringList::const_iterator iter = symbols.begin();

    while (iter != symbols.end())
      updateSymbol(*iter++);

    closeDatabase();
  
    // if we get here, things went well, so we persist our settings
    storeSettings();
  }

  emit done();
}

/**
 * updateSymbol(symbol)
 *
 * Update the quotes for given stock.
 */
void MySQLPlugin::updateSymbol(QString symbol) 
{
  emit message("Updating " + symbol);
  
  QString chartpath = dataPath;
  chartpath.append("/Stocks/");
  chartpath.append(symbol);

  ChartDb db;
  db.openChart(chartpath);
  
  QDate lastdate;

  if (getData(tr("Incremental")) == tr("True")) 
  {
    QString ld = db.getDetails()->getDateTime("Last Date");
    if (ld.length() != 0) 
      lastdate = QDate::fromString(ld, Qt::ISODate);
  }
 
  if (!lastdate.isValid()) 
    lastdate.setYMD(1800, 1, 1);
  
  QString sql = getData(tr("SQL Query"));
  sql.replace("$SYMBOL$", symbol);
  sql.replace("$LASTDAY$", lastdate.toString(Qt::ISODate));

  doQuery(sql, db);
}


/**
 * openDatabase()
 */
bool MySQLPlugin::openDatabase()
{
  if (!(mysql_init(&mysql) 
      && mysql_real_connect(&mysql,
	getData(tr("Host")), 
	getData(tr("Username")), 
	getData(tr("Password")),
	getData(tr("Database")), 
	0     /* port */, 
	NULL  /* socket */, 
	0     /* flags */)))
  {
    QString errstr = "Could not connect to database.\n";
    errstr.append(mysql_error(&mysql));
    QMessageBox::critical(0, "Database error", errstr); 
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
void MySQLPlugin::doQuery (QString sql, ChartDb& db)
{
  MYSQL_RES *res;
 
  // qDebug(sql);

  if (mysql_query (&mysql, sql) == 0 
      && (res = mysql_store_result(&mysql)) != NULL) 
  {
    MYSQL_ROW row;
    Setting rec;
    bool with_oi = mysql_num_fields(res) == 7;

    while ((row = mysql_fetch_row(res)) != NULL) 
    {
      QString d = row[0];
      d = d.remove('-');
      d.append("000000");

      rec.set("Date", d, Setting::Date);
      rec.set("Open", row[1], Setting::Float);
      rec.set("High", row[2], Setting::Float);
      rec.set("Low", row[3], Setting::Float);
      rec.set("Close", row[4], Setting::Float);
      rec.set("Volume", row[5], Setting::Float);
      if (with_oi)
	rec.set("Open Interest", row[6], Setting::Float);

      db.setRecord(&rec);
    }
   
    mysql_free_result(res);
  }
  else 
  {
    QString errstr = "Database query failed.\n";
    errstr.append(mysql_error(&mysql));
    QMessageBox::critical(0, "Database Query problem", errstr);
  }
}

/**
 * getCreateDetails()
 * overrides Plugin::getCreateDetails()
 *
 * Called by main application to get the settings needed to create a new
 * chart based on this plugin.
 * We return two setttings: chart format and a list of symbols to create.
 */
  
Setting* MySQLPlugin::getCreateDetails ()
{
  QStringList l;
  l.append(tr("DOHLCV"));
  l.append(tr("DOHLCVI"));

  Setting *s = new Setting;
  s->set(tr("Title"), "", Setting::Text);
  s->set(tr("Symbol"), "", Setting::Text);
  s->set(tr("Format"), l[0], Setting::List);
  s->setList(tr("Format"), l);
  return s;
}

/**
 * createChart()
 * overrides Plugin::createChart()
 *
 * Create new charts from the given Settings (see getCreateDetails())
 */
void MySQLPlugin::createChart (Setting *setting)
{
  // process the settings we received from the 'new chart' dialog

  QString symbol = setting->getData(tr("Symbol"));
  QString title = setting->getData(tr("Title"));
  bool withOI = setting->getData(tr("Format")) == tr("DOHLCVI");

  // do some checking on the settings

  if (symbol.length() == 0)
    return;

  if (title.length() == 0)
    title = symbol;
  
  // get path to database
  
  QString dbpath = dataPath;
  dbpath.append("/Stocks");
  QDir dir(dbpath);
  
  if (!dir.exists() && !dir.mkdir(dbpath))
  {
    QString errstr = "MySQL plugin: unable to create directory: ";
    errstr.append(dbpath);
    QMessageBox::critical(0, "MySQL Plugin Error", errstr);
    return;
  }
  
  dbpath.append("/");
  dbpath.append(symbol);

  // create the new chart

  if (!dir.exists(dbpath)) 
  {
    ChartDb db;
    db.openChart(dbpath);

    Setting *details = db.getDetails();
    if (withOI)
      details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
    else
      details->set("Format", "Open|High|Low|Close|Volume", Setting::None);
    details->set("Chart Type", "Stock", Setting::None);
    details->set("Symbol", symbol, Setting::None);
    details->set("Source", pluginName, Setting::None);
    details->set("Title", title, Setting::Text);
    db.saveDetails();
    db.setFormat();
  }
  else
  {
    QMessageBox::message(0, "Information", "Symbol already present"); 
  }

  // add the added stock to the list we update
  
  QString list = getData(tr("Symbols"));
  list.append(" ");
  list.append(symbol);
  set(tr("Symbols"), list, Setting::Text);
}

/**
 * create()
 *
 * Return new instance of this plugin.
 */
Plugin * create ()
{
  MySQLPlugin *o = new MySQLPlugin;
  return ((Plugin *) o);
}


