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
#include "Config.h"
#include <qdir.h>

QuotePlugin::QuotePlugin ()
{
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

bool QuotePlugin::setTFloat (QString d, bool flag)
{
  QString s = d;
  
  while (s.contains("A"))
    s = s.remove(s.find("A", 0, TRUE), 1);
  
  while (s.contains("B"))
    s = s.remove(s.find("B", 0, TRUE), 1);

  while (s.contains("K"))
  {
    s = s.remove(s.find("K", 0, TRUE), 1);
    s.append("000");
  }

  if (flag)
  {
    while (s.contains(","))
      s = s.replace(s.find(",", 0, TRUE), 1, ".");
  }
  else
  {
    while (s.contains(","))
      s = s.remove(s.find(",", 0, TRUE), 1);
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
  Config config;
  QString path = config.getData(Config::DataPath);
  
  QStringList l = QStringList::split("/", d, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    path.append("/");
    path.append(l[loop]);
    QDir dir(path);
    if (! dir.exists(path, TRUE))
    {
      if (! dir.mkdir(path, TRUE))
        return QString::null;
    }
  }

  return path;
}

QString QuotePlugin::getPluginName ()
{
  return pluginName;
}

QString QuotePlugin::getHelpFile ()
{
  return helpFile;
}

//**************************************************************************
//*************************** VIRTUAL OVERRIDES ****************************
//**************************************************************************

void QuotePlugin::update ()
{
}

void QuotePlugin::cancelUpdate ()
{
}

void QuotePlugin::prefDialog (QWidget *)
{
}

