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

#include "QuotePlugin.h"
#include <qdir.h>

QuotePlugin::QuotePlugin ()
{
  pluginType = "Quote";
  saveFlag = FALSE;
}

QuotePlugin::~QuotePlugin ()
{
}

QString QuotePlugin::stripJunk (QString d)
{
  QString s = d.stripWhiteSpace();

  while (1)
  {
    int p = s.find('"', 0, TRUE);
    if (p == -1)
      break;
    else
      s.remove(p, 1);
  }

  return s;
}

bool QuotePlugin::setTFloat (QString d)
{
  QString s = d;
  int p = s.find("A", 0, TRUE);
  if (p != -1)
    s.remove(p, 1);

  p = s.find("B", 0, TRUE);
  if (p != -1)
    s.remove(p, 1);

  p = s.find("K", 0, TRUE);
  if (p != -1)
  {
    s.remove(p, 1);
    s.append("000");
  }

  while (s.contains(","))
  {
    int pos = s.find(",", 0, TRUE);
    s.remove(pos, 1);
  }

  bool ok;
  tfloat = s.toFloat(&ok);
  if (! ok)
    return TRUE;
  else
    return FALSE;
}

QString QuotePlugin::createDirectory (QString d)
{
  QString path = dataPath;
  path.append("/");
  path.append(d);

  QDir dir(path);
  if (! dir.exists(path, TRUE))
  {
    if (! dir.mkdir(path, TRUE))
      return QString::null;
  }

  return path;
}

void QuotePlugin::setDataLogMessage (Setting *r)
{
  QString s;
  QStringList l = r->getKeyList();
  
  s = "D ";
  s.append(r->getData("Date"));

  if (l.findIndex("Open") != -1)
  {
    s.append(" O ");
    s.append(r->getData("Open"));
  }

  if (l.findIndex("High") != -1)
  {
    s.append(" H ");
    s.append(r->getData("High"));
  }

  if (l.findIndex("Low") != -1)
  {
    s.append(" L ");
    s.append(r->getData("Low"));
  }

  if (l.findIndex("Close") != -1)
  {
    s.append(" C ");
    s.append(r->getData("Close"));
  }

  if (l.findIndex("Volume") != -1)
  {
    s.append(" V ");
    s.append(r->getData("Volume"));
  }

  if (l.findIndex("Open Interest") != -1)
  {
    s.append(" OI ");
    s.append(r->getData("Open Interest"));
  }
 
  emit dataLogMessage(s);           
}



