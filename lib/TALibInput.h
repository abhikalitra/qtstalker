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

#ifndef TALIB_INPUT_HPP
#define TALIB_INPUT_HPP

#include <QList>

#include "ta_libc.h"
#include "Data.h"

class TALibInput
{
  public:
    TALibInput ();
    int fill (QList<Data *> &, QList<int> &, TA_Real out[], TA_Real out2[], TA_Real out3[], TA_Real out4[]);
    int fill1 (QList<Data *> &list, QList<int> &keys, TA_Real out[]);
    int fill2 (QList<Data *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[]);
    int fill3 (QList<Data *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[], TA_Real out3[]);
    int fill4 (QList<Data *> &list, QList<int> &keys, TA_Real out[], TA_Real out2[], TA_Real out3[], TA_Real out4[]);
};

#endif
