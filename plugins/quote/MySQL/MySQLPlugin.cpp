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
    QStringList symbolList = QStringList::split(' ', symbols, FALSE);
    QStringList::const_iterator iter = symbolList.begin();

    while (iter != symbolList.end())
      updateSymbol(*iter++);

    closeDatabase();
  
    // if we get here, things went well, so we persist our settings
    storeSettings();
  }

  emit done();
  emit statusLogMessage(tr("Done"));
}

/**
 * updateSymbol(symbol)
 *
 * Update the quotes for given stock.
 */
void MySQLPlugin::updateSymbol(QString symbol) 
{
  emit statusLogMessage("Updating " + symbol);

  // verify and create the directory if needed  
  QString chartpath = dataPath;
  chartpath.append("/Stocks");
  QDir dir(chartpath);
  if (!dir.exists() && !dir.mkdir(chartpath))
  {
    QString errstr = "MySQL plugin: unable to create directory: ";
    errstr.append(chartpath);
    QMessageBox::critical(0, "MySQL Plugin Error", errstr);
    emit statusLogMessage("MySQL Plugin Error: "+ errstr);
    return;
  }
  chartpath.append("/");
  chartpath.append(symbol);

  // create the new chart

  ChartDb db;
  db.openChart(chartpath);
  QString s = db.getDetail(ChartDb::Symbol);
  if (! s.length())
  {
    db.setDetail(ChartDb::Symbol, symbol);
    db.setDetail(ChartDb::Type, "Stock");
    db.setDetail(ChartDb::Title, symbol);
    db.setDetail(ChartDb::BarType, QString::number(BarData::Daily));
  }

  QDate lastdate;

  if (incremental == TRUE) 
  {
    Bar *bar = db.getLastBar();
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

  doQuery(sql, db);
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
void MySQLPlugin::doQuery (QString sql, ChartDb& db)
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
      Bar *bar = new Bar;
      
      QString d = row[0];
      d = d.remove('-');
      d.append("000000");
      if (bar->setDate(d))
      {
        delete bar;
        emit statusLogMessage("Bad date " + d);
        continue;
      }
      
      d = row[1];
      bar->setOpen(d.toDouble());
      
      d = row[2];
      bar->setHigh(d.toDouble());
      
      d = row[3];
      bar->setLow(d.toDouble());
      
      d = row[4];
      bar->setClose(d.toDouble());
      
      d = row[5];
      bar->setVolume(d.toDouble());
      
      if (with_oi)
      {
        d = row[6];
        bar->setOI(d.toInt());
      }
      
      db.setBar(bar);
      
      emit dataLogMessage(db.getDetail(ChartDb::Symbol) + " " + bar->getString());
      delete bar;
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
  dialog->createPage (tr("Details"));
  dialog->addTextItem(tr("Database"), tr("Details"), database);
  dialog->addTextItem(tr("Host"), tr("Details"), host);
  dialog->addTextItem(tr("Username"), tr("Details"), username);
  dialog->addTextItem(tr("Password"), tr("Details"), password);
  dialog->addTextItem(tr("SQL Query"), tr("Details"), sqlquery);
  dialog->addTextItem(tr("Symbols"), tr("Details"), symbols);
  dialog->addCheckItem(tr("Incremental"), tr("Details"), incremental);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    database = dialog->getText(tr("Database"));
    host = dialog->getText(tr("Host"));
    username = dialog->getText(tr("Username"));
    password = dialog->getText(tr("Password"));
    sqlquery = dialog->getText(tr("SQL Query"));
    symbols = dialog->getText(tr("Symbols"));
    incremental = dialog->getCheck(tr("Incremental"));
  }
  
  delete dialog;
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


