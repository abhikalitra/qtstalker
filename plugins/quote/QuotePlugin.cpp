/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

QuotePlugin::QuotePlugin ()
{
  pluginType = "Quote";
  createFlag = FALSE;
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

bool QuotePlugin::getCreateFlag ()
{
  return createFlag;
}

