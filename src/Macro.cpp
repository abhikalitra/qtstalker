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

#include "Macro.h"
#include <qfile.h>
#include <qtextstream.h>

Macro::Macro (QString d)
{
  path = d;
  flag = FALSE;
}

Macro::~Macro ()
{
}

void Macro::save ()
{
  if (! path.length())
    return;
    
  QFile f(path);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);
  
  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
    stream << list[loop] << "\n";
  
  f.close();
}

void Macro::load ()
{
  QFile f(path);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  list.clear();
  
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();

    if (! s.length())
      continue;
      
    list.append(s);
  }

  f.close();
}

void Macro::record ()
{
  flag = TRUE;
  list.clear();
}

void Macro::stop ()
{
  flag = FALSE;
  save();
}

void Macro::recordKey (int zone, int state, int key, int asc, QString t)
{
  if (! flag)
    return;
    
  if (! key)
    return;
  
  QString s = QString::number(zone);
  s.append(",");

  s.append(QString::number(state));  
  s.append(",");

  s.append(QString::number(key));
  s.append(",");
  
  s.append(QString::number(asc));  
  s.append(",");
  
  s.append(t);  
  
  list.append(s);
}

void Macro::setKey (int i)
{
  QStringList l = QStringList::split(",", list[i], FALSE);
  zone = l[0].toInt();
  state = l[1].toInt();
  key = l[2].toInt();
  ascii = l[3].toInt();
  
  if (l.count() == 4)
    text = QString();
  else
    text = l[4];
}

QKeyEvent * Macro::getKey ()
{
  QKeyEvent *e = new QKeyEvent(QEvent::KeyPress,
                               key,
			       ascii,
			       state,
			       text);
  return e;			       
}

int Macro::getCount ()
{
  return (int) list.count();
}

int Macro::getZone ()
{
  return zone;
}

