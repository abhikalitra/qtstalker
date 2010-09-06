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

  QString s;
  if (v < 1000000)
    strip.strip(v, 4, s);
  else
  {
    bool flag = FALSE;
    if (v < 0)
    {
      flag = TRUE;
      v = v * 1;
    }

    if (v >= 1000000 && v < 1000000000)
    {
      s = QString::number(v / 1000000, 'f', 2);
      while (s.endsWith("0"))
        s.chop(1);
      while (s.endsWith("."))
        s.chop(1);
      s.append("M");
      if (flag)
        s.prepend("-");
    }
    else if (v >= 1000000000)
    {
      s = QString::number(v / 1000000000, 'f', 2);
      while (s.endsWith("0"))
        s.chop(1);
      while (s.endsWith("."))
        s.chop(1);
      s.append("B");
      if (flag)
        s.prepend("-");
    }
  }

  return s;
}

