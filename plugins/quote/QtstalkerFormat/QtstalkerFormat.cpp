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

#include "QtstalkerFormat.h"
#include "ChartDb.h"
#include "PrefDialog.h"
#include "Config.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstring.h>
#include <qdir.h>
#include <qsettings.h>

QtstalkerFormat::QtstalkerFormat ()
{
  pluginName = "QtstalkerFormat";
  helpFile = "qtstalker.html";
  method = "Qtstalker";
  cancelFlag = FALSE;
  
  loadSettings();
}

QtstalkerFormat::~QtstalkerFormat ()
{
}

void QtstalkerFormat::update ()
{
  QTimer::singleShot(250, this, SLOT(parse()));
}

void QtstalkerFormat::parse ()
{
  if (! method.compare("DB"))
    importDB();
  else
    importNative();
}

void QtstalkerFormat::importDB ()
{
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
      emit statusLogMessage("ChartPath field not found. Skipping file.");
      continue;
    }
      
    if (createDirectories(path))
    {
      f.close();
      emit statusLogMessage("CreateDirectories failed. Skipping file.");
      continue;
    }
    
    // delete the db if it exists
    QDir dir;
    dir.remove(path, TRUE);
    
    ChartDb *db = new ChartDb;
    if (db->openChart(path))
    {
      emit statusLogMessage("Could not open db. Skipping file.");
      delete db;
      f.close();
      continue;
    }
    
    QStringList l = QStringList::split("/", path, FALSE);
    QString symbol = l[l.count() - 1];
    
    QString s = tr("Updating ");
    s.append(symbol);
    emit statusLogMessage(s);
    
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
	  {
	    QString s2 = key + "," + s;
	    db->setBarString(s2);
	  }
	  break;
	}
      
        if (! key.compare("Plugin"))
	{
	  db->setPlugin(s);
	  break;
	}
	
        if (! key.compare("Symbol"))
	{
	  db->setSymbol(s);
	  break;
	}
	
        if (! key.compare("Title"))
	{
	  db->setTitle(s);
	  break;
	}
	
        if (! key.compare("Type"))
	{
	  db->setType(s);
	  break;
	}
	
        if (! key.compare("FuturesType"))
	{
	  db->setFuturesType(s);
	  break;
	}
        
        if (! key.compare("FuturesMonth"))
	{
	  db->setFuturesMonth(s);
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
      db->setHeaderCO(co.join(","));
      
    f.close();
    delete db;
    
    if (cancelFlag)
    {
      cancelFlag = FALSE;
      emit done();
      emit statusLogMessage("Canceled");
      return;
    }
  }

  emit done();
  emit statusLogMessage(tr("Done"));
}

void QtstalkerFormat::importNative ()
{
  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    QFile f(list[loop]);
    if (! f.open(IO_ReadOnly))
      continue;
    QTextStream stream(&f);

    Setting *set = new Setting;
    QString s;
    while(stream.atEnd() == 0)
    {
      s = stream.readLine();
      s = s.stripWhiteSpace();
      if (! s.length())
        continue;
      
      if (! s.contains("="))
        break;
	
      QStringList l = QStringList::split("=", s, FALSE);
      QString s2 = s;
      s2 = s2.remove(0, l[0].length() + 1);
      set->setData(l[0], s2);
    }
      
    if (createDirectories(set->getData("Path")))
    {
      f.close();
      emit statusLogMessage("CreateDirectories failed. Skipping file.");
      delete set;
      continue;
    }
    
    // delete the db if it exists
    QDir dir;
    dir.remove(set->getData("Path"), TRUE);
    
    ChartDb *db = new ChartDb;
    db->setPlugin(set->getData("Plugin"));
    if (db->openChart(set->getData("Path")))
    {
      emit statusLogMessage("Could not open db. Skipping file.");
      delete db;
      delete set;
      f.close();
      continue;
    }
    
    db->setHeader(set);
    db->setBarString(s); // left over from the header loading above
    
    s = tr("Updating ");
    s.append(set->getData("Symbol"));
    emit statusLogMessage(s);
    
    while(stream.atEnd() == 0)
    {
      s = stream.readLine();
      s = s.stripWhiteSpace();
      if (! s.length())
        continue;

      db->setBarString(s);
    }

    f.close();
    delete db;
    delete set;
    
    if (cancelFlag)
    {
      cancelFlag = FALSE;
      emit done();
      emit statusLogMessage("Canceled");
      return;
    }
  }

  emit done();
  emit statusLogMessage(tr("Done"));
}

bool QtstalkerFormat::createDirectories (QString d)
{
  Config config;
  QString path = config.getData(Config::DataPath);
  
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

void QtstalkerFormat::prefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("QtstalkerFormat Prefs"));
  dialog->createPage (tr("Details"));
  dialog->setHelpFile(helpFile);
  
  QStringList l;
  l.append("DB");
  l.append("Qtstalker");
  dialog->addComboItem(tr("Method"), tr("Details"), l, method);

  Config config;  
  QString s = config.getData(Config::Home) + "/export";
  dialog->addFileItem(tr("File Input"), tr("Details"), list, s);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    list = dialog->getFile(tr("File Input"));
    method = dialog->getCombo(tr("Method"));
    saveFlag = TRUE;
  }
  
  delete dialog;
}

void QtstalkerFormat::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/QtstalkerFormat plugin");

  method = settings.readEntry("/Method", "Qtstalker");
  
  settings.endGroup();
}

void QtstalkerFormat::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/QtstalkerFormat plugin");
  
  settings.writeEntry("/Method", method);
  
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





