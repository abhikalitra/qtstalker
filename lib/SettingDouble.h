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

#ifndef SETTING_DOUBLE_HPP
#define SETTING_DOUBLE_HPP

#include "Setting.h"

class SettingDouble : public Setting
{
  public:
    SettingDouble (int input, int output, double d, double high, double low);
    SettingDouble (double d);
    double getDouble ();
    void setDouble (double);
    QString toString ();
    int fromString (QString);
    double getDoubleHigh ();
    double getDoubleLow ();

  private:
    double _data;
    double _high;
    double _low;
};

#endif
