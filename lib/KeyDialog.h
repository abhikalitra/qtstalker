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

// *************************************************************************************************
// *************************************************************************************************

#ifndef KEY_DIALOG_HPP
#define KEY_DIALOG_HPP

#include "Type.h"

class KeyDialog : public Type
{
  public:
    enum Key
    {
      _STRING_0,
      _STRING_1,
      _STRING_2,
      _INTEGER_0,
      _INTEGER_1,
      _INTEGER_2,
      _DOUBLE_0,
      _DOUBLE_1,
      _DOUBLE_2,
      _COLOR_0,
      _COLOR_1,
      _COLOR_2,
      _COLOR_3,
      _BOOL_0,
      _BOOL_1,
      _BOOL_2,
      _FILE_0,
      _FILE_1,
      _FILE_2,
      _MA_0,
      _MA_1,
      _MA_2
    };
    
    KeyDialog ();
};

#endif
