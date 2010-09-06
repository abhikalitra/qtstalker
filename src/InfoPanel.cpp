/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "InfoPanel.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <cmath>

InfoPanel::InfoPanel ()
{
  setReadOnly(TRUE);
}

void InfoPanel::showInfo (Setting d)
{
  // list bar values first
  QStringList l;
  l << "D" << "T" << "O" << "H" << "L" << "C" << "VOL" << "OI";
  
  QString s, s2, str;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    d.getData(l[loop], s);
    if (s.length())
    {
      str.append(l[loop] + " " + s + "\n");
      d.remove(l[loop]);
    }
  }

  d.getKeyList(l);
  l.sort();
  
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    d.getData(l[loop], s);
    
    // If it is a big number, then use zero precision.
    bool ok;
    double sn = s.toDouble(&ok);
    if (ok)
    {
      if (fabs(sn) > 1000)
        s = QString::number(sn, 'f', 0);
    }
    
    str.append(l[loop] + " " + s + "\n");
  }

  setText(str);
}

