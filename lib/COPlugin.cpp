/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "COPlugin.h"

COPlugin::COPlugin ()
{
  data = 0;
  menu = new QPopupMenu();
}

COPlugin::~COPlugin ()
{
  delete menu;
}

void COPlugin::draw (QPixmap &, Scaler &, int, int, int)
{
}

void COPlugin::prefDialog ()
{
}

void COPlugin::showMenu ()
{
}

void COPlugin::setData (BarData *d)
{
  data = d;
}

void COPlugin::newObject (QString &, QString &)
{
}

void COPlugin::addObject (Setting &)
{
}

COPlugin::Status COPlugin::pointerClick (QPoint &, BarDate &, double)
{
  return COPlugin::None;
}

void COPlugin::pointerMoving (QPixmap &, QPoint &, BarDate &, double)
{
}

void COPlugin::saveObjects (QString &)
{
}

void COPlugin::clear ()
{
}

void COPlugin::keyEvent (QKeyEvent *)
{
}

double COPlugin::getHigh ()
{
  return 0;
}

double COPlugin::getLow ()
{
  return 0;
}

QString COPlugin::getHelpFile ()
{
  return helpFile;
}

void COPlugin::getNameList (QStringList &)
{
}





