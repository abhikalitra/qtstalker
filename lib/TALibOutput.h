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

#ifndef TALIB_OUTPUT_HPP
#define TALIB_OUTPUT_HPP

#include <QList>

#include "ta_libc.h"
#include "Entity.h"

class TALibOutput
{
  public:
    TALibOutput ();
    int fillOutputDouble1 (Entity &, QList<int> &keys, int outNb, TA_Real out[]);
    int fillOutputDouble2 (Entity &, Entity &, QList<int> &keys, int outNb, TA_Real out[], TA_Real out2[]);
    int fillOutputDouble3 (Entity &, Entity &, Entity &, QList<int> &keys,
		           int outNb, TA_Real out[], TA_Real out2[], TA_Real out3[]);
    int fillOutputInteger1 (Entity &, QList<int> &keys, int outNb, TA_Integer out[]);
};

#endif
