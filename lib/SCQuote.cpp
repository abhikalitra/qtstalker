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
#include "BarData.h"
#include "PluginFactory.h"

#include <QtDebug>

SCQuote::SCQuote ()
{
}

void SCQuote::calculate (QStringList &l, QByteArray &ba, QString &pp)
{
  // determine what function is desired and call it
  
  ba.clear();
  ba.append("ERROR\n");
  
  if (l.count() < 1)
  {
    qDebug() << "SCQuote::calculate: invalid parm count" << l.count();
    return;
  }
  
  if (l[0] == "QUOTE_SET")
    quoteSet(l, ba, pp);

  if (l[0] == "QUOTE_SET_NAME")
    quoteSetName(l, ba, pp);
}

void SCQuote::quoteSet (QStringList &l, QByteArray &ba, QString &pp)
{
  // format = QUOTE_SET,PLUGIN,EXCHANGE,SYMBOL,DATE_FORMAT,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME*
  // we are only concerned with the first 2 parms, the plugin will check the rest

  if (l.count() < 2)
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

void SCQuote::quoteSetName (QStringList &l, QByteArray &ba, QString &)
{
  // format = QUOTE_SET_NAME,EXCHANGE,SYMBOL,NAME

  if (l.count() != 4)
  {
    qDebug() << "SCQuote::quoteSetName: invalid parm count" << l.count();
    return;
  }
  
  DBPlugin db;
  BarData bd;
  bd.setExchange(l[1]);
  bd.setSymbol(l[2]);
  if (db.getIndexData(bd))
  {
    qDebug() << "SCQuote::quoteSetName: symbol not found in database" << l[1] << l[2];
    return;
  }
  
  bd.setName(l[3]);
  if (db.setIndexData(bd))
  {
    qDebug() << "SCQuote::quoteSetName: error setting index";
    return;
  }
  
  ba.clear();
  ba.append("0\n");
}

