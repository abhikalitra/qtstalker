/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef TALIB_FUNCTION_HPP
#define TALIB_FUNCTION_HPP

#include <QList>
#include <QString>

#include "Entity.h"
#include "ta_libc.h"

class TALibFunction
{
  public:
    TALibFunction ();
    int run (Entity &parms, int inNum, Entity &input, Entity &input2, Entity &input3,
	     Entity &input4, int outNum, Entity &line, Entity &line2, Entity &line3);
};

#endif
