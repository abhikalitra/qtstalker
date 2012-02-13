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

#include "KeyDialog.h"

#include <QtDebug>

KeyDialog::KeyDialog ()
{
  _list << "STRING_0";
  _list << "STRING_1";
  _list << "STRING_2";
  _list << "INTEGER_0";
  _list << "INTEGER_1";
  _list << "INTEGER_2";
  _list << "DOUBLE_0";
  _list << "DOUBLE_1";
  _list << "DOUBLE_2";
  _list << "COLOR_0";
  _list << "COLOR_1";
  _list << "COLOR_2";
  _list << "COLOR_3";
  _list << "BOOL_0";
  _list << "BOOL_1";
  _list << "BOOL_2";
  _list << "FILE_0";
  _list << "FILE_1";
  _list << "FILE_2";
  _list << "MA_0";
  _list << "MA_1";
  _list << "MA_2";
}
