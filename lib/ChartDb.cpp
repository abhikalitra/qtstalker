/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "ChartDb.h"
#include <qtextstream.h>
#include <qfile.h>
#include <qfileinfo.h>

ChartDb::ChartDb ()
{
  db = 0;
  plug = 0;
}

ChartDb::~ChartDb ()
{
  if (plug)
    config.closePlugin(dbPlugin);
    
  if (db)
    db->close(db, 0);
}

int ChartDb::openChart (QString path)
{
  if (open(path))
    return TRUE;
    
  if (loadPlugin())
    return TRUE;

  plug->setDb(db);
  
  return FALSE;
}

QString ChartDb::getData (QString k)
{
  if (! plug)
    return QString();
    
  return plug->getData(k);
}

void ChartDb::setData (QString k, QString d)
{
  if (! plug)
    return;
    
  plug->setData(k, d);
}

void ChartDb::deleteData (QString k)
{
  if (! plug)
    return;
  
  plug->deleteData(k);
}

BarData * ChartDb::getHistory ()
{
  if (! plug)
    return 0;
    
  return plug->getHistory();
}

void ChartDb::setBar (Bar *bar)
{
  if (! plug)
    return;
  
  plug->setBar(bar);
}

QStringList ChartDb::getChartObjectsList ()
{
  if (! plug)
    return QStringList();

  return plug->getChartObjectsList();
}

QPtrList<Setting> * ChartDb::getChartObjects ()
{
  if (! plug)
    return 0;
  
  return plug->getChartObjects();
}

void ChartDb::setChartObject (QString d, Setting *set)
{
  if (! plug)
    return;

  plug->setChartObject (d, set);
}

void ChartDb::deleteChartObject (QString d)
{
  if (! plug)
    return;

  plug->deleteChartObject(d);
}

void ChartDb::dump (QString d)
{
  if (! db)
    return;

  QFile outFile(d);
  if (! outFile.open(IO_WriteOnly))
    return;
  QTextStream outStream(&outFile);

  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cursor, 0);

  while (! cursor->c_get(cursor, &key, &data, DB_NEXT))
    outStream << (char *) key.data << "=" << (char *) data.data << "\n";

  cursor->c_close(cursor);
  outFile.close();
}

void ChartDb::setBarCompression (BarData::BarCompression d)
{
  if (! plug)
    return;

  plug->setBarCompression(d);
}

void ChartDb::setBarRange (int d)
{
  if (! plug)
    return;
  
  plug->setBarRange(d);
}

Bar * ChartDb::getLastBar ()
{
  if (! plug)
    return 0;
    
  return plug->getLastBar();
}

Bar * ChartDb::getFirstBar ()
{
  if (! plug)
    return 0;

  return plug->getFirstBar();
}

Bar * ChartDb::getBar (QString k, QString d)
{
  if (! plug)
    return 0;
    
  return plug->getBar(k, d);
}

void ChartDb::createNew (QString d)
{
  dbPlugin = d;
  if (loadPlugin())
    return;
      
  path = plug->createNew();
  if (! path.length())
    return;

  if (open(path))
    return;
    
  plug->setDb(db);
  
  QFileInfo fi(path);
  Setting *set = new Setting;
  set->setData("BarType", QString::number(BarData::Daily));
  set->setData("Symbol", fi.fileName());
  set->setData("Title", fi.fileName());
  plug->saveDbDefaults(set);
  delete set;
  
  plug->dbPrefDialog();
}

void ChartDb::dbPrefDialog (QString d)
{
  if (open(d))
    return;

  if (loadPlugin())
    return;
    
  plug->setDb(db);
  plug->setDbPath(path);
  
  plug->dbPrefDialog();
}

void ChartDb::setPlugin (QString d)
{
  dbPlugin = d;
}

int ChartDb::open (QString d)
{
  if (db)
  {
    qDebug("ChartDb::open: db already open");
    return TRUE;
  }
  
  if (db_open((char *) d.latin1(), DB_BTREE, DB_CREATE, 0664, NULL, NULL, &db) != 0)
  {
    qDebug("ChartDb::open: can't open db");
    return TRUE;
  }
  
  path = d;
  return FALSE;
}

int ChartDb::loadPlugin ()
{
  if (! dbPlugin.length())
  {
    if (! db)
    {
      qDebug("ChartDb::loadPlugin: db not open");
      return TRUE;
    }
    
    DBT key;
    DBT data;
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    QString k = "Plugin";
    key.data = (char *) k.latin1();
    key.size = k.length() + 1;

    if (db->get(db, NULL, &key, &data, 0) == 0)
      dbPlugin = (char *) data.data;
    
    if (! dbPlugin.length())
    {
      dbPlugin = config.parseDbPlugin(path);
      if (dbPlugin)
      {
        key.data = (char *) k.latin1();
        key.size = k.length() + 1;
        data.data = (char *) dbPlugin.latin1();
        data.size = dbPlugin.length() + 1;
        db->put(db, NULL, &key, &data, 0);
      }
      else
      {
        qDebug("ChartDb::loadPlugin: can't resolve db plugin");
        return TRUE;
      }
    }
  }
    
  plug = config.getDbPlugin(dbPlugin);
  if (! plug)
  {
    config.closePlugin(dbPlugin);
    qDebug("ChartDb::loadPlugin: can't open db plugin");
    return TRUE;
  }
  
  return FALSE;
}

void ChartDb::saveDbDefaults (Setting *set)
{
  if (! plug)
    return;
    
  plug->saveDbDefaults (set);
}

