/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#include "QSLog.h"
#include "Globals.h"

#include <QDateTime>

QSLog::QSLog ()
{
}

void QSLog::message (QSLog::Type type, QString mess)
{
  if (mess.isEmpty())
    return;

  QDateTime dt = QDateTime::currentDateTime();
  g_log << dt.toString("yyyy-MM-dd HH:mm:ss");

  switch (type)
  {
    case Info:
      break;
    case Error:
      g_log << " ERROR: ";
      break;
    case Critical:
      g_log << " *** CRITICAL ***: ";
      break;
    default:
      break;
  }
  
  g_log << mess << "\n";
  g_log.flush();
}

