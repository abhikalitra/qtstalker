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
#include "PluginFactory.h"

#include <QtDebug>

SCQuote::SCQuote ()
{
}

void SCQuote::calculate (QStringList &l, QByteArray &ba, QString &pp)
{
  // format = QUOTE,PLUGIN,METHOD,*
  // we are only concerned with the first 3 parms, the plugin will check the rest
  
  ba.clear();
  ba.append("1\n");

  if (l.count() < 3)
  {
    qDebug() << "SCQuote::quoteSet: invalid parm count" << l.count();
    return;
  }
  
  PluginFactory fac;
  DBPlugin *plug = fac.getDB(pp, l[1]);
  if (! plug)
  {
    qDebug() << "SCQuote::quoteSet: plugin error" << l[1];
    return;
  }
  
  int rc = plug->scriptCommand(l);
  if (! rc)
  {
    ba.clear();
    ba.append("0\n");
  }
}

