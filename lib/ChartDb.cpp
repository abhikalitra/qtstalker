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
#include <string.h>
#include <qfileinfo.h>
#include <qfile.h>

ChartDb::ChartDb ()
{
  db = 0;
  plug = 0;
  header = new ChartHeader;
  memset(header, 0, sizeof(ChartHeader));
}

ChartDb::~ChartDb ()
{
  if (plug)
  {
    plug->close();
    config.closePlugin(QString(header->plugin));
  }
    
  delete header;
}

int ChartDb::openChart (QString path)
{
  if (open(path))
    return TRUE;
    
  if (loadPlugin())
    return TRUE;

  plug->setDb(db, header);
  
  return FALSE;
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

void ChartDb::setBarString (QString d)
{
  if (! plug)
    return;
  
  plug->setBarString(d);
}

QStringList ChartDb::getChartObjectsList ()
{
  if (! plug)
    return QStringList();
    
  return plug->getChartObjectsList();
}

QPtrList<Setting> ChartDb::getChartObjects ()
{
  if (! plug)
  {
    QPtrList<Setting> l;
    return l;
  }

  return plug->getChartObjects();
}

void ChartDb::setChartObject (QString d, Setting *set)
{
  if (! plug)
    return;

  plug->setChartObject(d, set);
}

void ChartDb::deleteChartObject (QString d)
{
  if (! plug)
    return;

  plug->deleteChartObject(d);
}

void ChartDb::dump (QString d, bool f)
{
  if (! db)
    return;

  plug->dump(d, f);
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

void ChartDb::createNew (QString d)
{
  setPlugin(d);
  
  if (loadPlugin())
    return;
      
  path = plug->createNew();
  if (! path.length())
    return;

  if (open(path))
    return;
    
  plug->setDb(db, header);
  
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
    
  plug->setDb(db, header);
  plug->setDbPath(path);
  
  plug->dbPrefDialog();
}

void ChartDb::setPlugin (QString d)
{
  strncpy(header->plugin, d.ascii(), SSIZE);
}

int ChartDb::open (QString d)
{
  if (db)
  {
    qDebug("ChartDb::open: db already open");
    return TRUE;
  }

  bool flag = FALSE;  
  QFile file(d);
  if (! file.exists())
  {
    file.open(IO_WriteOnly);
    file.close();
    flag = TRUE;
  }
  
  if ((db = fopen(d.ascii(), "r+")) == NULL)
  {
    qDebug("ChartDb::open: can't open db");
    return TRUE;
  }

  rewind(db);
  
  if (flag)
  {
    strncpy(header->path, d.ascii(), PATHSIZE);
    
    if (! fwrite(header, sizeof(ChartHeader), 1, db))
      qDebug("write header failed");
  }
  else
  {
    if (! fread(header, sizeof(ChartHeader), 1, db))
      qDebug("read header failed");
  }
  
  path = d;
  
  return FALSE;
}

int ChartDb::loadPlugin ()
{
  if (! strlen(header->plugin))
  {
    if (! db)
    {
      qDebug("ChartDb::loadPlugin: db not open");
      return TRUE;
    }
    
    QString s = config.parseDbPlugin(path);
    if (s.length())
    {
      strncpy(header->plugin, s.ascii(), SSIZE);
    }
    else
    {
      qDebug("ChartDb::loadPlugin: can't resolve db plugin");
      return TRUE;
    }
  }
    
  plug = config.getDbPlugin(QString(header->plugin));
  if (! plug)
  {
    config.closePlugin(QString(header->plugin));
    qDebug("ChartDb::loadPlugin: can't open db plugin");
    return TRUE;
  }
  
  return FALSE;
}

void ChartDb::saveDbDefaults (Setting *set)
{
  if (! plug)
    return;
    
  plug->saveDbDefaults(set);
}

QString ChartDb::getSymbol ()
{
  return QString(header->symbol);
}

void ChartDb::setSymbol (QString d)
{
  if (! plug)
    return;
    
  plug->setSymbol(d);
}

QString ChartDb::getTitle ()
{
  return QString(header->title);
}

void ChartDb::setTitle (QString d)
{
  if (! plug)
    return;
    
  plug->setTitle(d);
}

QString ChartDb::getType()
{
  return QString(header->type);
}

void ChartDb::setType (QString d)
{
  if (! plug)
    return;
    
  plug->setType(d);
}

QString ChartDb::getFuturesType()
{
  return QString(header->futuresType);
}

void ChartDb::setFuturesType (QString d)
{
  if (! plug)
    return;
    
  plug->setFuturesType(d);
}

void ChartDb::deleteBar (QString d)
{
  if (! plug)
    return;
    
  plug->deleteBar(d);
}

Bar * ChartDb::getBar (QString d)
{
  if (! plug)
    return 0;
    
  return plug->getBar(d);
}

void ChartDb::setHeaderCO (QString d)
{
  if (! plug)
    return;
    
  plug->setHeaderCO(d);
}

void ChartDb::setFuturesMonth (QString d)
{
  if (! plug)
    return;
    
  plug->setFuturesMonth(d);
}

QString ChartDb::getFuturesMonth ()
{
  return header->futuresMonth;
}

void ChartDb::setHeader (Setting *d)
{
  if (! plug)
    return;
    
  plug->setHeader(d);
}

void ChartDb::setHeaderFundamental (QString d)
{
  if (! plug)
    return;
    
  plug->setHeaderFundamental(d);
}

QString ChartDb::getHeaderFundamental ()
{
  return header->lvar1;
}

