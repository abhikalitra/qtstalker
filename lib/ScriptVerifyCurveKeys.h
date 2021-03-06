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

#ifndef SCRIPT_VERIFY_CURVE_KEYS_HPP
#define SCRIPT_VERIFY_CURVE_KEYS_HPP

#include "Entity.h"

#include <QList>

class ScriptVerifyCurveKeys
{
  public:
    ScriptVerifyCurveKeys ();
    int keys1 (Entity &in, QList<int> &keys);
    int keys2 (Entity &in, Entity &in2, QList<int> &keys);
    int keys3 (Entity &in, Entity &in2, Entity &in3, QList<int> &keys);
    int keys4 (Entity &in, Entity &in2, Entity &in3, Entity &in4, QList<int> &keys);
    int verifyKeys (Entity &e, int &minKey, int &pos);
    
};

#endif
