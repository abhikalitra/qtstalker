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

#include "PlotScaleDraw.h"
#include "Strip.h"

#include <QString>
#include <QDebug>

PlotScaleDraw::PlotScaleDraw ()
{
}

QwtText PlotScaleDraw::label (double v) const
{
  Strip strip;

  double tv = v;
  
  int flag = 0;
  if (tv < 0)
  {
    flag = 1;
    tv = tv * 1;
  }

  QString s;
  if (tv < 1000)
    strip.strip(tv, 4, s);
  else
  {
    if (tv >= 1000 && tv < 1000000)
    {
      s = QString::number(tv / 1000, 'f', 2);
      while (s.endsWith("0"))
        s.chop(1);
      while (s.endsWith("."))
        s.chop(1);
      s.append("K");
    }
    else if (tv >= 1000000 && tv < 1000000000)
    {
      s = QString::number(tv / 1000000, 'f', 2);
      while (s.endsWith("0"))
        s.chop(1);
      while (s.endsWith("."))
        s.chop(1);
      s.append("M");
    }
    else if (tv >= 1000000000)
    {
      s = QString::number(tv / 1000000000, 'f', 2);
      while (s.endsWith("0"))
        s.chop(1);
      while (s.endsWith("."))
        s.chop(1);
      s.append("B");
    }
  }

//  if (flag)
//    s.prepend("-");
  
  return s;
}

