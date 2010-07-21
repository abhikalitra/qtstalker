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

#include "Splitter.h"
#include "Config.h"

#include <QDebug>

Splitter::Splitter (int d)
{
  _parm = d;
}

void Splitter::load ()
{
  Config config;
  QString s;
  config.getData((Config::Parm) _parm, s);
  if (s.isEmpty())
  {
    QList<int> l;
    l << 437 << 20 << 200;
    
    setSizes(l);
    config.setData((Config::Parm) _parm, (QSplitter *) this);
  }
  else
    config.getData((Config::Parm) _parm, (QSplitter *) this);
}

void Splitter::save ()
{
  Config config;
  config.setData((Config::Parm) _parm, (QSplitter *) this);
}

