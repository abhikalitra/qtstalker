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

#include "MacroKey.h"
#include <qstringlist.h>

MacroKey::MacroKey (int z, int s, int k, int a, QString t)
{
  zone = z;
  state = s;
  key = k;
  ascii = a;
  text = t;
}

MacroKey::MacroKey ()
{
  zone = 0;
  state = 0;
  key = 0;
  ascii = 0;
}

MacroKey::~MacroKey ()
{
}

void MacroKey::setString (QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  zone = l[0].toInt();
  state = l[1].toInt();
  key = l[2].toInt();
  ascii = l[3].toInt();
  
  if (l.count() == 4)
    text = QString();
  else
    text = l[4];
}

QString MacroKey::getString ()
{
  QString s = QString::number(zone);
  s.append(",");

  s.append(QString::number(state));  
  s.append(",");

  s.append(QString::number(key));
  s.append(",");
  
  s.append(QString::number(ascii));  
  s.append(",");
  
  s.append(text);  

  return s;
}

QKeyEvent * MacroKey::getKeyEvent ()
{
  QKeyEvent *e = new QKeyEvent(QEvent::KeyPress,
                               key,
			       ascii,
			       state,
			       text);
  return e;			       
}

int MacroKey::getZone ()
{
  return zone;
}

int MacroKey::getState ()
{
  return state;
}

int MacroKey::getKey ()
{
  return key;
}

int MacroKey::getAscii ()
{
  return ascii;
}

QString MacroKey::getText ()
{
  return text;
}

