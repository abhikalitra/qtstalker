/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "QtstalkerFormat.h"
#include "DbPlugin.h"
#include "Config.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstring.h>
#include <qdir.h>
#include <qsettings.h>
#include <qlabel.h>

QtstalkerFormat::QtstalkerFormat ()
{
  pluginName = "QtstalkerFormat";
  helpFile = "qtstalker.html";
  cancelFlag = FALSE;

  buildGui();  
  loadSettings();
  resize(400, 400);
}

QtstalkerFormat::~QtstalkerFormat ()
{
  saveSettings();
}

void QtstalkerFormat::update ()
{
  parse();
}

void QtstalkerFormat::buildGui ()
{
  setCaption(tr("QtstalkerFormat Prefs"));

  QLabel *label = new QLabel(tr("File Input"), baseWidget);
  grid->addWidget(label, 0, 0);

  QStringList l;
  button = new FileButton(baseWidget, l, lastPath);
  grid->addWidget(button, 0, 1);
  
  label = new QLabel(tr("Delete Chart"), baseWidget);
  grid->addWidget(label, 1, 0);

  check = new QCheckBox(baseWidget);
  grid->addWidget(check, 1, 1);
}

void QtstalkerFormat::parse ()
{
  QStringList list;
  button->getFile(list);

  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    QFile f(list[loop]);
    if (! f.open(IO_ReadOnly))
      continue;
    QTextStream stream(&f);

    // get the chartpath    
    QString path;
    while(stream.atEnd() == 0)
    {
      QString s = stream.readLine();
      s = s.stripWhiteSpace();
      if (! s.length())
        continue;

      QStringList l = QStringList::split("=", s, FALSE);
      if (l.count() < 2)
        continue;
	
      if (! l[0].compare("ChartPath"))
      {
        path = l[1];
	break;
      }
    }
    f.reset();
    
    if (! path.length())
    {
      f.close();
      QString ss(tr("ChartPath field not found. Skipping file"));
      printStatusLogMessage(ss);
      continue;
    }
      
    if (createDirectories(path))
    {
      f.close();
      QString ss(tr("CreateDirectories failed. Skipping file"));
      printStatusLogMessage(ss);
      continue;
    }
    
    // delete the db if it exists
    if (check->isChecked())
    {
      QDir dir;
      dir.remove(path, TRUE);
    }
    
    DbPlugin db;
    if (db.openChart(path))
    {
      QString ss(tr("Could not open db. Skipping file"));
      printStatusLogMessage(ss);
      db.close();
      f.close();
      continue;
    }
    
    QStringList l = QStringList::split("/", path, FALSE);
    QString symbol = l[l.count() - 1];
    
    QString s = tr("Updating ");
    s.append(symbol);
    printStatusLogMessage(s);
    
    QStringList co;
    
    while(stream.atEnd() == 0)
    {
      s = stream.readLine();
      s = s.stripWhiteSpace();

      if (! s.length())
        continue;

      QStringList l = QStringList::split("=", s, FALSE);
      if (l.count() < 2)
        continue;
	
      QString key = l[0];
      
      s = s.remove(0, key.length() + 1);
      
      while (1)
      {
        if (key.length() == 14)
	{
	  bool ok;
	  key.toDouble(&ok);
	  if (ok)
	    db.setData(key, s);
	  break;
	}
      
        if (! key.compare("BarType"))
	{
	  db.setHeaderField(DbPlugin::BarType, s);
	  break;
	}
        
	if (! key.compare("Plugin"))
	{
	  db.setHeaderField(DbPlugin::Plugin, s);
	  break;
	}
	
        if (! key.compare("Symbol"))
	{
	  db.setHeaderField(DbPlugin::Symbol, s);
	  break;
	}
	
        if (! key.compare("Title"))
	{
	  db.setHeaderField(DbPlugin::Title, s);
	  break;
	}
	
        if (! key.compare("Type"))
	{
	  db.setHeaderField(DbPlugin::Type, s);
	  break;
	}
	
        if (! key.compare("FuturesType"))
	{
	  db.setData(key, s);
	  break;
	}
        
        if (! key.compare("FuturesMonth"))
	{
	  db.setData(key, s);
	  break;
	}
	
	bool ok;
	int t = key.toInt(&ok);
	if (ok && t < 1000)
	{
          co.append(s);
	  break;
	}
	
	break;
      }
    }

    if (co.count())
    {
      QString t = co.join(",");
      db.setHeaderField(DbPlugin::CO, t);
    }
      
    f.close();
    db.close();
    
    if (cancelFlag)
    {
      cancelFlag = FALSE;
      downloadComplete();
      printStatusLogMessage(stringCanceled);
      return;
    }
  }

  downloadComplete();
  printStatusLogMessage(stringDone);
}

bool QtstalkerFormat::createDirectories (QString &d)
{
  Config config;
  QString path;
  config.getData(Config::DataPath, path);
  
  QStringList l = QStringList::split("/", d, FALSE);
  int loop = l.findIndex("data");
  if (! loop)
    return TRUE;
  loop++;
  
  QString s = path;
  for (; loop < (int) l.count() - 1; loop++)
  {
    s.append("/" + l[loop]);
    QDir dir;
    if (! dir.exists(s, TRUE))
    {
      if (! dir.mkdir(s, TRUE))
        return TRUE;
    }
  }

  return FALSE;
}

void QtstalkerFormat::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/QtstalkerFormat plugin");

  Config config;
  QString s;
  config.getData(Config::Home, s);
  lastPath = settings.readEntry("/lastPath", s + "/export");

  s = settings.readEntry("/deleteFlag", "1");
  if (s.toInt())
    check->setChecked(TRUE);
  else
    check->setChecked(FALSE);

  settings.endGroup();
}

void QtstalkerFormat::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/QtstalkerFormat plugin");

  QStringList list;
  button->getFile(list);
  if (list.count())
  {
    QFileInfo fi(list[0]);
    lastPath = fi.dirPath(TRUE);
  }
  settings.writeEntry("/lastPath", lastPath);

  settings.writeEntry("/deleteFlag", QString::number(check->isChecked()));

  settings.endGroup();
}

void QtstalkerFormat::cancelUpdate ()
{
  cancelFlag = TRUE;
}

//******************************************************************
//******************************************************************
//******************************************************************

QuotePlugin * createQuotePlugin ()
{
  QtstalkerFormat *o = new QtstalkerFormat;
  return ((QuotePlugin *) o);
}





