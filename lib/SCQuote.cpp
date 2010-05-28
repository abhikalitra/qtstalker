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

#include "SCQuote.h"
#include "DBPlugin.h"

#include <QtDebug>

SCQuote::SCQuote ()
{
}

int SCQuote::calculate (QStringList &l, QByteArray &ba, QString &path, Indicator &ind)
{
  // format = QUOTE,PLUGIN,*
  // we are only concerned with the first 2 parms, the plugin will check the rest
  
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "SCQuote::calculate: invalid parm count" << l.count();
    return 1;
  }
  
  DBPlugin *plug = fac.getDB(path, l[1]);
  if (! plug)
  {
    qDebug() << "SCQuote::calculate: plugin error" << l[1];
    return 1;
  }
  
  int rc = plug->scriptCommand(l, ind);
  if (! rc)
  {
    ba.clear();
    ba.append("0\n");
  }
  
  return rc;
}

