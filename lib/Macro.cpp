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

#include "Macro.h"
#include <qfile.h>
#include <qtextstream.h>

Macro::Macro (QString &d)
{
  path = d;
  flag = FALSE;
  list.setAutoDelete(TRUE);
  index = 0;
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
  
  QPtrListIterator<MacroKey> it(list);
  MacroKey *mk = 0;
  while ((mk = it.current()) != 0)
  {
    stream << mk->getString() << "\n";
    ++it;
  }  
  
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
      
    MacroKey *mk = new MacroKey();
    mk->setString(s);
    list.append(mk);
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
  list.remove((unsigned int) 0);
  list.remove(list.count() - 1);
  save();
}

void Macro::recordKey (int z, int s, int k, int a, QString &t)
{
  if (! flag)
    return;
    
  if (z == 0 || k == 0)
    return;
  
  list.append(new MacroKey(z, s, k, a, t));
}

QKeyEvent * Macro::getKey (int i)
{
  MacroKey *mk = list.at(i);
  if (! mk)
    return 0;

  return mk->getKeyEvent();			       
}

int Macro::getCount ()
{
  return (int) list.count();
}

int Macro::getZone (int i)
{
  MacroKey *mk = list.at(i);
  if (mk)
    return mk->getZone();
  else
    return -1;
}

void Macro::setIndex (int i)
{
  index = i;
}

int Macro::getIndex ()
{
  return index;
}

void Macro::incIndex ()
{
  index++;
}

